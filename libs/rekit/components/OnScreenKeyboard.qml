import QtQuick

// On-screen keyboard overlay (story 010, phase 1): QWERTY plus a symbols
// layer, shift, and the essential action keys. Presentational: emits
// signals, never touches the focused application state directly.
Rectangle {
    id: keyboard

    signal keyPressed(string text)
    signal backspacePressed()
    signal enterPressed()
    signal dismissed()

    property bool shiftActive: false
    property bool symbolsActive: false

    readonly property var letterRows: [
        ["q", "w", "e", "r", "t", "y", "u", "i", "o", "p"],
        ["a", "s", "d", "f", "g", "h", "j", "k", "l", "ñ"],
        ["z", "x", "c", "v", "b", "n", "m"]
    ]
    readonly property var symbolRows: [
        ["1", "2", "3", "4", "5", "6", "7", "8", "9", "0"],
        ["-", "_", "/", "\\", "|", "~", "'", "\"", "`", "*"],
        ["!", "?", "@", "#", "$", "%", "&", "(", ")", "="],
        ["[", "]", "{", "}", "<", ">", ";", ":", ",", "."]
    ]

    height: keysColumn.height + 48
    color: "white"

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 3
        color: "black"
    }

    // One key cap. Local component: sized by the keyboard grid.
    component KeyCap: Rectangle {
        id: cap
        property string label: ""
        property bool primary: false
        property bool active: false
        signal tapped()

        readonly property bool highlighted: active || primary || capTapArea.pressed

        width: 118
        height: 92
        radius: 12
        color: highlighted ? "black" : "white"
        border.color: "black"
        border.width: 3

        Text {
            anchors.centerIn: parent
            font.pixelSize: 34
            font.bold: true
            color: cap.highlighted ? "white" : "black"
            text: cap.label
        }

        MouseArea {
            id: capTapArea
            anchors.fill: parent
            onClicked: cap.tapped()
        }
    }

    Column {
        id: keysColumn
        anchors.top: parent.top
        anchors.topMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 14

        Repeater {
            model: keyboard.symbolsActive ? keyboard.symbolRows : keyboard.letterRows
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 12
                Repeater {
                    model: modelData
                    KeyCap {
                        label: keyboard.shiftActive && !keyboard.symbolsActive
                               ? modelData.toUpperCase() : modelData
                        onTapped: {
                            keyboard.keyPressed(label)
                            keyboard.shiftActive = false
                        }
                    }
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 12

            // Word key caps, early-iOS style — also avoids glyphs missing
            // from the device fonts.
            KeyCap {
                label: "shift"
                visible: !keyboard.symbolsActive
                active: keyboard.shiftActive
                onTapped: keyboard.shiftActive = !keyboard.shiftActive
            }
            KeyCap {
                label: keyboard.symbolsActive ? "abc" : "?123"
                onTapped: keyboard.symbolsActive = !keyboard.symbolsActive
            }
            KeyCap {
                width: 420
                label: ""
                onTapped: keyboard.keyPressed(" ")
            }
            KeyCap {
                label: "del"
                onTapped: keyboard.backspacePressed()
            }
            KeyCap {
                label: "enter"
                primary: true
                onTapped: keyboard.enterPressed()
            }
            KeyCap {
                label: "hide"
                onTapped: keyboard.dismissed()
            }
        }
    }
}
