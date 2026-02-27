# Changelog

## Unreleased

### Added

- **Multi-name `let`** with `|` separator: `440 110 200 let | d m c |` binds three stack values at once. Single-name `let x` form unchanged.
- **Comparison ops**: `gt`, `lt`, `gte`, `lte`, `eq`, `neq` -- binary ops outputting 1.0 or 0.0, enabling signal-rate conditional logic. Constant-foldable.
- **SVF notch filter**: `svfnotch` (signal, freq, Q) -- band-reject filter using the existing SVF topology (`lp + hp`).
- **Fractional delay**: `delayf` (signal, delay_samples) -- linear-interpolating delay for chorus, flanger, Karplus-Strong, etc. Integer `delay` unchanged.
- **Cubic Hermite delay**: `delayc` (signal, delay_samples) -- 4-point cubic Hermite interpolating delay for higher-quality pitch-shifting, chorus, and Karplus-Strong. Smoother than `delayf` at the cost of extra computation.
- **Envelopes**: `line` (trig, time_s) -- triggered linear ramp 0->1; `ar` (trig, attack_s, release_s) -- triggered attack/release envelope. Both compose with `*` for amplitude shaping.
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
- `examples/prelude.dsp`: example definitions file with `osc2`, `double`, `softclip`. Updated to use multi-name `let` syntax.
- Function and file loading examples and token reference in both help patches.
- **Help patch examples** for all new features: multi-name `let`, comparisons (half-wave rectifier), `line`, `ar`, `delayf` (flanger), `svfnotch` -- added to both `dsp_graph~.maxhelp` and `joy~.maxhelp` with updated token reference lines.

- **Constant folding for comparisons**: `gt`, `lt`, `gte`, `lte`, `eq`, `neq` with constant inputs now fold at compile time (e.g. `5 3 gt` -> `1`).
- **Prelude additions** (22 -> 26 definitions):
  - `select` -- conditional selection alias (`a b cond select`, where cond is 0/1 from a comparison op).
  - `gate` -- signal gate using `gt` (`signal threshold gate` -> signal when above threshold, else 0).
  - `burst` -- AR-shaped noise transient (2ms attack, 50ms release).
  - `flange` -- basic flanger using sinusoidal delay modulation via `delayf`.

### Fixed

- **Pink noise instance isolation**: `pull_pink` used a `static long counter` shared across all instances. Counter moved to per-node state; RNG seed now varies per node index so multiple `pink` nodes produce independent output.
- **Noise/dust instance isolation**: `noise` and `dust` nodes shared hardcoded RNG seeds across all instances. Seeds now vary per node index (same golden-ratio scramble as `pink`).
- **Function definition overflow**: `dsp_func_define` now returns an error if name or body exceeds max length, instead of silently truncating via `strncpy`.
- **Stale `joy_tilde.c` comment**: `joy_tilde_dsp_msg` comment block updated to reflect the full current token set.

### Tests

- 28 new unit tests (82 -> 110 total): multi-name let (6), comparisons (1), svfnotch (1), delayf (2), line/ar envelopes (2), pink independence (1), noise independence (1), dust independence (1), hp1 (1), svflp/svfhp/svfbp attenuation (3), delay multi-sample (1), sah rising-edge (1), latch zero-hold (1), func body/name overflow (2), comparison folding (1), delay boundary (3: one-sample, near-max clamping for both `delay` and `delayf`).

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
