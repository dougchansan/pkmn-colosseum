// configure_scratch.js — on an open decomp.me scratch tab: set compiler flags
// (Options "No arguments"), set Source code, Compile, read match %.
//
// usage:
//   node configure_scratch.js <scratchIdOrUrlFragment> <source.c> "<flags>"
// e.g.
//   node configure_scratch.js rPQRU build_pc/decompme/fn_800DCAB0/source.c "-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8"
const { chromium } = require('playwright-core');
const fs = require('fs');

(async () => {
  const frag = process.argv[2];
  const src = fs.readFileSync(process.argv[3], 'utf8');
  const flags = process.argv[4] || '';
  const b = await chromium.connectOverCDP('http://127.0.0.1:9222');
  let page = null;
  for (const c of b.contexts()) for (const p of c.pages())
    if (p.url().includes('/scratch/' + frag)) page = p;
  if (!page) {
    // tab not open (e.g. after a Chrome relaunch) -> open the scratch URL
    page = await b.contexts()[0].newPage();
    await page.goto('https://decomp.me/scratch/' + frag, { waitUntil: 'domcontentloaded' });
    await page.waitForTimeout(3500);
  }
  await page.bringToFront();
  const mod = process.platform === 'darwin' ? 'Meta' : 'Control';

  // 1) Options tab -> flags
  if (flags) {
    await page.getByText('Options', { exact: true }).first().click().catch(() => {});
    await page.waitForTimeout(800);
    const fi = page.locator('input[placeholder="No arguments"]');
    await fi.click();
    await page.keyboard.press(`${mod}+A`);
    await page.keyboard.press('Delete');
    await fi.type(flags, { delay: 0 });
    await page.waitForTimeout(400);
  }

  // 2) Source code tab -> set source (first cm editor)
  await page.getByText('Source code', { exact: true }).first().click().catch(() => {});
  await page.waitForTimeout(600);
  const ed = page.locator('.cm-content').first();
  await ed.click();
  await page.keyboard.press(`${mod}+A`);
  await page.keyboard.press('Delete');
  await page.keyboard.insertText(src);
  await page.waitForTimeout(400);

  // 3) Compile
  await page.getByRole('button', { name: /^Compile$/ }).click()
    .catch(async () => { await page.keyboard.press(`${mod}+Enter`); });
  await page.waitForTimeout(4500);

  const body = await page.evaluate(() => document.body.innerText);
  const m = body.match(/Compilation[\s\S]{0,30}?(\d+)\s*\(([\d.]+)%\)/);
  console.log(m ? `RESULT score=${m[1]} pct=${m[2]}%` : 'pct not found (maybe still compiling / error)');

  // persist: click Save so the pushed source isn't lost when the tab closes
  const save = page.getByRole('button', { name: /^Save$/ });
  if (await save.count()) {
    const dis = await save.first().isDisabled().catch(() => false);
    if (!dis) { await save.first().click().catch(() => {}); await page.waitForTimeout(1500); console.log('saved'); }
    else console.log('already saved');
  } else console.log('no Save button');
  await b.close();
})().catch(e => { console.error('ERR', e.message); process.exit(2); });
