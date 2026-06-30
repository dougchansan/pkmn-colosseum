export const meta = {
  name: 'decomp-wall-research',
  description: 'Deep research: fundamentally new ways to crack the CW reg-alloc/scheduling matching wall',
  phases: [
    { title: 'Probe', detail: 'parallel deep-research agents across forefront directions + literature' },
    { title: 'Synthesize', detail: 'rank + combine into concrete novel experiments' },
  ],
}

const PROBLEM = `
PROBLEM (be precise; this is a real forefront-research question, not a toy):
We are byte-exact decompiling Pokemon Colosseum (GameCube, PowerPC) compiled by Metrowerks CodeWarrior (mwcceppc, ~CW 1.3, deterministic). Goal: source C that compiles to the EXACT target bytes.
- ~4,594 / 8,287 functions already match 100% (we have this as a labeled corpus of (target asm -> exact-matching CW C) pairs).
- ~2,000+ functions are "near-misses": our C is SEMANTICALLY CORRECT and 90-99% byte-identical, but the residual differences are compiler-internal decisions we cannot steer from C:
   (a) REGISTER ALLOCATION: target uses r31/r30/r29 in an order our source can't reproduce; CW's allocator is deterministic graph-coloring/linear-scan driven by variable declaration order + live ranges, but declaration order ALSO drives instruction scheduling, and the two are coupled so we can't satisfy both.
   (b) INSTRUCTION SCHEDULING: order of independent loads/ops differs; CW's scheduler wants the opposite order from what our decl-order produces.
   (c) IDIOM/THRESHOLD: stmw emitted only at >=5 saved regs (target wants 2-3 stw); branchless-ternary peephole; bool-materialize (subic/subfe vs neg/or/srwi); commutative operand order landing values in a fresh vs reused register.
- We have EXHAUSTIVELY REFUTED these fixes: per-function C rewrites, all CW versions, all compiler flags (-O/-inline/-sym/-func_align), register pressure manipulation, explicit register pinning (register int x asm("r31") — CW ignores it), exact upstream source import.
- We have mwcceppc.exe locally (a deterministic black-box oracle: source in, asm out, fast). We have objdiff for byte-diffing. Ryzen 9 + 128GB RAM. We can run the compiler millions of times.
- A "lever catalog" of ~21 hand-found source transformations exists; manual application is the current method but yield is diminishing.

WHAT WE NEED: fundamentally NEW, creative, possibly cross-disciplinary approaches to crack the walls — automated search, machine learning, compiler inversion, constraint solving, stochastic/quantum-inspired optimization, learned/differentiable surrogates, information-theoretic residual exploitation. Forefront ideas. Real papers/tools where they exist.
`

const FINDING_SCHEMA = {
  type: 'object',
  properties: {
    direction: { type: 'string' },
    key_techniques: { type: 'array', items: { type: 'string' } },
    most_promising_idea: { type: 'string', description: 'the single most promising concrete approach for OUR problem' },
    why_it_could_work: { type: 'string' },
    applicability: { type: 'string', description: 'how directly it maps to CW/PPC byte-exact matching; honest about gaps' },
    concrete_experiment: { type: 'string', description: 'a specific first experiment we could run this week, with the mechanism' },
    estimated_impact: { type: 'string', enum: ['game-changer', 'high', 'medium', 'low'] },
    feasibility: { type: 'string', enum: ['this-week', 'weeks', 'months', 'research-project'] },
    references: { type: 'array', items: { type: 'string' }, description: 'papers, tools, repos with names/authors/urls' },
  },
  required: ['direction', 'most_promising_idea', 'why_it_could_work', 'applicability', 'concrete_experiment', 'estimated_impact', 'feasibility', 'references'],
}

