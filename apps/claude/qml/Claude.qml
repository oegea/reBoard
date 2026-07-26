import QtQuick
import QtQuick.Window
import "qrc:/rekit/components"

Window {
    id: root
    visible: true
    width: 1404
    height: 1872
    color: "white"
    title: "Claude Code"

    Item {
        id: content
        readonly property bool rotated: uiRotation === 90 || uiRotation === 270
        width: rotated ? root.height : root.width
        height: rotated ? root.width : root.height
        anchors.centerIn: parent
        rotation: uiRotation

        property string pendingInput: ""

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
                text: "Claude Code" + (claudeVm.configured ? "  ·  " + claudeVm.host : "")
            }

            Text {
                anchors.right: parent.right
                anchors.rightMargin: 32
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 26
                color: "black"
                opacity: 0.6
                text: claudeVm.status
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 3
                color: "black"
            }
        }

        // Not configured yet: explain the phase-1 config file.
        Column {
            visible: !claudeVm.configured
            anchors.centerIn: parent
            width: parent.width - 240
            spacing: 32

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.pixelSize: 32
                color: "black"
                text: qsTr("To connect to a remote Claude Code session, create ~/.config/reboard/claude.conf on the tablet with:\n\nhost=user@your-machine\ndirectory=/path/to/project\n\nSSH key authentication is required.")
            }
        }

        // Transcript: lazy list, one delegate per entry.
        ListView {
            id: transcriptView
            visible: claudeVm.configured
            anchors.top: header.bottom
            anchors.topMargin: 24
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: inputBar.top
            anchors.bottomMargin: 16
            anchors.leftMargin: 40
            anchors.rightMargin: 40
            clip: true
            spacing: 28
            boundsBehavior: Flickable.StopAtBounds
            model: claudeVm.transcript
            onCountChanged: positionViewAtEnd()

            delegate: Column {
                width: ListView.view.width
                spacing: 8

                Text {
                    font.pixelSize: 24
                    font.bold: true
                    color: "black"
                    opacity: modelData.role === "assistant" ? 1 : 0.55
                    text: modelData.role === "assistant" ? "Claude"
                        : modelData.role === "user" ? qsTr("You")
                        : modelData.role === "tool" ? qsTr("Action") : qsTr("System")
                }

                Text {
                    width: parent.width
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: 29
                    font.family: modelData.role === "tool" ? "monospace" : "sans-serif"
                    color: "black"
                    opacity: modelData.role === "assistant" || modelData.role === "user" ? 1 : 0.6
                    text: modelData.text
                }
            }
        }

        ScrollIndicator {
            target: transcriptView
            visible: transcriptView.visible &&
                     transcriptView.contentHeight > transcriptView.height
            anchors.top: transcriptView.top
            anchors.bottom: transcriptView.bottom
            anchors.right: parent.right
            anchors.rightMargin: 16
        }

        // Prompt input: shows what is being typed via the on-screen
        // keyboard or the Type Folio, sends on enter.
        Rectangle {
            id: inputBar
            visible: claudeVm.configured
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: osk.visible ? osk.top : parent.bottom
            anchors.leftMargin: 40
            anchors.rightMargin: 40
            anchors.bottomMargin: 20
            height: 92
            radius: 16
            color: "white"
            border.color: "black"
            border.width: 3

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 24
                anchors.right: sendButton.left
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 30
                color: "black"
                opacity: content.pendingInput === "" ? 0.4 : 1
                elide: Text.ElideLeft
                text: content.pendingInput === "" ? qsTr("Ask Claude…") : content.pendingInput
            }

            PushButton {
                id: sendButton
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                width: 170
                height: 72
                label: qsTr("Send")
                primary: content.pendingInput !== "" && !claudeVm.busy
                onClicked: {
                    if (content.pendingInput !== "" && !claudeVm.busy) {
                        claudeVm.sendPrompt(content.pendingInput)
                        content.pendingInput = ""
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                anchors.rightMargin: 180
                onClicked: {
                    inputKeys.forceActiveFocus()
                    if (!osk.visible && !inputKeys.hasPhysicalKeyboard) {
                        osk.visible = true
                    }
                }
            }
        }

        // Physical keyboard input for the prompt line.
        Item {
            id: inputKeys
            readonly property bool hasPhysicalKeyboard: false
            focus: claudeVm.configured
            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    if (content.pendingInput !== "" && !claudeVm.busy) {
                        claudeVm.sendPrompt(content.pendingInput)
                        content.pendingInput = ""
                    }
                } else if (event.key === Qt.Key_Backspace) {
                    content.pendingInput = content.pendingInput.slice(0, -1)
                } else if (event.text.length > 0) {
                    content.pendingInput += event.text
                }
                event.accepted = true
            }
        }

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
            onKeyPressed: (text) => content.pendingInput += text
            onBackspacePressed: content.pendingInput = content.pendingInput.slice(0, -1)
            onEnterPressed: {
                if (content.pendingInput !== "" && !claudeVm.busy) {
                    claudeVm.sendPrompt(content.pendingInput)
                    content.pendingInput = ""
                }
            }
            onDismissed: visible = false
        }
    }
}
