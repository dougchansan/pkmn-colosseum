// resave_all.js : ONE CDP connection; for every ledger scratch that has a staged
// build_pc/decompme/<fn>/source.c, open it, push the source, compile, and Save.
// Guarantees all scratches are persisted with their correct source after a Chrome restart.
const { chromium } = require('playwright-core');
const fs = require('fs');
const path = require('path');

const FLAGS = '-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8';
const ROOT = path.resolve(__dirname, '../..');

(async () => {
  const ledger = JSON.parse(fs.readFileSync(path.join(ROOT, 'build_pc/decompme/scratches.json'), 'utf8'));
  const b = await chromium.connectOverCDP('http://127.0.0.1:9222');
  const ctx = b.contexts()[0];
  const mod = process.platform === 'darwin' ? 'Meta' : 'Control';

  for (const [fn, e] of Object.entries(ledger)) {
    const id = (e.url || '').split('/scratch/')[1];
    const srcPath = path.join(ROOT, 'build_pc/decompme', fn, 'source.c');
    if (!id || !fs.existsSync(srcPath)) { console.log(`${fn}: skip (no url/source)`); continue; }
    const src = fs.readFileSync(srcPath, 'utf8');
    const page = await ctx.newPage();
    try {
      await page.goto('https://decomp.me/scratch/' + id, { waitUntil: 'domcontentloaded' });
      await page.waitForTimeout(3500);
      // flags
      await page.getByText('Options', { exact: true }).first().click().catch(() => {});
      await page.waitForTimeout(600);
      const fi = page.locator('input[placeholder="No arguments"]');
      if (await fi.count()) { await fi.click(); await page.keyboard.press(`${mod}+A`); await page.keyboard.press('Delete'); await fi.type(FLAGS, { delay: 0 }); }
      // source
      await page.getByText('Source code', { exact: true }).first().click().catch(() => {});
      await page.waitForTimeout(500);
      const ed = page.locator('.cm-content').first();
      await ed.click(); await page.keyboard.press(`${mod}+A`); await page.keyboard.press('Delete');
      await page.keyboard.insertText(src);
      await page.waitForTimeout(400);
      // compile
      await page.getByRole('button', { name: /^Compile$/ }).click().catch(() => {});
      await page.waitForTimeout(3500);
      // save
      let saved = 'no-save';
      if (await page.getByRole('button', { name: /^Saved$/ }).count()) saved = 'already-saved';
      else {
        const sv = page.getByRole('button', { name: /^Save$/ });
        if (await sv.count() && !(await sv.first().isDisabled().catch(() => false))) { await sv.first().click().catch(() => {}); await page.waitForTimeout(1500); saved = 'saved'; }
      }
      console.log(`${fn} (${id}): ${saved}`);
    } catch (err) { console.log(`${fn} (${id}): ERR ${err.message}`); }
  }
  await b.close();
})().catch(e => { console.error('FATAL', e.message); process.exit(2); });
