import "." as Skin
import QtQuick 2.12
import QtQuick.Shapes 1.12
import QtQuick.Layouts
import Mixxx 1.0 as Mixxx
import "Theme"

Column {
    id: root

    required property string group
    property var player: Mixxx.PlayerManager.getPlayer(root.group)

    Mixxx.ControlProxy {
        id: stemCountControl

        group: root.group
        key: "stem_count"
    }

    Row {
        Column {
            id: stem
            spacing: 4
            width: undefined
            Repeater {
                model: root.player.stemsModel

                Row {
                    id: stem
                    required property int index
                    required property string label
                    required property color color

                    Rectangle {
                        id: stemRect
                        width: 56
                        height: 56
                        color: stem.color
                        radius: 5

                        Skin.ControlKnob {
                            id: knob
                            group: root.group
                            key: `stem_${index}_volume`
                            color: Theme.gainKnobColor
                            anchors.topMargin: 5
                            anchors.top: stemRect.top
                            anchors.horizontalCenter: stemRect.horizontalCenter

                            width: 36
                            height: 36
                        }
                        Text {
                            anchors.bottom: stemRect.bottom
                            anchors.horizontalCenter: stemRect.horizontalCenter
                            text: label
                        }
                    }
                }
            }
        }
        Column {
            id: eq
            spacing: 4
            width: undefined
            Skin.EqKnob {
                statusKey: "button_parameter3"
                knob.group: "[EqualizerRack1_" + root.group + "_Effect1]"
                knob.key: "parameter3"
                knob.color: Theme.eqHighColor
            }

            Skin.EqKnob {
                statusKey: "button_parameter2"
                knob.group: "[EqualizerRack1_" + root.group + "_Effect1]"
                knob.key: "parameter2"
                knob.color: Theme.eqMidColor
            }

            Skin.EqKnob {
                knob.group: "[EqualizerRack1_" + root.group + "_Effect1]"
                knob.key: "parameter1"
                statusKey: "button_parameter1"
                knob.color: Theme.eqLowColor
            }

            Skin.EqKnob {
                knob.group: "[QuickEffectRack1_" + root.group + "]"
                knob.key: "super1"
                statusGroup: "[QuickEffectRack1_" + root.group + "_Effect1]"
                statusKey: "enabled"
                knob.arcStyle: ShapePath.DashLine
                knob.arcStylePattern: [2, 2]
                knob.color: Theme.eqFxColor
            }
        }
        states: [
            State {
                name: "eq"
                when: stemCountControl.value == 0
                PropertyChanges { target: stem; opacity: 0; width: 0}
            },
            State {
                name: "stem"
                when: stemCountControl.value != 0
                PropertyChanges { target: eq; opacity: 0; width: 0 }
            }
        ]

        transitions: [
            Transition {
                from: "eq"
                to: "stem"
                ParallelAnimation {
                    PropertyAnimation { targets: [eq, stem]; properties: "opacity,width"; duration: 1000}
                }
            },
            Transition {
                from: "stem"
                to: "eq"
                ParallelAnimation {
                    PropertyAnimation { target: stem; properties: "opacity"; duration: 1000}
                    PropertyAnimation { target: eq; properties: "opacity"; duration: 1000}
                }
            }
        ]
    }

    Skin.OrientationToggleButton {
        group: root.group
        key: "orientation"
        color: Theme.crossfaderOrientationColor
    }
}
