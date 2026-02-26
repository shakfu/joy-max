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
        "rect": [ 100.0, 87.0, 900.0, 779.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-dac",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 710.0, 45.0, 45.0 ]
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
                    "patching_rect": [ 30.0, 105.0, 180.0, 22.0 ],
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
                    "patching_rect": [ 220.0, 105.0, 150.0, 22.0 ],
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
                    "patching_rect": [ 380.0, 105.0, 160.0, 22.0 ],
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
                    "patching_rect": [ 550.0, 105.0, 200.0, 22.0 ],
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
                    "patching_rect": [ 30.0, 135.0, 180.0, 22.0 ],
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
                    "patching_rect": [ 220.0, 135.0, 280.0, 22.0 ],
                    "text": "100 phasor 2 * 1 - abs"
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
                    "patching_rect": [ 320.0, 195.0, 200.0, 22.0 ],
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
                    "patching_rect": [ 530.0, 195.0, 280.0, 22.0 ],
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
                    "patching_rect": [ 30.0, 255.0, 250.0, 22.0 ],
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
                    "patching_rect": [ 290.0, 255.0, 200.0, 22.0 ],
                    "text": "noise 200 hp1"
                }
            },
            {
                "box": {
                    "id": "obj-svflp",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 285.0, 250.0, 22.0 ],
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
                    "patching_rect": [ 290.0, 285.0, 250.0, 22.0 ],
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
                    "patching_rect": [ 550.0, 285.0, 250.0, 22.0 ],
                    "text": "noise 1000 5 svfbp"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-delay",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 320.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 345.0, 250.0, 22.0 ],
                    "text": "440 sinosc 22050 delay"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-input",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 380.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 405.0, 200.0, 22.0 ],
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
                    "patching_rect": [ 240.0, 405.0, 250.0, 22.0 ],
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
                    "patching_rect": [ 630.0, 405.0, 66.0, 22.0 ],
                    "text": "cycle~ 220"
                }
            },
            {
                "box": {
                    "id": "obj-stop",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 700.0, 345.0, 50.0, 22.0 ],
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
                    "patching_rect": [ 760.0, 345.0, 55.0, 22.0 ],
                    "text": "status"
                }
            },
            {
                "box": {
                    "id": "obj-dg",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "" ],
                    "patching_rect": [ 30.0, 460.0, 92.0, 22.0 ],
                    "text": "dsp_graph~ 1 1"
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
                    "patching_rect": [ 30.0, 500.0, 29.0, 150.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 120.0, 500.0, 200.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 120.0, 525.0, 100.0, 20.0 ],
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
                    "patching_rect": [ 66.0, 560.0, 300.0, 20.0 ],
                    "text": "Reference:"
                }
            },
            {
                "box": {
                    "id": "obj-ref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 81.0, 585.0, 734.0, 20.0 ],
                    "text": "<expr> - any message compiles and runs an RPN DSP expression"
                }
            },
            {
                "box": {
                    "id": "obj-ref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 81.0, 605.0, 734.0, 20.0 ],
                    "text": "stop - silence output and free graph"
                }
            },
            {
                "box": {
                    "id": "obj-ref2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 81.0, 625.0, 734.0, 20.0 ],
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
                    "patching_rect": [ 66.0, 650.0, 400.0, 20.0 ],
                    "text": "DSP expression tokens:"
                }
            },
            {
                "box": {
                    "id": "obj-tok0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 81.0, 673.0, 734.0, 20.0 ],
                    "text": "math: + - * / pow min max mod, clip mix/lerp, neg abs sin cos tanh exp log sqrt wrap fold floor ceil round sign db2a a2db mtof ftom"
                }
            },
            {
                "box": {
                    "id": "obj-tok1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 81.0, 693.0, 734.0, 20.0 ],
                    "text": "gen: sinosc phasor noise tri saw pulse | filt: onepole hp1 svflp svfhp svfbp | util: delay sah latch | stack: dup swap pop over rot"
                }
            },
            {
                "box": {
                    "id": "obj-tok2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 81.0, 713.0, 734.0, 20.0 ],
                    "text": "inputs: in1..in8, numbers | Arguments: dsp_graph~ <num_inputs> <num_outputs> (default: 1 1, max: 8 8)"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-cycle", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
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
                    "destination": [ "obj-info", 1 ],
                    "source": [ "obj-dg", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-dup", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dac", 1 ],
                    "order": 0,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dac", 0 ],
                    "order": 1,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-hp1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-input1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-input2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-mix", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-mtof", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-noise", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-onepole", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-phasor", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-pulse", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-saw", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-sinosc", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-status", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-stop", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-svfbp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-svfhp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-svflp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-dg", 0 ],
                    "source": [ "obj-tri", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}