// create_scratch.js — create a decomp.me GameCube/Wii scratch in the user's logged-in
// debug Chrome (CDP :9222), leave the tab open, print the resulting /scratch/<id> URL.
//
// usage:
//   node create_scratch.js --label <fn> --compiler "<dropdown label>" --asm target.s [--ctx context.c]
// e.g.
//   node create_scratch.js --label fn_800DCAB0 --compiler "2.4.2 build 53 (GC MW 1.3)" --asm build_pc/decompme/fn_800DCAB0/target.s
//
// Flags + Source are NOT set here (the /new page has no flags/source field) — set them
// afterward with configure_scratch.js (Options "No arguments" + Source + Compile).
const { chromium } = require('playwright-core');
const fs = require('fs');

function arg(name, def) {
  const i = process.argv.indexOf(name);
  return i >= 0 ? process.argv[i + 1] : def;
}

(async () => {
  const label = arg('--label');
  const compiler = arg('--compiler', '2.4.2 build 53 (GC MW 1.3)');
  const asmFile = arg('--asm');
  const ctxFile = arg('--ctx');
  if (!label || !asmFile) { console.error('need --label and --asm'); process.exit(2); }
  const asm = fs.readFileSync(asmFile, 'utf8');
  const ctx = ctxFile ? fs.readFileSync(ctxFile, 'utf8') : '';

  const b = await chromium.connectOverCDP('http://127.0.0.1:9222');
  const bctx = b.contexts()[0];
  const page = await bctx.newPage();
  await page.bringToFront();
  await page.goto('https://decomp.me/scratch/new', { waitUntil: 'domcontentloaded' });
  await page.waitForTimeout(3500);

  // 1) platform
  await page.locator('li', { hasText: 'GameCube / Wii' }).first().click();
  await page.waitForTimeout(1500);

  // 2) compiler (2nd <select>; first is the preset = Custom)
  const selects = await page.locator('select').all();
  await selects[1].selectOption({ label: compiler });
  await page.waitForTimeout(1200);

  // 3) diff label
  const lab = page.locator('input[name="label"]');
  await lab.click(); await lab.fill(label);

  // 4) target assembly = cm editor #0 ; context = cm #1
  const eds = page.locator('.cm-content');
  async function setEditor(idx, text) {
    const ed = eds.nth(idx);
    await ed.click();
    const mod = process.platform === 'darwin' ? 'Meta' : 'Control';
    await page.keyboard.press(`${mod}+A`);
    await page.keyboard.press('Delete');
    await page.keyboard.insertText(text);
    await page.waitForTimeout(300);
  }
  await setEditor(0, asm);
  if (ctx) await setEditor(1, ctx);
  await page.waitForTimeout(400);

  // 5) create
  await page.getByRole('button', { name: /^Create scratch$/ }).click();
  // wait for navigation to /scratch/<id>
  for (let i = 0; i < 40; i++) {
    await page.waitForTimeout(500);
    if (/\/scratch\/[A-Za-z0-9]+/.test(page.url())) break;
  }
  const url = page.url();
  console.log('URL ' + url);
  await b.close(); // detach without closing the tab

  // auto-log into the ledger (build_pc/decompme/scratches.json)
  try {
    const path = require('path');
    const ledger = path.resolve(__dirname, '../../build_pc/decompme/scratches.json');
    const d = fs.existsSync(ledger) ? JSON.parse(fs.readFileSync(ledger, 'utf8')) : {};
    const e = d[label] || {};
    e.url = url;
    if (!e.status) e.status = 'wip';
    if (!e.compiler) e.compiler = compiler.includes('1.2.5n') ? 'GC/1.2.5n' : 'GC/1.3';
    d[label] = e;
    fs.writeFileSync(ledger, JSON.stringify(d, null, 2) + '\n');
    console.log('logged ' + label + ' -> ' + ledger);
  } catch (e) { console.error('log-warn', e.message); }
})().catch(e => { console.error('ERR', e.message); process.exit(2); });
