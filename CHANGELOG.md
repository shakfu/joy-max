# Changelog

## Unreleased

### Added
- **Compile-time functions** (`def`/`undef`/`cleardef`): reusable named operation patterns that inline at compile time with zero runtime cost.
  - `def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *` defines a function; `440 osc2` uses it.
  - Functions persist across expression changes on the same instance.
  - Nested function calls supported up to 8 levels deep (compile-time recursion guard).
  - `undef <name>` removes a single function; `cleardef` removes all.
  - Available in both `dsp_graph~` and `joy~` externals.
- **File loading for function definitions** (`load` message and constructor arg):
  - `load prelude.dsp` loads a batch of `def` statements from a `.dsp` file resolved via the Max search path.
  - Constructor arg: `[dsp_graph~ 1 2 prelude.dsp]` or `[joy~ 1 2 prelude.dsp]` loads definitions at creation time.
  - `.dsp` file format: one `def <name> <body>` per line; `#` comments and blank lines are ignored.
  - Shared parser (`dsp_func_load_text`) in libdsp, testable without the Max SDK.
  - Available in both `dsp_graph~` and `joy~` externals.
- `dsp_func_table` API in libdsp: `dsp_func_table_init`, `dsp_func_define`, `dsp_func_undef`, `dsp_func_clear`, `dsp_func_load_text`.
- `dsp_compile()` now accepts an optional `const dsp_func_table*` parameter (pass `NULL` for no functions).
- 13 new unit tests covering function basics, nesting, let-binding interaction, error cases, recursion depth, table helpers, and file loading (basic, comments, error lines, missing body, integration, CRLF, empty).
- `examples/prelude.dsp`: example definitions file with `osc2`, `double`, `softclip`.
- Function and file loading examples and token reference in both help patches.

## 0.2.0

### Added
- `dsp_graph~` external: standalone compiled DSP graph (no Joy dependency).
- Let bindings (`<expr> let <name>`): name a node and reuse it without re-evaluation.
- Feedback buses (`fbread1`-`fbread4`, `fbwrite1`-`fbwrite4`): one-block delay feedback paths.
- Constant folding: pure math expressions collapse to single constants at compile time.
- Generators: `sinosc`, `phasor`, `noise`, `pink`, `dust`, `tri`, `saw`, `pulse`.
- Envelope: `decay`.
- Filters: `onepole`, `hp1`, `lag`, `slew`, `svflp`, `svfhp`, `svfbp`, `biquad`.
- Utilities: `delay`, `sah`, `latch`.
- Stack ops: `dup`, `swap`, `pop`/`drop`, `over`, `rot`.
- Multi-output: N values on stack produce N outlet channels (max 8).
- Signal inputs: `in1`..`in8`.

## 0.1.0

### Added
- `joy~` external: Joy language DSP for Max/MSP.
- Dual mode: `perform` (Joy eval with DSP compile fast-path) and `dsp` (pure compiled graph).
- Double-buffered Joy output for lock-free audio thread reads.
