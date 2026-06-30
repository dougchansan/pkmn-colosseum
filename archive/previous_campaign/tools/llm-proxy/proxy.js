#!/usr/bin/env node
// Local routing proxy for Claude Code.
//
// Routes each request to either Anthropic (Opus/Sonnet/Haiku) or a GLM
// Anthropic-compatible endpoint (Z.ai / BigModel), based on the `model`
// field in the request body. Lets you flip between Opus and GLM live with
// `/model` inside a single Claude Code terminal.
//
//   - model starting with "glm"  -> GLM upstream (swaps in GLM_API_KEY)
//   - anything else              -> Anthropic upstream (passes auth through
//                                   untouched, so your existing subscription
//                                   or API-key auth keeps working)
//
// Usage:
//   GLM_API_KEY=xxxxx node tools/llm-proxy/proxy.js
// then point Claude Code at it:
//   ANTHROPIC_BASE_URL=http://127.0.0.1:8787 claude
//
// Env vars:
//   PORT              listen port            (default 8787)
//   GLM_API_KEY       your Z.ai / GLM key    (required to use GLM)
//   GLM_UPSTREAM      GLM base url           (default https://api.z.ai/api/anthropic)
//   ANTHROPIC_UPSTREAM Anthropic base url    (default https://api.anthropic.com)
//   GLM_MODEL_PREFIX  route prefix           (default "glm")
//   LOG               set to "0" to silence routing log

const http = require("http");
const https = require("https");
const fs = require("fs");
const path = require("path");
const { URL } = require("url");

// GLM key: prefer the environment, else a gitignored .glm_key file next to this
// script. The file path keeps the secret out of command lines / process args.
function resolveGlmKey() {
  if (process.env.GLM_API_KEY) return process.env.GLM_API_KEY;
  try {
    const f = path.join(__dirname, ".glm_key");
    if (fs.existsSync(f)) return fs.readFileSync(f, "utf8").replace(/^﻿/, "").trim();
  } catch (_) {
    /* unreadable file — treated as no key */
  }
  return "";
}

const PORT = parseInt(process.env.PORT || "8787", 10);
const GLM_API_KEY = resolveGlmKey();
const GLM_UPSTREAM = process.env.GLM_UPSTREAM || "https://api.z.ai/api/anthropic";
const ANTHROPIC_UPSTREAM = process.env.ANTHROPIC_UPSTREAM || "https://api.anthropic.com";
const GLM_MODEL_PREFIX = (process.env.GLM_MODEL_PREFIX || "glm").toLowerCase();
// Whenever a GLM model is selected, the wire request is pinned to this id.
// The "[1m]" tag selects the 1M-context variant; it's stripped off the model
// id and translated into the context-1m beta header (see splitModelTag).
const GLM_MODEL = process.env.GLM_MODEL || "glm-5.2[1m]";
const LOG = process.env.LOG !== "0";

function log(...args) {
  if (LOG) console.error(`[proxy ${new Date().toISOString()}]`, ...args);
}

// --- True context-usage accounting -----------------------------------------
// The Claude Code in-TUI context meter divides by the BASE window (200K) even
// when the [1m] beta is active, and has no idea what window GLM uses — so it
// reads "100%" long before the real 1M window is full. We measure the TRUE
// usage here (we already see every upstream `usage` block) and write it to a
// status file the renaming dashboard reads. This is authoritative, the meter is
// not. See README "context accounting".
const CTX_FILE = path.join(__dirname, "context_usage.json");
const WINDOW_1M = 1000000;
const WINDOW_BASE = 200000;
const ctxState = {}; // sessionKey -> { model, fullInput, window, pct, ts, target }

function windowFor(betaHeader, model) {
  const beta = String(betaHeader || "");
  if (beta.includes("context-1m")) return WINDOW_1M;
  if (/\[1m\]/.test(String(model))) return WINDOW_1M;
  return WINDOW_BASE;
}

