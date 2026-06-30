// Canonical (tracked) copy of the lever-crack-batch workflow.
// .claude/workflows/ is gitignored, so this tracked copy is the source of truth that
// travels with the repo. To make it invocable by name on a new machine (e.g. the Mac):
//     mkdir -p .claude/workflows && cp tools/decomp_work/workflows/lever-crack-batch.js .claude/workflows/
// Then: Workflow({ name: 'lever-crack-batch', args: { n: 12 } })
// Or run this copy directly without installing: Workflow({ scriptPath: 'tools/decomp_work/workflows/lever-crack-batch.js', args: { n: 12 } })
export const meta = {
  name: 'lever-crack-batch',
  description: 'Self-fueling batch: cheap Sonnet agents crack REG-COLORING near-miss decomp fns with proven levers (named locals + decl-order); Haiku independently verifies every claimed 100%. The mechanical subset the permuter is overkill for.',
  whenToUse: 'On-demand sweep of near-miss functions. Cheaper than always-on Opus/Sonnet lanes for the mechanical REG-COLORING work; pairs with the permuter (RELOC/SHAPE search-walls) and Opus (reshape).',
  phases: [
    { title: 'Scout', detail: 'pick N closest near-misses not locked by the fleet' },
    { title: 'Crack', detail: 'one Sonnet agent per fn: classify_residual + named-local/decl-order levers in a private band', model: 'sonnet' },
    { title: 'Verify', detail: 'independent Haiku re-measure + anti-fraud check of each claimed 100%', model: 'haiku' },
  ],
}

// args: { n?: number } — batch size (default 12). Each batch is one well-scoped fan-out.
const N = (args && Number(args.n)) || (Number.isFinite(args) ? Number(args) : 12)

phase('Scout')
const scout = await agent(
`Run exactly this and return its output verbatim:
  python tools/decomp_work/crack_batch_worklist.py --n ${N}
It prints a JSON array of [fn, src] pairs — the closest real-C near-miss functions that are NOT currently locked by the running fleet. Return that array as {"work": [[fn,src],...]}. If it prints [] return {"work": []}.`,
  { label: 'scout', phase: 'Scout',
    schema: { type: 'object', properties: { work: { type: 'array', items: { type: 'array', items: { type: 'string' } } } }, required: ['work'] } }
)
const WORK = (scout && Array.isArray(scout.work)) ? scout.work.filter(p => Array.isArray(p) && p.length === 2) : []
if (!WORK.length) { log('scout found no unlocked near-miss candidates'); return { error: 'no candidates', counts: { total: 0 } } }
log(`lever-crack-batch over ${WORK.length} near-miss fn(s)`)

const CRACK = {
  type: 'object',
  properties: {
    fn: { type: 'string' }, tag: { type: 'string' },
    status: { type: 'string', enum: ['SAVED', 'BANKED', 'WALL', 'ERROR'] },
    pct: { type: 'number' }, verdict: { type: 'string' }, note: { type: 'string' },
  },
  required: ['fn', 'tag', 'status', 'pct'],
}
const VERIFY = {
  type: 'object',
  properties: { fn: { type: 'string' }, confirmed: { type: 'boolean' }, pct: { type: 'number' }, note: { type: 'string' } },
  required: ['fn', 'confirmed', 'pct'],
}

const out = await pipeline(
  WORK,
  ([fn, src]) => {
    const stem = src.replace(/^.*\//, '').replace(/\.c$/, '')
    const tag = `wf_${fn}`
    return agent(
`MECHANICAL CRACK of ONE decomp near-miss in the pkmn-colosseum CodeWarrior matching decomp. Work ENTIRELY in a PRIVATE band (tag ${tag}) so you never collide with other agents or the running fleet. Run all commands from the repo root.

Target: ${fn} in ${src} — real C, currently <100%. Goal: byte-exact 100%, OR bank it for the permuter.

1. python tools/decomp_work/band.py init ${tag} ${src}
2. python tools/decomp_work/band.py check ${tag} ${fn}              # baseline match %
3. python tools/decomp_work/classify_residual.py ${tag} ${fn}       # the verdict decides the approach
4. If the verdict is REG-COLORING (the MECHANICAL case): edit tools/decomp_work/scratch/band_${tag}.c —
   (a) give ${fn}'s locals descriptive NAMED C locals; NEVER raw rNN register-locals (they pin the wrong coloring);
   (b) permute the DECLARATION ORDER of co-surviving locals (try declaring a later-surviving var first), re-measuring after each change with: python tools/decomp_work/band.py check ${tag} ${fn}. Up to 5 orderings. Change ONLY declarations / names — never logic or types.
5. If the verdict is RELOC / SCHEDULING / SHAPE / conv-literal: it is NOT mechanically crackable — do NOT grind it.
6. Final action (exactly ONE):
   - reached 100.00%  -> python tools/decomp_work/band.py save ${tag} ${fn}   (status SAVED)
   - best in [90,100) -> python tools/decomp_work/band.py bank ${tag} ${fn}   (status BANKED -> permuter)
   - below 90 OR not REG-COLORING -> if best >=90 bank it, else just stop (status WALL)
HARD RULES: real C ONLY — never write asm{}, __asm, an asm-storage-class function, or #include a .inc; never fake a match. ~10 min max; do ONLY ${fn}.
Report: fn=${fn}, tag=${tag}, status, the best pct you actually measured, and the classifier verdict.`,
      { label: `crack:${fn}`, phase: 'Crack', model: 'sonnet', schema: CRACK }
    )
  },
  (cr, [fn, src]) => {
    if (!cr) return null
    if (cr.status !== 'SAVED') return { ...cr, confirmed: null }
    const tag = cr.tag || `wf_${fn}`
    return agent(
`Independently VERIFY a claimed 100% decomp win — do NOT trust the prior agent (agents have faked matches before). pkmn-colosseum repo; run from the repo root.
1. python tools/decomp_work/band.py check ${tag} ${fn}   -> must report 100.00%
2. Open tools/decomp_work/scratch/band_${tag}.c, find ${fn}, and confirm its body is REAL C: NOT an asm{}/__asm block, NOT an asm-storage-class function, and NOT a #include of a .inc file.
Set confirmed=true ONLY if BOTH hold (100.00% AND real C). Report fn=${fn}, confirmed, pct (the re-measured value), and a short note.`,
      { label: `verify:${fn}`, phase: 'Verify', model: 'haiku', schema: VERIFY }
    ).then(v => ({ ...cr, confirmed: v ? v.confirmed === true : false, verify_pct: v ? v.pct : null }))
  }
)

const r = out.filter(Boolean)
const saved = r.filter(x => x.status === 'SAVED')
const confirmed = saved.filter(x => x.confirmed === true)
const fakes = saved.filter(x => x.confirmed === false)
const banked = r.filter(x => x.status === 'BANKED')
const walls = r.filter(x => x.status === 'WALL')
log(`done: SAVED ${saved.length} (confirmed ${confirmed.length}, FAKE ${fakes.length}) | BANKED ${banked.length} | WALL ${walls.length}`)
return {
  confirmed_wins: confirmed.map(x => x.fn),
  fakes: fakes.map(x => ({ fn: x.fn, claimed_pct: x.pct })),
  banked: banked.map(x => ({ fn: x.fn, pct: x.pct })),
  walls: walls.map(x => ({ fn: x.fn, verdict: x.verdict, pct: x.pct })),
  counts: { total: r.length, confirmed: confirmed.length, fakes: fakes.length, banked: banked.length, walls: walls.length },
}
