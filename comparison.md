# dsp_graph~ vs sapf~ vs joy~ -- Comparison

Three externals across two projects:

| | `sapf~` (sapf-max) | `dsp_graph~` (joy-max) | `joy~` (joy-max) |
|---|---|---|---|
| **Engine** | SAPF interpreter (C++, ~500K) | Compiled C node graph (~2K lines) | Joy interpreter + DSP graph |
| **Audio thread** | Interpreter pulls generators | Zero-interpretation, native pull | DSP graph or looped buffer |
| **Multi-channel** | `[300 301] 0 saw` auto-expands | N values on stack -> N outlets (max 8) | Same as dsp_graph~ in DSP mode |
| **Feedback** | `combn`, `apverb`, recursive chains | `fbread`/`fbwrite` buses (1-block delay, 4 buses) | Same as dsp_graph~ |
| **Variables** | `= x`, closures, forms | `let x`, `let \| a b c \|` (compile-time bindings) | Joy stack + DSP let bindings |
| **Functions** | Forms, lambdas, full language | `def`/`undef`/`cleardef`, inline at compile time | Joy stdlib + DSP functions |
| **Prelude** | 32KB standard library | `prelude.dsp` (22 defs), `.dsp` file loading | Joy stdlib + .dsp loading |
| **State** | Persistent across messages | Each message replaces graph; functions persist | Joy stack persists |
| **Conditionals** | Full language semantics | `gt` `lt` `gte` `lte` `eq` `neq` (signal-rate) | Joy conditionals + DSP comparisons |
| **Envelopes** | Full ADSR, segments | `decay`, `line`, `ar` | Same as dsp_graph~ |
| **Filters** | Extensive UGen library | `onepole` `hp1` `lag` `slew` SVF(lp/hp/bp/notch) `biquad` | Same as dsp_graph~ |
| **Delay** | Full delay lines, interpolation | `delay` (integer) + `delayf` (linear interp, ~4s max) | Same as dsp_graph~ |
| **Optimization** | None (interpreted) | Constant folding at compile time | DSP path gets folding |
| **Dependencies** | SAPF engine, Accelerate, CoreAudio | None (pure C, libm) | Joy interpreter |

## Where dsp_graph~ has a defensible niche

- Zero-dependency, zero-interpretation audio path (matters for hard real-time guarantees)
- Embeddable anywhere -- 2 files, pure C, no C++ runtime, no frameworks
- Compile cost is trivial (microseconds vs SAPF's interpreter setup)
- Constant folding eliminates pure-math subexpressions at compile time
- Let bindings enable node sharing without re-evaluation (DAG, not tree)
- Function definitions provide reusable abstractions without runtime cost

## Where sapf~ wins

- Full language: lambdas, closures, forms, lazy infinite sequences
- Auto-expanding multi-channel from lists (`[300 301] 0 saw .3 *` -> stereo with 1Hz beating)
- Rich parameterization via forms (overridable synth patches)
- Larger UGen library and more filter/delay variants
- Recursive feedback structures (comb, allpass reverb, FDN)

## Remaining gaps

1. **Recursive feedback** -- `dsp_graph` has `fbread`/`fbwrite` buses (fixed 1-block delay, 4 buses), which covers basic feedback paths. SAPF has arbitrary recursive structures, comb filters, and allpass reverbs as built-in primitives. Karplus-Strong is now possible in dsp_graph (using `fbread`/`fbwrite` + `delayf`), but ergonomics are rougher.
2. **Multi-channel expansion** -- `dsp_graph` supports N outputs (stack -> outlets), but has no auto-expansion from a single expression. Each channel must be explicitly written. SAPF's list-based expansion is more elegant for polyphony and stereo.
3. **Rich envelopes/sequencing** -- `dsp_graph` has `decay`, `line`, `ar`. SAPF has full ADSR, breakpoint envelopes, and sequencing primitives.
4. **Dynamic behavior** -- `dsp_graph` is compile-once, run-forever. No per-sample conditional branching (though comparison ops + `mix` enable signal-rate switching). SAPF's interpreter allows more dynamic structures.

## Strategic question

The gap between the two has narrowed significantly. `dsp_graph` now covers most common synthesis tasks (oscillators, FM, subtractive, feedback delays, envelopes, signal-rate conditionals) with zero runtime overhead. The remaining advantages of SAPF are language-level features (lambdas, auto-expansion, recursive structures) that would require fundamentally different compilation strategies to replicate.

The real value of `dsp_graph` might still be as an **optimization target inside sapf~** -- JIT-compiling hot SAPF expressions to native pull graphs -- but it has also become a credible standalone tool for Max patching where low-overhead compiled DSP matters.
