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
        "rect": [ 100.0, 100.0, 872.0, 903.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-1",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 780.0, 45.0, 45.0 ]
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
                    "text": "perform 100 phasor dup 2 * 1 - abs"
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
                    "patching_rect": [ 30.0, 345.0, 280.0, 22.0 ],
                    "text": "perform 440 sinosc 22050 delay"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-buf",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 380.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 405.0, 290.0, 22.0 ],
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
                    "patching_rect": [ 330.0, 405.0, 310.0, 22.0 ],
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
                    "patching_rect": [ 30.0, 440.0, 400.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 465.0, 200.0, 22.0 ],
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
                    "patching_rect": [ 240.0, 465.0, 250.0, 22.0 ],
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
                    "patching_rect": [ 630.0, 465.0, 80.0, 22.0 ],
                    "text": "cycle~ 2"
                }
            },
            {
                "box": {
                    "id": "obj-stop",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 700.0, 405.0, 50.0, 22.0 ],
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
                    "patching_rect": [ 760.0, 405.0, 55.0, 22.0 ],
                    "text": "status"
                }
            },
            {
                "box": {
                    "id": "obj-joy",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "" ],
                    "patching_rect": [ 30.0, 520.0, 51.0, 22.0 ],
                    "text": "joy~ 1 1"
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
                    "patching_rect": [ 30.0, 560.0, 29.0, 150.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 120.0, 560.0, 200.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 120.0, 585.0, 100.0, 20.0 ],
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
                    "patching_rect": [ 74.0, 629.0, 300.0, 20.0 ],
                    "text": "Reference:"
                }
            },
            {
                "box": {
                    "id": "obj-ref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 654.0, 726.0, 20.0 ],
                    "text": "perform <expr> - tries DSP compile first, falls back to Joy eval"
                }
            },
            {
                "box": {
                    "id": "obj-ref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 674.0, 726.0, 20.0 ],
                    "text": "dsp <expr> - compile RPN expression to native DSP graph (no fallback, reports errors)"
                }
            },
            {
                "box": {
                    "id": "obj-ref2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 694.0, 726.0, 20.0 ],
                    "text": "bang - re-evaluate last perform code (no-op in dsp graph mode)"
                }
            },
            {
                "box": {
                    "id": "obj-ref3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 714.0, 726.0, 20.0 ],
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
                    "patching_rect": [ 74.0, 739.0, 400.0, 20.0 ],
                    "text": "DSP expression tokens:"
                }
            },
            {
                "box": {
                    "id": "obj-dspref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 762.0, 726.0, 20.0 ],
                    "text": "math: + - * / pow min max mod, clip mix/lerp, neg abs sin cos tanh exp log sqrt wrap fold floor ceil round sign db2a a2db mtof ftom"
                }
            },
            {
                "box": {
                    "id": "obj-dspref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 782.0, 726.0, 20.0 ],
                    "text": "gen: sinosc phasor noise tri saw pulse | filt: onepole hp1 svflp svfhp svfbp | util: delay sah latch | stack: dup swap pop over rot"
                }
            },
            {
                "box": {
                    "id": "obj-dspref2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 89.0, 802.0, 726.0, 20.0 ],
                    "text": "inputs: in1..in8, numbers | Arguments: joy~ <num_inputs> <num_outputs> (default: 1 1, max: 8 8)"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-cycle", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-delay", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-dsp1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-dsp2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-dsp3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-dsp4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-dsp5", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-dsp6", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-dup", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 1 ],
                    "order": 0,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "order": 1,
                    "source": [ "obj-gain", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-hp1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-input1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
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
                    "destination": [ "obj-info", 1 ],
                    "source": [ "obj-joy", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-mtof", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-onepole", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-perform1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-perform2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-status", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-stop", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-svfbp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-svfhp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-svflp", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-wrap", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}