const DIRECTIONS = [
  {
    key: 'permuter-community',
    prompt: `Direction: existing decompilation-community automated matching tooling.\n${PROBLEM}\nDeeply investigate (WEB SEARCH heavily): simonlindholm's "decomp-permuter" (randomized C source mutation with the compiler in the loop, used by N64/GC decomps), decomp.me, objdiff, m2c/mips_to_c, asm-differ. Does a PowerPC/CodeWarrior permuter backend exist or has anyone adapted permuter for mwcceppc? What mutation operators does permuter use (temp/perm randomization, reordering, type changes)? How do GameCube decomp projects (zeldaret/tww, melee/ssbm, prime, pikmin) crack reg-alloc/scheduling near-misses at scale? Find the actual tools and how to run them for our case.`,
  },
  {
    key: 'stochastic-superopt',
    prompt: `Direction: stochastic superoptimization & MCMC search over program space (the "new equations" angle).\n${PROBLEM}\nDeeply investigate (WEB SEARCH): STOKE (Schkufza, Sharma, Aiken — "Stochastic Superoptimization", ASPLOS 2013) which uses Metropolis-Hastings / MCMC over instruction sequences to match a target via a cost function; how to adapt the Metropolis acceptance criterion exp(-beta * cost) to search over SOURCE-level transformations (not instructions) with mwcceppc as the oracle. Define the cost function (byte-distance + register-permutation distance), proposal distribution (source mutations), and annealing schedule. This is the core "derivatives/quantum" framing: simulated annealing / MCMC on a discrete energy landscape. Give the actual math.`,
  },
  {
    key: 'quantum-annealing-qubo',
    prompt: `Direction: quantum-inspired / annealing / QUBO formulation (the user's "think like quantum mechanics" framing).\n${PROBLEM}\nDeeply investigate (WEB SEARCH): can the source-transformation search be encoded as a QUBO (Quadratic Unconstrained Binary Optimization) or Ising model and solved by simulated annealing / quantum annealing / quantum-inspired solvers (D-Wave, Fujitsu Digital Annealer, simulated bifurcation)? Each binary variable = a discrete source choice (decl order swap, cast on/off, pragma on/off); energy = byte-mismatch. Is register allocation itself an Ising/graph-coloring problem we can invert? Graph coloring IS a canonical QUBO. Could we solve "what interference graph / allocation order yields the target register assignment" as a constraint problem, then back out the source? Give the formulation honestly, including whether it's overkill vs classical search.`,
  },
  {
    key: 'ml-corpus-surrogate',
    prompt: `Direction: machine learning on OUR OWN 4,594-pair solved corpus + differentiable surrogate compilers (the "derivatives/gradient" angle).\n${PROBLEM}\nDeeply investigate (WEB SEARCH): neural decompilation (LLM4Decompile, Nova, SLaDe, Meta's work), learned compiler cost models (Google MLGO, CompilerGym, Ithemal for throughput), and the idea of training a DIFFERENTIABLE SURROGATE of mwcceppc (source-features -> predicted asm / predicted register assignment) so we can gradient-descend source toward the target. Most important: we have 4,594 (target-asm -> exact-CW-C) pairs SPECIFIC to this compiler. How to fine-tune a code model (LoRA on qwen2.5-coder, etc.) on this corpus to map target-asm -> CW-matching-C, exploiting that it learns CW's exact reg-alloc idioms? Design the training + the inference-with-compiler-verification loop.`,
  },
  {
    key: 'compiler-inversion-regalloc',
    prompt: `Direction: white-box inversion of CodeWarrior's register allocator & scheduler.\n${PROBLEM}\nDeeply investigate (WEB SEARCH + reasoning): what algorithm does Metrowerks CodeWarrior's PPC backend use for register allocation (Chaitin-Briggs graph coloring? linear scan? what spill/coalesce heuristics?) and instruction scheduling (list scheduling? what priority function?). Decomp community lore on "CW register allocation". If the allocator is a KNOWN deterministic algorithm, can we SOLVE for the source-level live-range structure that forces the target coloring (treat coloring order as the free variable)? Can we characterize it EMPIRICALLY by fuzzing tiny source variations and fitting a model of its decision function (a "compiler oracle" / active-learning approach)? Propose the characterization experiment.`,
  },
  {
    key: 'cegis-smt-synthesis',
    prompt: `Direction: program synthesis / CEGIS / SMT for exact compiler-output matching.\n${PROBLEM}\nDeeply investigate (WEB SEARCH): counterexample-guided inductive synthesis (CEGIS), Souper (SMT-based superoptimizer), sketch-based synthesis, and "compiler-in-the-loop" synthesis where the compiler is an opaque verifier. Frame matching as: synthesize a source AST whose compilation equals M_f, using mwcceppc as an oracle in a refinement loop. What is the right hypothesis space (a grammar of CW-equivalent source transformations) and search strategy (enumerative/constraint-guided)? How to use the residual diff as a counterexample to prune the space each iteration?`,
  },
  {
    key: 'residual-clustering-generalization',
    prompt: `Direction: information-theoretic exploitation of the residual diffs across all ~2,000 near-misses (analysis of OUR data, plus literature).\n${PROBLEM}\nReason deeply (and WEB SEARCH for "decompilation pattern mining"): if we cluster the 2,000 residual diff signatures, how many DISTINCT wall-archetypes likely exist (probably far fewer than 2,000)? If solving ONE instance of an archetype yields a transformation that GENERALIZES to all instances of that archetype, the effective problem size collapses. Propose: a systematic residual-signature taxonomy + a "solve-one-generalize-many" pipeline. Also: which archetypes are PROVABLY unsolvable from C (true walls) vs merely unsolved? Give the clustering method and how to turn a solved cluster into an automated rewrite rule.`,
  },
]

const findings = await parallel(
  DIRECTIONS.map((d) => () =>
    agent(d.prompt, { label: `research:${d.key}`, phase: 'Probe', schema: FINDING_SCHEMA, agentType: 'general-purpose' })
  )
)

const valid = findings.filter(Boolean)
log(`collected ${valid.length}/${DIRECTIONS.length} research findings`)

const synthesis = await agent(
  `You are the principal investigator synthesizing a forefront research plan to crack the CodeWarrior byte-exact matching wall.\n${PROBLEM}\n` +
    `Here are ${valid.length} deep-research findings from parallel probes:\n${JSON.stringify(valid, null, 1)}\n\n` +
    `Synthesize into a RANKED research plan. For the top 3-4 approaches: explain the core idea in plain terms, the precise mechanism (with any math/equations), WHY it can beat the wall where manual levers can't, the concrete first experiment (runnable this week given we have mwcceppc + 128GB + the 4594-pair corpus), the risk, and the expected payoff (how many of the ~2000 walls it could plausibly crack). Call out any approach that is a likely game-changer and any that is hype. Be honest about which walls are truly compiler-fundamental (no source can express them) vs merely unsolved. End with a single recommended FIRST move.`,
  { label: 'synthesize', phase: 'Synthesize' }
)

return { findings: valid, plan: synthesis }
