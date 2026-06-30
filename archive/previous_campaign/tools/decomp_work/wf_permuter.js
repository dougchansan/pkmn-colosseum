export const meta = {
  name: 'decomp-permuter-sweep',
  description: 'Parallel decomp-permuter (annealing) sweep over near-miss functions, one agent per file',
  phases: [{ title: 'Anneal', detail: 'per file: find near-misses, run the permuter on each, surface score-0 wins' }],
}

// args = ["src/game/....c", ...]  (high-value near-miss files) — tolerate array OR JSON-string
let FILES = []
if (Array.isArray(args)) FILES = args
else if (typeof args === 'string') { try { const p = JSON.parse(args); if (Array.isArray(p)) FILES = p } catch (e) {} }
log(`permuter sweep over ${FILES.length} files`)

const RESULT_SCHEMA = {
  type: 'object',
  properties: {
    file: { type: 'string' },
    wins: { type: 'array', items: { type: 'string' }, description: 'fn names the permuter cracked to score-0 (saved to wins/<fn>.c)' },
    nowins: { type: 'array', items: { type: 'string' }, description: 'fn:score for attempted-but-not-cracked' },
    failed: { type: 'array', items: { type: 'string' }, description: 'fn:reason for dir-build/compile failures' },
    notes: { type: 'string' },
  },
  required: ['file', 'wins', 'nowins'],
}

const results = await pipeline(
  FILES,
  (file) =>
    agent(
      `You run decomp-permuter (an automated annealing source-search) on the near-miss functions of ONE file, to find byte-exact CodeWarrior matches. This is SETUP+RUN; the parent re-verifies every win.\n\n` +
        `File: ${file}\n` +
        `Repo: C:\\Users\\douglaswhittingham\\pkmn-colosseum  (cwd). The permuter harness lives at tools/decomp_work/permuter/ and runs under WSL.\n\n` +
        `STEP 1 — find the BEST targets: run\n` +
        `  python tools/compile_check.py --diff ${file}\n` +
        `The output has columns: function, match%, instr (m/n), Mismatches. The permuter's sweet spot is a SMALL residual on a REAL C body. So: keep functions with Mismatches between 1 and 6 inclusive (skip 0 = already 100%, skip large mismatch counts = wrong-shape/walls). SKIP functions that are empty stubs (body is just /* TODO */) or inline-__asm-only (build_dir handles these but they have nothing to permute). Sort ASCENDING by Mismatches (fewest = most crackable first). Take the top 3.\n\n` +
        `STEP 2 — anneal each (up to 3 funcs, sequentially). build_dir.sh was just fixed to always parse. For each fn, run via the PowerShell tool (the Bash tool's WSL invocation STALLS under load — use PowerShell):\n` +
        `  wsl.exe bash -c "cd /mnt/c/Users/douglaswhittingham/pkmn-colosseum/tools/decomp_work/permuter && bash anneal_one.sh <FN> ${file} 540 2"\n` +
        `Each builds the dir and runs a 540s search at -j2 (KEEP -j2 — many agents run at once; do NOT raise -j). It prints 'WIN <fn>' (score-0, saved to wins/<fn>.c), 'NOWIN <fn> <score>', or 'FAIL <fn> <reason>'. Run one at a time. Verify the job actually started (wsl.exe bash -c "ps aux | grep permuter"); if WSL refuses to run after 2 attempts, record FAIL and move on.\n\n` +
        `STEP 3 — report. You MUST end by returning the structured result (wins/nowins/failed), even if partial or all-empty — never finish without it. Do NOT claim a match is real; the parent applies wins/<fn>.c and re-verifies. Just report what anneal_one.sh printed. If this file has no 1-6 mismatch real-C functions, return empty quickly.\n\n` +
        `HARD RULES: only create files under tools/decomp_work/permuter/. NEVER edit src/**/*.c, *_fn_*.inc, symbols/splits/config, or build scripts. NEVER run git. If compile_check finds NO 80-99.99% functions in this file, return empty wins/nowins quickly. If a fn has no *_fn_*.inc truth file, anneal_one will FAIL it — that's fine, just record it.`,
      { label: `anneal:${file.split('/').pop()}`, phase: 'Anneal', schema: RESULT_SCHEMA }
    )
)

const out = results.filter(Boolean)
const allWins = out.flatMap((r) => (r.wins || []).map((fn) => ({ fn, file: r.file })))
log(`permuter sweep done: ${allWins.length} score-0 candidates across ${out.length} files (PARENT MUST RE-VERIFY EACH)`)
return { wins: allWins, perFile: out }