// Pull the prompt-side token total out of an SSE `message_start` event. The full
// context size = input + cache-read + cache-creation tokens (everything the
// model had to read), which is what actually fills the window.
function parseStartUsage(text) {
  const m = text.match(/"message_start"[\s\S]*?"usage"\s*:\s*\{([^}]*)\}/);
  if (!m) return null;
  const blk = m[1];
  const num = (k) => {
    const mm = blk.match(new RegExp('"' + k + '"\\s*:\\s*(\\d+)'));
    return mm ? parseInt(mm[1], 10) : 0;
  };
  return num("input_tokens") + num("cache_read_input_tokens") + num("cache_creation_input_tokens");
}

// Capture provider rate-limit headers from a response so the dashboard can show
// live lockout status + reset times. We don't hard-code Anthropic's exact unified
// header names (they evolve) — we grab every ratelimit/reset/quota header and
// store it raw, keyed by upstream. Anthropic Max returns anthropic-ratelimit-*
// (incl. the unified 5h/weekly status + reset); z.ai returns its own.
const LIMITS_FILE = path.join(__dirname, "usage_limits.json");
const limitsState = {}; // target -> { headers:{...}, ts }

function recordLimits(target, headers) {
  const grabbed = {};
  for (const k of Object.keys(headers || {})) {
    const lk = k.toLowerCase();
    if (lk.includes("ratelimit") || lk.includes("rate-limit") ||
        lk.endsWith("-reset") || lk.includes("quota") || lk.includes("usage")) {
      grabbed[lk] = headers[k];
    }
  }
  if (!Object.keys(grabbed).length) return;
  limitsState[target] = { headers: grabbed, ts: new Date().toISOString() };
  try {
    fs.writeFileSync(LIMITS_FILE, JSON.stringify(limitsState, null, 2));
  } catch (_) { /* dashboard tolerates stale */ }
}

function recordUsage(sessionKey, model, target, fullInput, window) {
  const pct = window ? Math.round((fullInput / window) * 1000) / 10 : 0;
  ctxState[sessionKey] = {
    model, target, fullInput, window, pct,
    ts: new Date().toISOString(),
  };
  try {
    fs.writeFileSync(CTX_FILE, JSON.stringify(ctxState, null, 2));
  } catch (_) { /* dashboard will just show stale data */ }
  if (LOG) log(`CTX ${sessionKey} ${model} ${fullInput}/${window} tok = ${pct}% (TRUE) -> ${target}`);
}

// Split a trailing "[tag]" off a model id and map known tags to beta flags.
// e.g. "glm-5.2[1m]" -> { id: "glm-5.2", betas: ["context-1m-2025-08-07"] }
function splitModelTag(model) {
  const m = String(model).match(/^(.*?)\s*\[([^\]]+)\]\s*$/);
  if (!m) return { id: model, betas: [] };
  const id = m[1];
  const tag = m[2].toLowerCase();
  const betas = [];
  if (tag === "1m") betas.push("context-1m-2025-08-07");
  return { id, betas };
}

function pickUpstream(model) {
  if (model && model.toLowerCase().startsWith(GLM_MODEL_PREFIX)) {
    return { base: GLM_UPSTREAM, target: "GLM", isGlm: true };
  }
  return { base: ANTHROPIC_UPSTREAM, target: "Anthropic", isGlm: false };
}

