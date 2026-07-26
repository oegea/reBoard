import QtQuick
import QtQuick.Window
import "qrc:/rekit/components"

Window {
    id: root
    visible: true
    width: 1404
    height: 1872
    color: "white"
    title: "Terminal"

    Item {
        id: content
        readonly property bool rotated: uiRotation === 90 || uiRotation === 270
        width: rotated ? root.height : root.width
        height: rotated ? root.width : root.height
        anchors.centerIn: parent
        rotation: uiRotation

        // Slim header: title left, on-screen keyboard toggle right.
        Rectangle {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 64
            color: "white"

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 32
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 30
                font.bold: true
                color: "black"
                text: "Terminal" + (terminalVm.shellRunning ? "" : " — " + qsTr("session ended"))
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 3
                color: "black"
            }
        }

        // Accessory keys a real shell needs (Ctrl is a latch for the next
        // key, like mobile terminals do).
        component AccessoryKey: Rectangle {
            id: accessory
            property string label: ""
            property bool active: false
            signal tapped()
            width: 96
            height: 56
            radius: 10
            color: active ? "black" : "white"
            border.color: "black"
            border.width: 3
            Text {
                anchors.centerIn: parent
                font.pixelSize: 24
                font.bold: true
                color: accessory.active ? "white" : "black"
                text: accessory.label
            }
            MouseArea {
                anchors.fill: parent
                onClicked: accessory.tapped()
            }
        }

        Row {
            id: accessoryBar
            anchors.top: header.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 24
            spacing: 12

            AccessoryKey { label: "esc"; onTapped: terminalVm.sendKey(Qt.Key_Escape, 0, "") }
            AccessoryKey { label: "tab"; onTapped: terminalVm.sendKey(Qt.Key_Tab, 0, "") }
            AccessoryKey {
                id: ctrlKey
                label: "ctrl"
                onTapped: active = !active
            }
            AccessoryKey { label: "up"; onTapped: terminalVm.sendKey(Qt.Key_Up, 0, "") }
            AccessoryKey { label: "down"; onTapped: terminalVm.sendKey(Qt.Key_Down, 0, "") }
            AccessoryKey { label: "left"; onTapped: terminalVm.sendKey(Qt.Key_Left, 0, "") }
            AccessoryKey { label: "right"; onTapped: terminalVm.sendKey(Qt.Key_Right, 0, "") }
        }

        // Sends OSK/typed text honoring the Ctrl latch.
        function sendTypedText(text) {
            if (ctrlKey.active && text.length === 1) {
                ctrlKey.active = false
                const upper = text.toUpperCase().charCodeAt(0)
                if (upper >= 65 && upper <= 90) {
                    terminalVm.sendKey(Qt.Key_A + (upper - 65), Qt.ControlModifier, "")
                    return
                }
            }
            terminalVm.sendText(text)
        }

        // Terminal grid.
        Item {
            id: screenArea
            anchors.top: accessoryBar.bottom
            anchors.topMargin: 12
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.right: parent.right
            anchors.rightMargin: 24
            anchors.bottom: osk.visible ? osk.top : parent.bottom
            anchors.bottomMargin: 12
            clip: true

            focus: true
            Keys.onPressed: (event) => {
                if (ctrlKey.active && event.text.length === 1) {
                    content.sendTypedText(event.text)
                } else {
                    terminalVm.sendKey(event.key, event.modifiers, event.text)
                }
                event.accepted = true
            }

            // Measure a long run and divide: a single-glyph advance misses
            // the fractional accumulation the layout engine applies, which
            // made the cursor drift right of the text.
            TextMetrics {
                id: cellMetrics
                font.family: "monospace"
                font.pixelSize: 26
                text: "M".repeat(50)
            }

            readonly property real cellWidth: cellMetrics.advanceWidth / 50
            readonly property real cellHeight: Math.ceil(cellMetrics.height)
            // One column of safety margin: if the metric slightly
            // underestimates the advance, the last column would clip.
            readonly property int gridColumns: Math.max(20, Math.floor(width / cellWidth) - 1)
            readonly property int gridRows: Math.max(5, Math.floor(height / cellHeight))

            onGridColumnsChanged: terminalVm.setGridSize(gridColumns, gridRows)
            onGridRowsChanged: terminalVm.setGridSize(gridColumns, gridRows)
            Component.onCompleted: terminalVm.startShell(gridColumns, gridRows)

            Column {
                Repeater {
                    model: terminalVm.lines
                    Text {
                        height: screenArea.cellHeight
                        font.family: "monospace"
                        font.pixelSize: 26
                        color: "black"
                        textFormat: Text.RichText
                        text: modelData
                    }
                }
            }

            // (The block cursor is rendered inside the text itself as an
            // inverted cell — see TerminalViewModel — so it can never drift.)

            // System convention (ADR-0005): tapping a typing surface
            // summons the on-screen keyboard, unless a physical keyboard is
            // attached; the keyboard's own hide key dismisses it.
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    screenArea.forceActiveFocus()
                    if (!osk.visible && !terminalVm.physicalKeyboardPresent()) {
                        osk.visible = true
                    }
                }
            }
        }

        // Gesture-zone hint, standard on every screen (the home gesture
        // works over the keyboard area too).
        HomeMarker {
            z: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
        }

        OnScreenKeyboard {
            id: osk
            visible: false
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            onKeyPressed: (text) => content.sendTypedText(text)
            onBackspacePressed: terminalVm.sendKey(Qt.Key_Backspace, 0, "")
            onEnterPressed: terminalVm.sendKey(Qt.Key_Return, 0, "")
            onDismissed: visible = false
        }
    }
}
