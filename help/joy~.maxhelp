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
        "rect": [ 100.0, 100.0, 900.0, 780.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-1",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 640.0, 45.0, 45.0 ]
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
                    "text": "Two modes: 'perform' evaluates Joy code into a looping buffer. 'dsp' compiles an RPN expression to a real-time DSP graph."
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
                    "text": "DSP graph mode (real-time, zero allocation)"
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
                    "text": "dsp 440 sinosc"
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
                    "text": "dsp 440 sinosc 0.3 *"
                }
            },
            {
                "box": {
                    "id": "obj-dsp3",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 430.0, 105.0, 250.0, 22.0 ],
                    "text": "dsp 100 phasor 2 * 1 - abs"
                }
            },
            {
                "box": {
                    "id": "obj-dsp4",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 135.0, 120.0, 22.0 ],
                    "text": "dsp noise"
                }
            },
            {
                "box": {
                    "id": "obj-dsp5",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 160.0, 135.0, 200.0, 22.0 ],
                    "text": "dsp in1 sin"
                }
            },
            {
                "box": {
                    "id": "obj-dsp6",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 370.0, 135.0, 250.0, 22.0 ],
                    "text": "dsp 440 sinosc in1 *"
                }
            },
            {
                "box": {
                    "id": "obj-cycle",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 630.0, 135.0, 80.0, 22.0 ],
                    "text": "cycle~ 2"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section-buf",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 175.0, 400.0, 20.0 ],
                    "text": "Buffer mode (Joy eval, looping waveform)"
                }
            },
            {
                "box": {
                    "id": "obj-perform1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 200.0, 290.0, 22.0 ],
                    "text": "perform 0.0 6.283185307 512 vlinspace [sin] map"
                }
            },
            {
                "box": {
                    "id": "obj-perform2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 330.0, 200.0, 310.0, 22.0 ],
                    "text": "perform 0.0 6.283185307 1024 vlinspace [sin] map"
                }
            },
            {
                "box": {
                    "id": "obj-perform3",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 230.0, 350.0, 22.0 ],
                    "text": "perform 0.0 6.283185307 256 vlinspace [3 * sin] map"
                }
            },
            {
                "box": {
                    "id": "obj-perform4",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 400.0, 230.0, 180.0, 22.0 ],
                    "text": "perform 0.5"
                }
            },
            {
                "box": {
                    "id": "obj-stop",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 700.0, 200.0, 50.0, 22.0 ],
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
                    "patching_rect": [ 760.0, 200.0, 55.0, 22.0 ],
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
                    "patching_rect": [ 30.0, 310.0, 50.0, 22.0 ],
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
                    "patching_rect": [ 30.0, 370.0, 29.0, 180.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 120.0, 370.0, 200.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-info-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 120.0, 395.0, 100.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 430.0, 300.0, 20.0 ],
                    "text": "Reference:"
                }
            },
            {
                "box": {
                    "id": "obj-ref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 455.0, 800.0, 20.0 ],
                    "text": "perform <Joy code> - evaluate Joy, loop result as waveform (message thread, GC-safe)"
                }
            },
            {
                "box": {
                    "id": "obj-ref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 475.0, 800.0, 20.0 ],
                    "text": "dsp <expr> - compile RPN expression to native DSP graph (real-time, zero allocation)"
                }
            },
            {
                "box": {
                    "id": "obj-ref2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 495.0, 800.0, 20.0 ],
                    "text": "bang - re-evaluate last perform code (no-op in dsp mode)"
                }
            },
            {
                "box": {
                    "id": "obj-ref3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 515.0, 800.0, 20.0 ],
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
                    "patching_rect": [ 30.0, 545.0, 400.0, 20.0 ],
                    "text": "DSP expression tokens:"
                }
            },
            {
                "box": {
                    "id": "obj-dspref0",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 570.0, 800.0, 20.0 ],
                    "text": "numbers, in1..in8, + - * / pow min max, sin cos tanh exp log sqrt abs neg, sinosc phasor noise"
                }
            },
            {
                "box": {
                    "id": "obj-dspref1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 45.0, 590.0, 800.0, 20.0 ],
                    "text": "Arguments: joy~ <num_inputs> <num_outputs> (default: 1 1, max: 8 8)"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-1", 1 ],
                    "source": [ "obj-gain", 0 ]
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
                    "source": [ "obj-cycle", 0 ]
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
                    "source": [ "obj-perform3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-perform4", 0 ]
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
            }
        ],
        "autosave": 0
    }
}