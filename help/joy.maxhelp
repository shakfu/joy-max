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
        "rect": [ 100.0, 100.0, 800.0, 600.0 ],
        "boxes": [
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 217.0, 79.0, 22.0 ],
                    "text": "loadmess set"
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
                    "patching_rect": [ 15.0, 15.0, 300.0, 24.0 ],
                    "text": "joy - Joy language interpreter for Max"
                }
            },
            {
                "box": {
                    "id": "obj-desc",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 15.0, 45.0, 500.0, 20.0 ],
                    "text": "Evaluate Joy code and output results as Max messages"
                }
            },
            {
                "box": {
                    "id": "obj-msg1",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 105.0, 100.0, 22.0 ],
                    "text": "2 3 +"
                }
            },
            {
                "box": {
                    "id": "obj-msg2",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 150.0, 105.0, 120.0, 22.0 ],
                    "text": "[1 2 3] [4 5 6] concat"
                }
            },
            {
                "box": {
                    "id": "obj-msg3",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 290.0, 105.0, 120.0, 22.0 ],
                    "text": "[1 2 3 4 5] 0 [+] fold"
                }
            },
            {
                "box": {
                    "id": "obj-bang",
                    "maxclass": "button",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 430.0, 105.0, 24.0, 24.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-clear",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 470.0, 105.0, 50.0, 22.0 ],
                    "text": "clear"
                }
            },
            {
                "box": {
                    "id": "obj-stack",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 530.0, 105.0, 50.0, 22.0 ],
                    "text": "stack"
                }
            },
            {
                "box": {
                    "id": "obj-reset",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 590.0, 105.0, 50.0, 22.0 ],
                    "text": "reset"
                }
            },
            {
                "box": {
                    "id": "obj-joy",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "", "" ],
                    "patching_rect": [ 30.0, 165.0, 420.0, 22.0 ],
                    "text": "joy"
                }
            },
            {
                "box": {
                    "id": "obj-result",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 30.0, 255.0, 200.0, 22.0 ],
                    "text": "1 2 3 4 5"
                }
            },
            {
                "box": {
                    "id": "obj-error",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 250.0, 255.0, 200.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-result-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 280.0, 100.0, 20.0 ],
                    "text": "result outlet"
                }
            },
            {
                "box": {
                    "id": "obj-error-label",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 250.0, 280.0, 100.0, 20.0 ],
                    "text": "error outlet"
                }
            },
            {
                "box": {
                    "fontface": 1,
                    "id": "obj-section2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 15.0, 320.0, 300.0, 20.0 ],
                    "text": "Messages:"
                }
            },
            {
                "box": {
                    "id": "obj-help1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 345.0, 500.0, 20.0 ],
                    "text": "anything - evaluate Joy code (e.g. '2 3 +' or '[1 2 3] reverse')"
                }
            },
            {
                "box": {
                    "id": "obj-help2",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 365.0, 500.0, 20.0 ],
                    "text": "bang - re-evaluate the last code"
                }
            },
            {
                "box": {
                    "id": "obj-help3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 385.0, 500.0, 20.0 ],
                    "text": "clear - clear the Joy stack"
                }
            },
            {
                "box": {
                    "id": "obj-help4",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 405.0, 500.0, 20.0 ],
                    "text": "stack - output all stack contents from the result outlet"
                }
            },
            {
                "box": {
                    "id": "obj-help5",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 30.0, 425.0, 500.0, 20.0 ],
                    "text": "reset - destroy and recreate the Joy interpreter context"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-error", 0 ],
                    "order": 0,
                    "source": [ "obj-3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-result", 0 ],
                    "order": 1,
                    "source": [ "obj-3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-bang", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-clear", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-error", 1 ],
                    "source": [ "obj-joy", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-result", 1 ],
                    "source": [ "obj-joy", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-msg1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-msg2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-msg3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-reset", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-joy", 0 ],
                    "source": [ "obj-stack", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}