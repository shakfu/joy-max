# TODO -- dsp_graph

## Done

### Bug fixes
- [x] **`pull_pink` static counter bug**: `static long counter` in `pull_pink` was shared across all instances globally. Fixed: counter moved into `node->state.pink`, RNG seed now varies per node index.

### Primitives
- [x] **Comparison ops**: `gt`, `lt`, `eq`, `neq`, `gte`, `lte` -- 2-input, output 1.0 or 0.0. Added to op table with macro-generated pull functions.
- [x] **SVF notch filter**: Added `svfnotch` -- outputs `lp + hp` from the existing `svf_tick`. Same 3-input signature as `svflp`/`svfhp`/`svfbp`.
- [x] **Fractional delay with interpolation**: Added `delayf` with linear interpolation. Same 2-input signature as `delay` (signal, delay_samples) but supports fractional sample values. Integer `delay` kept as-is.
- [x] **Attack-release envelope**: Added both `line` (trig, time_s -> linear ramp 0->1) and `ar` (trig, attack_s, release_s -> attack/release envelope). Both trigger on rising edge, compose with `*` for amplitude shaping.

### Test coverage
- [x] **`hp1` direct test**: DC convergence test -- verifies highpass blocks DC after settling.
- [x] **`svflp`/`svfhp`/`svfbp` signal tests**: Attenuation tests -- 10kHz through 100Hz LP, 100Hz through 10kHz HP, 100Hz through 10kHz BP all attenuate below 0.05.
- [x] **`delay` multi-sample test**: Impulse delayed by 10 samples -- verifies ring buffer read/write positions.
- [x] **`sah` rising-edge test**: Ramping signal with trigger at sample 5 -- verifies capture on edge and hold.
- [x] **`latch` zero-hold test**: Non-zero then zero input -- verifies last non-zero value is held.

### Robustness
- [x] **`DSP_FUNC_BODY_LEN` silent truncation**: `dsp_func_define` now returns an error if name >= 64 chars or body >= 1024 chars, instead of silently truncating.
- [x] **`in1`-`in8` vs `DSP_MAX_INPUTS` mismatch**: Not a bug. `DSP_MAX_INPUTS` (6) limits per-node input connections (biquad max). `DSP_INPUT` nodes have 0 inputs and store the inlet index in `state.inlet_idx`. The `in1`-`in8` range correctly aligns with `DSP_MAX_OUTPUTS` (8 channels).

---

## Open

### Bug fixes
- [x] **`noise` static seed**: Fixed -- RNG seed now varies per node index using the same golden-ratio scramble as `pink`. Added `process_noise_independent` test.
- [x] **`dust` static seed**: Fixed -- same treatment. Added `process_dust_independent` test.

### Constant folding
- [x] **Comparison ops not folded**: Added all six comparison cases to the constant folding switch. `5 3 gt` now folds to `DSP_CONST(1.0)`. Added `fold_comparisons` test.

### Documentation
- [x] **Help patches missing new tokens**: Updated token reference lines and added example messages for all new features (multi-name let, comparisons, line, ar, delayf, svfnotch) in both `dsp_graph~.maxhelp` and `joy~.maxhelp`. Infrastructure elements repositioned to accommodate new content.
- [x] **`joy_tilde.c` comment block stale**: Updated `joy_tilde_dsp_msg` comment to list the full current token set (arithmetic, comparisons, unary, generators, envelopes, filters, delay, utilities, stack, feedback, bindings, functions).

### Prelude
- [x] **New prelude definitions**: Added 4 new defs (22 -> 26 total):
  - `def select mix` -- conditional selection alias
  - `def gate let | t s | s s t gt *` -- signal gate using comparison ops
  - `def burst let | t | t 0.002 0.05 ar noise *` -- AR noise transient
  - `def flange let | r s | s s r sinosc 20 * 40 + delayf + 0.5 *` -- basic flanger using `delayf`
  - `load_text_prelude` test updated with all 4 new defs and value/compile checks.

### Test coverage
- [x] **Delay boundary tests**: `delay_one_sample` (impulse at d=1), `delay_near_max` (d=200000 clamped to buf_len-1, reads zeroed buffer), `delayf_near_max` (d=200000.5 clamped to buf_len-2). 107 -> 110 tests.
- [x] **Cubic interpolation delay `delayc`**: Added `delayc` with 4-point cubic Hermite interpolation. Separate token from `delayf` so users choose quality vs cost.
