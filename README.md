# joy-max

Max/MSP externals for DSP programming with the Joy language and a compiled stack-based DSP graph.

## Externals

### joy~

Joy language DSP external. Dual mode operation:

- **`perform <expr>`** -- tries compiled DSP graph first, falls back to Joy interpreter (buffer mode).
- **`dsp <expr>`** -- compiles an RPN expression to a native DSP graph (no fallback, reports errors).
- **`bang`** -- re-evaluates the last perform expression.

### dsp_graph~

Standalone compiled DSP graph external (no Joy dependency). Any message is compiled as an RPN DSP expression.

```text
[dsp_graph~ 1 2]         -- 1 signal input, 2 signal outputs
440 sinosc 0.3 *         -- sine oscillator at 440 Hz, scaled to 0.3
in1 2000 0.707 svflp     -- SVF lowpass filter on input signal
```

## DSP Expression Language

Stack-based RPN (reverse Polish notation). Values are pushed onto a stack; operators consume inputs and push results.

### Tokens

| Category | Tokens |
|----------|--------|
| Math | `+` `-` `*` `/` `pow` `min` `max` `mod` `clip` `mix`/`lerp` |
| Comparison | `gt` `lt` `gte` `lte` `eq` `neq` (output 1.0 or 0.0) |
| Unary | `neg` `abs` `sin` `cos` `tanh` `exp` `log` `sqrt` `wrap` `fold` `floor` `ceil` `round` `sign` `db2a` `a2db` `mtof` `ftom` |
| Generators | `sinosc` `phasor` `noise` `pink` `dust` `tri` `saw` `pulse` |
| Envelope | `decay` `line` `ar` |
| Filters | `onepole` `hp1` `lag` `slew` `svflp` `svfhp` `svfbp` `svfnotch` `biquad` |
| Delay | `delay` `delayf` (fractional, linear interpolation) |
| Utilities | `sah` `latch` |
| Stack | `dup` `swap` `pop`/`drop` `over` `rot` |
| Feedback | `fbread1`-`fbread4` `fbwrite1`-`fbwrite4` |
| Bindings | `let <name>` or `let &#124; n1 n2 ... &#124;` |
| Inputs | `in1`..`in8` |

### Let Bindings

Bind stack values to names for reuse:

```text
440 sinosc let s  s s 0.3 * -- single name
440 110 200 let | d m c |  m sinosc d * c + sinosc -- multi-name
```

Multi-name `let` binds left-to-right from top of stack: in `let | a b c |`, `a` gets the top value, `b` the next, `c` the third.

### Functions

Define reusable operations that inline at compile time:

```text
def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *
440 osc2 0.3 *
```

- `def <name> <body>` -- define a function
- `undef <name>` -- remove a function
- `cleardef` -- remove all functions

### File Loading

Load batches of function definitions from `.dsp` files:

```text
load prelude.dsp
```

Or as a constructor argument:

```text
[dsp_graph~ 1 2 prelude.dsp]
[joy~ 1 2 prelude.dsp]
```

`.dsp` file format:

```text
# comment lines start with #
def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *
def double dup +
def softclip tanh
```

Files are resolved through the Max search path.

### Multi-output

N values left on the stack produce N output channels (max 8):

```text
440 sinosc 0.3 * 443 sinosc 0.3 *    -- stereo detuned pair
```

### Comparison Ops

Signal-rate conditionals returning 1.0 or 0.0:

```text
in1 0.5 gt                -- gate: 1.0 when input > 0.5, else 0.0
in1 dup 0 lt 0 swap mix   -- half-wave rectifier (zero out negatives)
```

### Envelopes

```text
1 0.01 decay               -- triggered exponential decay (0.01s)
1 0.1 line                 -- triggered linear ramp 0->1 (0.1s)
1 0.01 0.1 ar              -- triggered attack/release (10ms atk, 100ms rel)
```

`line` ramps from 0 to 1 and stays. `ar` ramps up then back down to 0. All trigger on rising edge of the first input. Compose with `*` for amplitude shaping.

### Delay

```text
in1 4410 delay             -- integer sample delay (100ms at 44.1kHz)
in1 44.5 delayf            -- fractional delay with linear interpolation
```

Both support up to ~4 seconds of delay. `delayf` enables chorus, flanger, and Karplus-Strong effects.

### Constant Folding

Pure math subexpressions are folded to constants at compile time:

```text
69 mtof sinosc    -- 69 mtof folds to 440 at compile time
```

Comparison ops are also foldable: `3 5 lt` folds to `1` at compile time.

## Build

Requires the [Min-DevKit](https://github.com/Cycling74/min-devkit) (included as submodule).

```sh
make setup    # init submodules + symlink to Max Packages
make build    # cmake build
make test     # run unit tests
```

## License

See [LICENSE](LICENSE).
