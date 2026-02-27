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

```
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
| Unary | `neg` `abs` `sin` `cos` `tanh` `exp` `log` `sqrt` `wrap` `fold` `floor` `ceil` `round` `sign` `db2a` `a2db` `mtof` `ftom` |
| Generators | `sinosc` `phasor` `noise` `pink` `dust` `tri` `saw` `pulse` |
| Envelope | `decay` |
| Filters | `onepole` `hp1` `lag` `slew` `svflp` `svfhp` `svfbp` `biquad` |
| Utilities | `delay` `sah` `latch` |
| Stack | `dup` `swap` `pop`/`drop` `over` `rot` |
| Feedback | `fbread1`-`fbread4` `fbwrite1`-`fbwrite4` |
| Bindings | `<expr> let <name>` |
| Inputs | `in1`..`in8` |

### Functions

Define reusable operations that inline at compile time:

```
def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *
440 osc2 0.3 *
```

- `def <name> <body>` -- define a function
- `undef <name>` -- remove a function
- `cleardef` -- remove all functions

### File Loading

Load batches of function definitions from `.dsp` files:

```
load prelude.dsp
```

Or as a constructor argument:

```
[dsp_graph~ 1 2 prelude.dsp]
[joy~ 1 2 prelude.dsp]
```

`.dsp` file format:
```
# comment lines start with #
def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *
def double dup +
def softclip tanh
```

Files are resolved through the Max search path.

### Multi-output

N values left on the stack produce N output channels (max 8):

```
440 sinosc 0.3 * 443 sinosc 0.3 *    -- stereo detuned pair
```

### Constant Folding

Pure math subexpressions are folded to constants at compile time:

```
69 mtof sinosc    -- 69 mtof folds to 440 at compile time
```

## Build

Requires the [Min-DevKit](https://github.com/Cycling74/min-devkit) (included as submodule).

```sh
make setup    # init submodules + symlink to Max Packages
make build    # cmake build
make test     # run unit tests
```

## License

See [LICENSE](LICENSE).
