import QtQuick

// Early-iOS-inspired "slide to unlock" control: a recessed track with a
// draggable knob; dragging the knob to the far end emits `unlocked`.
// E-paper friendly: the knob snaps, nothing animates.
Item {
    id: slider

    property string label: qsTr("slide to unlock")

    signal unlocked()

    height: 110

    function reset() {
        knob.x = 8
    }

    Rectangle {
        id: track
        anchors.fill: parent
        radius: height / 2
        color: "white"
        border.color: "black"
        border.width: 4

        Text {
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 30
            font.pixelSize: 36
            color: "black"
            opacity: 0.45
            text: slider.label
        }
    }

    Rectangle {
        id: knob
        x: 8
        y: 8
        width: 170
        height: parent.height - 16
        radius: 20
        color: "black"

        // Drawn ">" chevron (device fonts lack arrow glyphs).
        Item {
            anchors.centerIn: parent
            width: 34
            height: 44

            Rectangle {
                x: 2
                y: 12
                width: 28
                height: 9
                radius: 4
                color: "white"
                rotation: 45
            }
            Rectangle {
                x: 2
                y: 24
                width: 28
                height: 9
                radius: 4
                color: "white"
                rotation: -45
            }
        }

        MouseArea {
            anchors.fill: parent
            drag.target: knob
            drag.axis: Drag.XAxis
            drag.minimumX: 8
            drag.maximumX: slider.width - knob.width - 8
            onReleased: {
                const reached = knob.x >= slider.width - knob.width - 24
                knob.x = 8
                if (reached) {
                    slider.unlocked()
                }
            }
        }
    }
}
