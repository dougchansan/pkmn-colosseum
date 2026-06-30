// save_scratch.js [<idOrFragment>] : click "Save" on the matching scratch tab (persists
// the pushed source + claims ownership). With no arg, saves EVERY open /scratch/ tab.
const { chromium } = require('playwright-core');

async function saveOne(page) {
  await page.bringToFront();
  if (await page.getByRole('button', { name: /^Saved$/ }).count()) return 'already saved';
  const btn = page.getByRole('button', { name: /^Save$/ });
  if (!(await btn.count())) return 'no Save/Saved button (not owner? -> Fork instead)';
  const dis = await btn.first().isDisabled().catch(() => false);
  if (dis) return 'already saved (button disabled)';
  await btn.first().click().catch(() => {});
  await page.waitForTimeout(1500);
  return 'saved';
}

(async () => {
  const frag = process.argv[2];
  const b = await chromium.connectOverCDP('http://127.0.0.1:9222');
  const pages = [];
  for (const c of b.contexts()) for (const p of c.pages())
    if (p.url().includes('/scratch/')) pages.push(p);
  const targets = frag ? pages.filter(p => p.url().includes('/scratch/' + frag)) : pages;
  if (!targets.length) { console.log('no matching scratch tab'); await b.close(); return; }
  for (const p of targets) {
    const id = p.url().split('/scratch/')[1].split(/[/?#]/)[0];
    const r = await saveOne(p);
    console.log(id + ': ' + r);
  }
  await b.close();
})().catch(e => { console.error('ERR', e.message); process.exit(2); });
