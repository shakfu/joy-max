# dsp_graph~ vs sapf~ vs joy~ -- Comparison

Three externals across two projects:

| | `sapf~` (sapf-max) | `dsp_graph~` (joy-max) | `joy~` (joy-max) |
|---|---|---|---|
| **Engine** | SAPF interpreter (C++, ~500K) | Compiled C node graph (~1.2K lines) | Joy interpreter + DSP graph |
| **Audio thread** | Interpreter pulls generators | Zero-interpretation, native pull | DSP graph or looped buffer |
| **Multi-channel** | `[300 301] 0 saw` auto-expands | Single output | Single output |
| **Feedback** | `combn`, `apverb`, recursive chains | None | None |
| **Variables** | `= x`, closures, forms | None | Joy stack |
| **Prelude** | 32KB standard library | None | Joy stdlib |
| **State** | Persistent across messages | Each message replaces graph | Joy stack persists |
| **Dependencies** | SAPF engine, Accelerate, CoreAudio | None (pure C, libm) | Joy interpreter |

## Where dsp_graph~ has a defensible niche

- Zero-dependency, zero-interpretation audio path (matters for hard real-time guarantees)
- Embeddable anywhere -- 2 files, pure C, no C++ runtime, no frameworks
- Compile cost is trivial (microseconds vs SAPF's interpreter setup)

## Where sapf~ wins

Everything else. It's a real language -- variables, lambdas, auto-mapping, lazy infinite sequences, form-based subgraph parameterization. The expression `[300 301] 0 saw .3 *` producing stereo with 1Hz beating is something `dsp_graph` fundamentally cannot express.

## The gap that matters most

1. **Feedback** -- SAPF has comb filters, allpass reverbs, recursive structures. `dsp_graph` has none. This blocks Karplus-Strong, physical modeling, feedback delay networks.
2. **Multi-channel** -- SAPF's auto-expansion from lists is elegant. `dsp_graph` is mono-output only.
3. **Parameterization** -- SAPF's forms let you build reusable, overridable synth patches. `dsp_graph` has no abstraction mechanism.

## Strategic question

Extending `dsp_graph` toward feedback, multi-channel, variables, and subgraphs would be converging on a worse version of SAPF. The architecture differs (compiled vs interpreted), but SAPF already has the language design, the UGen library, and the user-facing ergonomics figured out.

The real value of `dsp_graph` might be as an **optimization target inside sapf~** -- JIT-compiling hot SAPF expressions to native pull graphs -- rather than a user-facing external.
