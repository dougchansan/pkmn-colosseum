// readmatch.js <frag> : parse decomp.me diff table, report identical/total instr lines.
const { chromium } = require('playwright-core');
(async () => {
  const frag=process.argv[2];
  const b = await chromium.connectOverCDP('http://127.0.0.1:9222');
  let page=null;
  for (const c of b.contexts()) for (const p of c.pages())
    if (p.url().includes('/scratch/'+frag)) page=p;
  if(!page){console.log('no tab /scratch/'+frag);await b.close();return;}
  const txt=await page.evaluate(()=>document.body.innerText);
  // diff lines look like:  " 0:    lbz  r0,2(r3)" (target, leading space col)
  //                        "   0:    lbz  r0,2(r3)" (current, indented)
  // Collect all "<hex>:   insn" rows in order, pair target/current by alternation.
  const rows=txt.split('\n').map(l=>l.replace(/\s+$/,'')).filter(l=>/^\s*[0-9a-f]+:\s/.test(l));
  // normalize: strip leading offset+colon, collapse ws
  const norm=rows.map(l=>l.replace(/^\s*[0-9a-f]+:\s*/,'').replace(/\s+/g,' ').trim());
  // they come in T,C,T,C,... pairs
  let ident=0, tot=0;
  for(let i=0;i+1<norm.length;i+=2){tot++; if(norm[i]===norm[i+1]) ident++;}
  const noProblems=/No problems/.test(txt);
  console.log(`identical ${ident}/${tot} pairs; noProblems=${noProblems}; ${ident===tot&&tot>0?'MATCH-100%':'NOT-100%'}`);
  await b.close();
})().catch(e=>{console.error('ERR',e.message);process.exit(2);});