const server = http.createServer((req, res) => {
  const chunks = [];
  req.on("data", (c) => chunks.push(c));
  req.on("end", () => {
    let body = Buffer.concat(chunks);

    // Parse the body so we can read (and, for GLM, rewrite) the model.
    let bodyObj = null;
    let model = "";
    if (body.length) {
      try {
        bodyObj = JSON.parse(body.toString("utf8"));
        model = bodyObj.model || "";
      } catch (_) {
        /* not JSON (e.g. health check) — fall through to default route */
      }
    }

    const route = pickUpstream(model);
    if (route.isGlm && !GLM_API_KEY) {
      res.writeHead(500, { "content-type": "application/json" });
      res.end(JSON.stringify({ error: "GLM_API_KEY not set in proxy environment" }));
      log(`REJECT ${model} -> GLM (no GLM_API_KEY)`);
      return;
    }

    const upstream = new URL(route.base);
    const path = upstream.pathname.replace(/\/$/, "") + req.url;

    // Clone headers, then fix host + auth for the chosen upstream.
    const headers = { ...req.headers };
    headers.host = upstream.host;
    delete headers["content-length"];

    if (route.isGlm) {
      // GLM authenticates with a bearer key; drop Anthropic-style auth.
      delete headers["x-api-key"];
      headers["authorization"] = `Bearer ${GLM_API_KEY}`;

      // Pin to the configured GLM model and strip the "[1m]" tag off the wire
      // id, translating it into the context-1m beta header.
      if (bodyObj) {
        const { id, betas } = splitModelTag(GLM_MODEL);
        bodyObj.model = id;
        if (betas.length) {
          const existing = (headers["anthropic-beta"] || "")
            .split(",").map((s) => s.trim()).filter(Boolean);
          headers["anthropic-beta"] = [...new Set([...existing, ...betas])].join(",");
        }
        body = Buffer.from(JSON.stringify(bodyObj), "utf8");
        model = `${id} (pinned from GLM_MODEL=${GLM_MODEL})`;
      }
    }
    // For Anthropic we leave authorization / x-api-key and the body exactly as
    // Claude Code sent them, so OAuth subscription tokens and API keys both
    // pass through, and Claude Code's own "[1m]" handling is untouched.

    headers["content-length"] = Buffer.byteLength(body);

    const transport = upstream.protocol === "http:" ? http : https;
    const proxyReq = transport.request(
      {
        protocol: upstream.protocol,
        hostname: upstream.hostname,
        port: upstream.port || (upstream.protocol === "http:" ? 80 : 443),
        method: req.method,
        path,
        headers,
      },
      (proxyRes) => {
        res.writeHead(proxyRes.statusCode || 502, proxyRes.headers);
        recordLimits(route.target, proxyRes.headers); // live lockout for the dashboard
        // Tee: stream straight through (SSE-safe) AND sniff the first ~16KB for
        // the message_start usage so we can record TRUE context usage.
        let scan = "";
        let recorded = false;
        const sessionKey =
          (bodyObj && bodyObj.metadata && bodyObj.metadata.user_id
            ? String(bodyObj.metadata.user_id).slice(-12)
            : route.target.toLowerCase()) + ":" + (bodyObj ? (bodyObj.model || "?") : "?");
        const window = windowFor(headers["anthropic-beta"], bodyObj && bodyObj.model);
        proxyRes.on("data", (chunk) => {
          res.write(chunk); // forward immediately — never block the stream
          if (!recorded && scan.length < 16384) {
            scan += chunk.toString("utf8");
            const full = parseStartUsage(scan);
            if (full != null) {
              recorded = true;
              recordUsage(sessionKey, model || (bodyObj && bodyObj.model) || "?", route.target, full, window);
            }
          }
        });
        proxyRes.on("end", () => res.end());
        proxyRes.on("error", () => res.end());
      }
    );

    proxyReq.on("error", (err) => {
      log(`ERROR ${route.target} ${err.message}`);
      if (!res.headersSent) {
        res.writeHead(502, { "content-type": "application/json" });
      }
      res.end(JSON.stringify({ error: `proxy upstream error: ${err.message}` }));
    });

    if (model) log(`${req.method} ${req.url} model=${model} -> ${route.target}`);
    proxyReq.end(body);
  });
});

server.on("error", (err) => {
  if (err.code === "EADDRINUSE") {
    console.error(
      `\n[proxy] Port ${PORT} is already in use - another process is listening there.\n` +
        `        Start the proxy on a different port, e.g.:  PORT=8789 node proxy.js\n` +
        `        (the launcher uses -Port; pass a free port if 8788 is taken).\n`
    );
    process.exit(1);
  }
  console.error(`[proxy] server error: ${err.message}`);
  process.exit(1);
});

server.listen(PORT, "127.0.0.1", () => {
  log(`listening on http://127.0.0.1:${PORT}`);
  log(`  Anthropic upstream: ${ANTHROPIC_UPSTREAM}`);
  log(`  GLM upstream:       ${GLM_UPSTREAM}  (key ${GLM_API_KEY ? "set" : "MISSING"})`);
  log(`  GLM route prefix:   "${GLM_MODEL_PREFIX}"  -> pinned model: ${GLM_MODEL}`);
});
