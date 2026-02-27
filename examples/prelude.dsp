# prelude.dsp -- common DSP function definitions
#
# Load via: [dsp_graph~ 1 2 prelude.dsp] or "load prelude.dsp"


# ------- math utilities -------

# double a value: x -> 2x
def double dup +

# halve a value: x -> x/2
def half 0.5 *

# square: x -> x*x
def square dup *

# reciprocal: x -> 1/x
def reciprocal 1 swap /

# clamp to [0, 1]
def clamp01 0 1 clip

# unipolar [0,1] to bipolar [-1,1]
def bipolar 2 * 1 -

# bipolar [-1,1] to unipolar [0,1]
def unipolar 1 + 0.5 *


# ------- conversions -------

# semitones to frequency ratio: 12 semi2ratio -> 2.0
def semi2ratio 12 / 2 swap pow

# BPM to Hz (for LFOs synced to tempo): 120 bpm2hz -> 2.0
def bpm2hz 60 /


# ------- waveshaping -------

# soft saturation via tanh: signal softclip
def softclip tanh

# hard clip to [-1, 1]: signal hardclip
def hardclip -1 1 clip

# rational saturation x/(1+|x|), smoother than tanh: signal saturate
def saturate dup abs 1 + /


# ------- oscillators -------

# detuned unison oscillator: freq osc2 -> mono sum of two detuned sines
def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *

# sub oscillator blend (fundamental + octave below): freq subosc
def subosc let | f | f sinosc f 0.5 * sinosc + 0.5 *

# FM oscillator: carrier mod_freq mod_depth fmosc
def fmosc let | d m c | m sinosc d * c + sinosc

# ring modulator: signal freq ring
def ring sinosc *


# ------- filters -------

# lowpass (SVF, Q=0.707): signal freq lp
def lp 0.707 svflp

# highpass (SVF, Q=0.707): signal freq hp
def hp 0.707 svfhp

# DC blocker (20 Hz highpass): signal dcblock
def dcblock 20 hp1


# ------- dynamics -------

# noise burst shaped by trigger envelope: trigger time noiseburst
def noiseburst decay noise *


# ------- stereo -------

# linear stereo pan: signal pos(0-1) pan -> L R
def pan let | p s | s 1 p - * s p *

# stereo spread from mono via detuning: freq spread -> L R
def spread let | f | f 1.003 * sinosc f 0.997 * sinosc
