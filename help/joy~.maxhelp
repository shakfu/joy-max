{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 1,
            "revision": 2,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 100.0, 100.0, 900.0, 1250.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-1",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 1490.0, 45.0, 45.0 ]
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "fontsize": 16.0,
                    "id": "obj-title",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 15.0, 15.0, 600.0, 24.0 ],
                    "text": "joy~ - Joy language DSP external for Max/MSP"
                }
            },
            {
                "box": {
                    "id": "obj-desc",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 15.0, 45.0, 850.0, 20.0 ],
                    "text": "'perform' tries DSP compile first, falls back to Joy eval. 'dsp' compiles RPN to a real-time DSP graph (no fallback)."
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-dsp",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 80.0, 400.0, 20.0 ],
                    "text": "Generators"
                }
            },
            {
                "box": {
                    "id": "obj-dsp1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 105.0, 180.0, 22.0 ],
                    "text": "perform 440 sinosc"
                }
            },
            {
                "box": {
                    "id": "obj-dsp2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 220.0, 105.0, 200.0, 22.0 ],
                    "text": "perform 440 sinosc 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-dsp3",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 430.0, 105.0, 180.0, 22.0 ],
                    "text": "perform 440 tri"
                }
            },
            {
                "box": {
                    "id": "obj-dsp4",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 135.0, 180.0, 22.0 ],
                    "text": "perform 440 saw 0.5 *"
                }
            },
            {
                "box": {
                    "id": "obj-dsp5",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 220.0, 135.0, 220.0, 22.0 ],
                    "text": "perform 440 0.5 pulse 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-dsp6",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 450.0, 135.0, 200.0, 22.0 ],
                    "text": "perform noise 0.2 *"
                }
            },
            {
                "box": {
                    "id": "obj-pink",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 660.0, 135.0, 160.0, 22.0 ],
                    "text": "perform pink 0.2 *"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-stack",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 170.0, 400.0, 20.0 ],
                    "text": "Stack ops & math"
                }
            },
            {
                "box": {
                    "id": "obj-dup",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 195.0, 280.0, 22.0 ],
                    "text": "perform 440 sinosc dup * 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-mtof",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 320.0, 195.0, 200.0, 22.0 ],
                    "text": "perform 69 mtof sinosc"
                }
            },
            {
                "box": {
                    "id": "obj-wrap",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 530.0, 195.0, 250.0, 22.0 ],
                    "text": "perform 200 phasor wrap 2 * 1 -"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-filter",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 230.0, 400.0, 20.0 ],
                    "text": "Filters"
                }
            },
            {
                "box": {
                    "id": "obj-onepole",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 255.0, 250.0, 22.0 ],
                    "text": "perform noise 5000 onepole"
                }
            },
            {
                "box": {
                    "id": "obj-hp1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 290.0, 255.0, 250.0, 22.0 ],
                    "text": "perform noise 200 hp1"
                }
            },
            {
                "box": {
                    "id": "obj-lag",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 550.0, 255.0, 250.0, 22.0 ],
                    "text": "perform noise 0.01 lag"
                }
            },
            {
                "box": {
                    "id": "obj-svflp",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 285.0, 280.0, 22.0 ],
                    "text": "perform noise 2000 0.707 svflp"
                }
            },
            {
                "box": {
                    "id": "obj-svfhp",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 320.0, 285.0, 280.0, 22.0 ],
                    "text": "perform noise 2000 0.707 svfhp"
                }
            },
            {
                "box": {
                    "id": "obj-svfbp",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 610.0, 285.0, 205.0, 22.0 ],
                    "text": "perform noise 1000 5 svfbp"
                }
            },
            {
                "box": {
                    "id": "obj-slew",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 315.0, 370.0, 22.0 ],
                    "text": "perform noise sign 0.001 0.01 slew"
                }
            },
            {
                "box": {
                    "id": "obj-biquad",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 410.0, 315.0, 400.0, 22.0 ],
                    "text": "perform noise 0.0675 0.135 0.0675 -1.143 0.413 biquad"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-delay",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 350.0, 400.0, 20.0 ],
                    "text": "Delay & utilities"
                }
            },
            {
                "box": {
                    "id": "obj-delay",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 375.0, 280.0, 22.0 ],
                    "text": "perform 440 sinosc 22050 delay"
                }
            },
            {
                "box": {
                    "id": "obj-decay",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 320.0, 375.0, 310.0, 22.0 ],
                    "text": "perform noise 10 dust 0.1 decay * 0.3 *"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-buf",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 410.0, 400.0, 20.0 ],
                    "text": "Joy fallback (buffer mode)"
                }
            },
            {
                "box": {
                    "id": "obj-perform1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 435.0, 290.0, 22.0 ],
                    "text": "perform 0. 6.283185 512 vlinspace [sin] map"
                }
            },
            {
                "box": {
                    "id": "obj-perform2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 330.0, 435.0, 310.0, 22.0 ],
                    "text": "perform 0. 6.283185 1024 vlinspace [sin] map"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-input",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 470.0, 400.0, 20.0 ],
                    "text": "Signal input processing"
                }
            },
            {
                "box": {
                    "id": "obj-input1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 495.0, 200.0, 22.0 ],
                    "text": "dsp in1 0.5 *"
                }
            },
            {
                "box": {
                    "id": "obj-input2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 240.0, 495.0, 250.0, 22.0 ],
                    "text": "dsp in1 sin"
                }
            },
            {
                "box": {
                    "id": "obj-cycle",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 630.0, 495.0, 80.0, 22.0 ],
                    "text": "cycle~ 2"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-multi",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 530.0, 400.0, 20.0 ],
                    "text": "Multi-output -- leave N values on stack for N channels"
                }
            },
            {
                "box": {
                    "id": "obj-stereo",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 555.0, 250.0, 22.0 ],
                    "text": "dsp 440 sinosc dup 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-stereo-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 290.0, 555.0, 400.0, 20.0 ],
                    "text": "2 values -> out 1: full sine, out 2: 0.3x sine"
                }
            },
            {
                "box": {
                    "id": "obj-stereo-detune",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 582.0, 340.0, 22.0 ],
                    "text": "dsp 440 sinosc 0.3 * 443 sinosc 0.3 *"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-fb",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 615.0, 400.0, 20.0 ],
                    "text": "Feedback (fbread/fbwrite) -- one-block delay"
                }
            },
            {
                "box": {
                    "id": "obj-fb1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 640.0, 420.0, 22.0 ],
                    "text": "dsp 440 fbread1 + sinosc dup fbwrite1"
                }
            },
            {
                "box": {
                    "id": "obj-fb1-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 460.0, 640.0, 300.0, 20.0 ],
                    "text": "feedback FM synthesis"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-func",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 675.0, 400.0, 20.0 ],
                    "text": "Functions (def/undef/cleardef) -- reusable operation patterns"
                }
            },
            {
                "box": {
                    "id": "obj-func-def",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 700.0, 420.0, 22.0 ],
                    "text": "def osc2 dup 1.003 * sinosc swap sinosc + 0.5 *"
                }
            },
            {
                "box": {
                    "id": "obj-func-use",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 730.0, 220.0, 22.0 ],
                    "text": "perform 440 osc2 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-func-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 260.0, 730.0, 400.0, 20.0 ],
                    "text": "def persists -- define once, use in any expression"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-load",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 760.0, 400.0, 20.0 ],
                    "text": "File loading -- batch load definitions from .dsp files"
                }
            },
            {
                "box": {
                    "id": "obj-load",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 785.0, 180.0, 22.0 ],
                    "text": "load prelude.dsp"
                }
            },
            {
                "box": {
                    "id": "obj-load-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 220.0, 785.0, 500.0, 20.0 ],
                    "text": "load file of def statements from Max search path (also works as 3rd constructor arg)"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-macro",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 820.0, 400.0, 20.0 ],
                    "text": "Let bindings -- name values for reuse"
                }
            },
            {
                "box": {
                    "id": "obj-macro1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 845.0, 360.0, 22.0 ],
                    "text": "dsp 440 sinosc let s s s 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-macro1-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 400.0, 845.0, 400.0, 20.0 ],
                    "text": "let binding: s = sinosc, output full + 0.3x"
                }
            },
            {
                "box": {
                    "id": "obj-multilet",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 875.0, 380.0, 22.0 ],
                    "text": "dsp 440 110 200 let | d m c | m sinosc d * c + sinosc 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-multilet-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 420.0, 876.0, 250.0, 20.0 ],
                    "text": "multi-name let: bind 3 values at once"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-cmp",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 910.0, 400.0, 20.0 ],
                    "text": "Comparisons & conditionals"
                }
            },
            {
                "box": {
                    "id": "obj-cmp1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 935.0, 340.0, 22.0 ],
                    "text": "dsp 440 sinosc let s s 0 gt s * 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-cmp1-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 380.0, 936.0, 280.0, 20.0 ],
                    "text": "half-wave rectifier: zero out negative samples"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-env",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 970.0, 400.0, 20.0 ],
                    "text": "Envelopes -- line (ramp 0->1), ar (attack/release)"
                }
            },
            {
                "box": {
                    "id": "obj-line",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 995.0, 310.0, 22.0 ],
                    "text": "dsp 440 sinosc 1 0.5 line * 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-line-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 350.0, 996.0, 280.0, 20.0 ],
                    "text": "line: triggered 0.5s fade-in"
                }
            },
            {
                "box": {
                    "id": "obj-ar",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 1025.0, 350.0, 22.0 ],
                    "text": "dsp 440 sinosc 1 0.01 0.2 ar * 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-ar-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 390.0, 1026.0, 280.0, 20.0 ],
                    "text": "ar: 10ms attack, 200ms release"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-newdelay",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 1060.0, 400.0, 20.0 ],
                    "text": "Fractional delay & notch filter"
                }
            },
            {
                "box": {
                    "id": "obj-delayf",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 1085.0, 420.0, 22.0 ],
                    "text": "dsp 440 sinosc let s s s 0.3 sinosc 20 * 40 + delayf + 0.5 *"
                }
            },
            {
                "box": {
                    "id": "obj-delayf-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 460.0, 1086.0, 280.0, 20.0 ],
                    "text": "delayf: flanger effect (fractional delay)"
                }
            },
            {
                "box": {
                    "id": "obj-svfnotch",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 1115.0, 260.0, 22.0 ],
                    "text": "perform noise 1000 5 svfnotch 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-svfnotch-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 300.0, 1116.0, 280.0, 20.0 ],
                    "text": "svfnotch: band-reject (notch) filter"
                }
            },
            {
                "box": {
                    "id": "obj-stop",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 700.0, 435.0, 50.0, 22.0 ],
                    "text": "stop"
                }
            },
            {
                "box": {
                    "id": "obj-status",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 760.0, 435.0, 55.0, 22.0 ],
                    "text": "status"
                }
            },
            {
                "box": {
                    "id": "obj-joy",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "signal", "signal", "" ],
                    "patching_rect": [ 30.0, 1155.0, 51.0, 22.0 ],
                    "text": "joy~ 1 2"
                }
            },
            {
                "box": {
                    "id": "obj-gain",
                    "maxclass": "gain~",
                    "multichannelvariant": 0,
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 30.0, 1195.0, 29.0, 150.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-gain2",
                    "maxclass": "gain~",
                    "multichannelvariant": 0,
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 85.0, 1195.0, 29.0, 150.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 170.0, 1195.0, 200.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 170.0, 1220.0, 100.0, 20.0 ],
                    "text": "info outlet"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-ref",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 74.0, 1165.0, 300.0, 20.0 ],
                    "text": "Reference:"
                }
            },
            {
                "box": {
                    "id": "obj-ref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1190.0, 726.0, 20.0 ],
                    "text": "perform <expr> - tries DSP compile first, falls back to Joy eval"
                }
            },
            {
                "box": {
                    "id": "obj-ref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1210.0, 726.0, 20.0 ],
                    "text": "dsp <expr> - compile RPN expression to native DSP graph (no fallback, reports errors)"
                }
            },
            {
                "box": {
                    "id": "obj-ref-load",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1230.0, 726.0, 20.0 ],
                    "text": "load <filename> - load .dsp file of def statements (also as 3rd constructor arg)"
                }
            },
            {
                "box": {
                    "id": "obj-ref2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1250.0, 726.0, 20.0 ],
                    "text": "bang - re-evaluate last perform code (no-op in dsp graph mode)"
                }
            },
            {
                "box": {
                    "id": "obj-ref3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1270.0, 726.0, 20.0 ],
                    "text": "stop - silence output, clear both modes"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-dspref",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 74.0, 1295.0, 400.0, 20.0 ],
                    "text": "DSP expression tokens:"
                }
            },
            {
                "box": {
                    "id": "obj-dspref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1318.0, 780.0, 20.0 ],
                    "text": "math: + - * / pow min max mod clip mix/lerp | cmp: gt lt gte lte eq neq | unary: neg abs sin cos tanh exp log sqrt wrap fold floor ceil round sign db2a a2db mtof ftom"
                }
            },
            {
                "box": {
                    "id": "obj-dspref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1338.0, 780.0, 20.0 ],
                    "text": "gen: sinosc phasor noise pink dust tri saw pulse | env: decay line ar | filt: onepole hp1 lag slew svflp svfhp svfbp svfnotch biquad"
                }
            },
            {
                "box": {
                    "id": "obj-dspref2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1358.0, 780.0, 20.0 ],
                    "text": "delay: delay delayf delayc | util: sah latch | stack: dup swap pop over rot | fb: fbread1-4 fbwrite1-4 | bind: let <name> or let | n1 n2 | | func: def undef cleardef"
                }
            },
            {
                "box": {
                    "id": "obj-dspref3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 1378.0, 780.0, 20.0 ],
                    "text": "inputs: in1..in8, numbers | Multi-output: N stack values -> N outlets | Args: joy~ <num_in> <num_out> [file.dsp] (default 1 1, max 8)"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-ar", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-biquad", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-cmp1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-cycle", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-decay", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-delay", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-delayf", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dsp1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dsp2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dsp3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dsp4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dsp5", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dsp6", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dup", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-fb1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-func-def", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-func-use", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 1 ],
                    "source": [ "obj-gain2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-hp1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-input1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-input2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-gain", 0 ],
                    "source": [ "obj-joy", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-gain2", 0 ],
                    "source": [ "obj-joy", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-info", 1 ],
                    "source": [ "obj-joy", 2 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-lag", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-line", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-load", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-macro1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-mtof", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-multilet", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-onepole", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-perform1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-perform2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-pink", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-slew", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-status", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-stereo", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-stereo-detune", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-stop", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-svfbp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-svfhp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-svflp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-svfnotch", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "hidden": 1,
                    "source": [ "obj-wrap", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}