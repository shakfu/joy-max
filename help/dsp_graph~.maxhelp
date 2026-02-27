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
        "rect": [ 100.0, 87.0, 991.0, 779.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-dac",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 1051.0, 45.0, 45.0 ]
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
                    "text": "dsp_graph~ - Compiled DSP graph external for Max/MSP"
                }
            },
            {
                "box": {
                    "id": "obj-desc",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 15.0, 45.0, 850.0, 20.0 ],
                    "text": "Compiles stack-based RPN expressions to a native DSP graph. No Joy dependency. Any message is a DSP expression."
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-gen",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 80.0, 400.0, 20.0 ],
                    "text": "Generators"
                }
            },
            {
                "box": {
                    "id": "obj-sinosc",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 105.0, 103.0, 22.0 ],
                    "text": "440 sinosc 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-tri",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 148.0, 105.0, 76.0, 22.0 ],
                    "text": "440 tri 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-saw",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 240.0, 105.0, 93.0, 22.0 ],
                    "text": "440 saw 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-pulse",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 349.0, 105.0, 120.0, 22.0 ],
                    "text": "440 0.5 pulse 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-noise",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 135.0, 73.0, 22.0 ],
                    "text": "noise 0.2 *"
                }
            },
            {
                "box": {
                    "id": "obj-phasor",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 148.0, 135.0, 135.0, 22.0 ],
                    "text": "100 phasor 2 * 1 - abs"
                }
            },
            {
                "box": {
                    "id": "obj-pink",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 294.0, 135.0, 73.0, 22.0 ],
                    "text": "pink 0.2 *"
                }
            },
            {
                "box": {
                    "id": "obj-dust",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 383.0, 135.0, 114.0, 22.0 ],
                    "text": "10 dust 0.1 decay"
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
                    "patching_rect": [ 30.0, 195.0, 137.0, 22.0 ],
                    "text": "440 sinosc dup * 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-mtof",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 174.0, 195.0, 126.0, 22.0 ],
                    "text": "69 mtof sinosc 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-mix",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 311.0, 195.0, 177.0, 22.0 ],
                    "text": "440 sinosc 880 tri 0.5 mix 0.3 *"
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
                    "patching_rect": [ 30.0, 255.0, 129.0, 22.0 ],
                    "text": "noise 5000 onepole"
                }
            },
            {
                "box": {
                    "id": "obj-hp1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 167.0, 255.0, 97.0, 22.0 ],
                    "text": "noise 200 hp1"
                }
            },
            {
                "box": {
                    "id": "obj-lag",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 285.0, 255.0, 96.0, 22.0 ],
                    "text": "noise 0.01 lag"
                }
            },
            {
                "box": {
                    "id": "obj-svflp",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 285.0, 138.0, 22.0 ],
                    "text": "noise 2000 0.707 svflp"
                }
            },
            {
                "box": {
                    "id": "obj-svfhp",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 170.0, 285.0, 147.0, 22.0 ],
                    "text": "noise 2000 0.707 svfhp"
                }
            },
            {
                "box": {
                    "id": "obj-svfbp",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 335.0, 285.0, 115.0, 22.0 ],
                    "text": "noise 1000 5 svfbp"
                }
            },
            {
                "box": {
                    "id": "obj-slew",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 315.0, 173.0, 22.0 ],
                    "text": "noise sign 0.001 0.01 slew"
                }
            },
            {
                "box": {
                    "id": "obj-biquad",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 192.0, 315.0, 270.0, 22.0 ],
                    "text": "noise 0.0675 0.135 0.0675 -1.143 0.413 biquad"
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
                    "patching_rect": [ 30.0, 375.0, 151.0, 22.0 ],
                    "text": "440 sinosc 22050 delay"
                }
            },
            {
                "box": {
                    "id": "obj-decay",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 184.0, 375.0, 183.0, 22.0 ],
                    "text": "noise 10 dust 0.1 decay * 0.3 *"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-input",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 410.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 435.0, 61.0, 22.0 ],
                    "text": "in1 0.5 *"
                }
            },
            {
                "box": {
                    "id": "obj-input2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 240.0, 435.0, 127.0, 22.0 ],
                    "text": "in1 2000 0.707 svflp"
                }
            },
            {
                "box": {
                    "id": "obj-cycle",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 437.0, 783.0, 66.0, 22.0 ],
                    "text": "cycle~ 220"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-multi",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 468.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 493.0, 129.0, 22.0 ],
                    "text": "440 sinosc dup 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-stereo-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 155.0, 495.0, 400.0, 20.0 ],
                    "text": "2 values on stack -> output 1: full sine, output 2: 0.3x sine"
                }
            },
            {
                "box": {
                    "id": "obj-stereo-detune",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 520.0, 185.0, 22.0 ],
                    "text": "440 sinosc 0.3 * 443 sinosc 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-stereo-detune-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 235.0, 522.0, 350.0, 20.0 ],
                    "text": "stereo detuned pair -- L and R slightly different"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-fb",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 553.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 578.0, 200.0, 22.0 ],
                    "text": "440 fbread1 + sinosc dup fbwrite1"
                }
            },
            {
                "box": {
                    "id": "obj-fb1-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 218.5, 579.0, 290.0, 20.0 ],
                    "text": "feedback FM -- fbread1 modulates sinosc frequency"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-func",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 613.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 638.0, 275.0, 22.0 ],
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
                    "patching_rect": [ 30.0, 668.0, 92.0, 22.0 ],
                    "text": "440 osc2 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-func-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 128.0, 668.0, 279.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 698.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 723.0, 103.0, 22.0 ],
                    "text": "load prelude.dsp"
                }
            },
            {
                "box": {
                    "id": "obj-load-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 134.0, 724.0, 459.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 758.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 783.0, 151.0, 22.0 ],
                    "text": "440 sinosc let s s s 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-macro1-comment",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 190.0, 784.0, 226.0, 20.0 ],
                    "text": "let binding: s = sinosc, output full + 0.3x"
                }
            },
            {
                "box": {
                    "id": "obj-stop",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 530.0, 783.0, 50.0, 22.0 ],
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
                    "patching_rect": [ 590.0, 783.0, 55.0, 22.0 ],
                    "text": "status"
                }
            },
            {
                "box": {
                    "id": "obj-dg",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "signal", "signal", "" ],
                    "patching_rect": [ 30.0, 828.0, 92.0, 22.0 ],
                    "text": "dsp_graph~ 1 2"
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
                    "patching_rect": [ 30.0, 873.0, 29.0, 150.0 ]
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
                    "patching_rect": [ 85.0, 873.0, 29.0, 150.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 445.0, 834.0, 200.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 170.0, 898.0, 100.0, 20.0 ],
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
                    "patching_rect": [ 125.0, 835.0, 300.0, 20.0 ],
                    "text": "Reference:"
                }
            },
            {
                "box": {
                    "id": "obj-ref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 860.0, 424.0, 20.0 ],
                    "text": "<expr> - any message compiles and runs an RPN DSP expression"
                }
            },
            {
                "box": {
                    "id": "obj-ref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 880.0, 424.0, 20.0 ],
                    "text": "stop - silence output and free graph"
                }
            },
            {
                "box": {
                    "id": "obj-ref-load",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 900.0, 424.0, 20.0 ],
                    "text": "load <filename> - load .dsp file of def statements (also as 3rd constructor arg)"
                }
            },
            {
                "box": {
                    "id": "obj-ref3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 920.0, 424.0, 20.0 ],
                    "text": "status - post info to Max console and info outlet"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-tokens",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 125.0, 945.0, 400.0, 20.0 ],
                    "text": "DSP expression tokens:"
                }
            },
            {
                "box": {
                    "id": "obj-tok0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 968.0, 734.0, 20.0 ],
                    "text": "math: + - * / pow min max mod, clip mix/lerp, neg abs sin cos tanh exp log sqrt wrap fold floor ceil round sign db2a a2db mtof ftom"
                }
            },
            {
                "box": {
                    "id": "obj-tok1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 988.0, 734.0, 20.0 ],
                    "text": "gen: sinosc phasor noise pink dust tri saw pulse | env: decay | filt: onepole hp1 lag slew svflp svfhp svfbp biquad"
                }
            },
            {
                "box": {
                    "id": "obj-tok2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 1008.0, 834.0, 20.0 ],
                    "text": "util: delay sah latch | stack: dup swap pop over rot | fb: fbread1-4 fbwrite1-4 | bind: <expr> let <name> | func: def <name> <body> | undef <name> | cleardef"
                }
            },
            {
                "box": {
                    "id": "obj-tok3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 140.0, 1028.0, 741.0, 20.0 ],
                    "text": "inputs: in1..in8, numbers | Multi-output: N stack values -> N outlets | Args: dsp_graph~ <num_in> <num_out> [file.dsp] (default 1 1, max 8)"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-biquad", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-cycle", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-decay", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-delay", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-gain", 0 ],
                    "source": [ "obj-dg", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-gain2", 0 ],
                    "source": [ "obj-dg", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-info", 1 ],
                    "source": [ "obj-dg", 2 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dup", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-dust", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-fb1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-func-def", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-func-use", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dac", 0 ],
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dac", 1 ],
                    "source": [ "obj-gain2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-hp1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-input1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-input2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-lag", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-load", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-macro1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-mix", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-mtof", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-noise", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-onepole", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-phasor", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-pink", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-pulse", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-saw", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-sinosc", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-slew", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-status", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-stereo", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-stereo-detune", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-stop", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-svfbp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-svfhp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-svflp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "hidden": 1,
                    "source": [ "obj-tri", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}