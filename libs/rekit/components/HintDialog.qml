import QtQuick

// Modal alert shown before launching an application: reminds the user how
// to come back to the launcher. All literals are translatable (story 004).
Item {
    id: dialogRoot

    property var pendingApp: null

    signal confirmed(var app)

    visible: false

    function openFor(app) {
        pendingApp = app
        visible = true
    }

    function close() {
        visible = false
        pendingApp = null
    }

    // Scrim: dims the board and dismisses on tap outside the panel.
    Rectangle {
        anchors.fill: parent
        color: "white"
        opacity: 0.75
    }
    MouseArea {
        anchors.fill: parent
        onClicked: dialogRoot.close()
    }

    Rectangle {
        id: panel
        width: Math.min(parent.width - 120, 760)
        height: layout.height + 80
        anchors.centerIn: parent
        color: "white"
        border.color: "black"
        border.width: 4
        radius: 16

        // Swallow taps so they do not reach the scrim.
        MouseArea {
            anchors.fill: parent
        }

        Column {
            id: layout
            width: parent.width - 80
            anchors.centerIn: parent
            spacing: 40

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                font.pixelSize: 32
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                text: qsTr("To come back to reBoard at any time, press and hold the bottom edge of the screen for a couple of seconds, or swipe up from it.")
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 40

                PushButton {
                    label: qsTr("Cancel")
                    onClicked: dialogRoot.close()
                }

                PushButton {
                    label: qsTr("Open")
                    primary: true
                    onClicked: {
                        var app = dialogRoot.pendingApp
                        dialogRoot.close()
                        dialogRoot.confirmed(app)
                    }
                }
            }
        }
    }
}
