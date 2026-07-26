import QtQuick

// Generic confirmation modal: message + cancel/confirm actions. The
// confirm button is visually primary; pass `payload` through openFor and
// receive it back in `confirmed`.
Item {
    id: dialogRoot

    property string message: ""
    property string cancelLabel: qsTr("Cancel")
    property string confirmLabel: qsTr("Confirm")
    property var payload: null

    signal confirmed(var payload)

    visible: false

    function openFor(value) {
        payload = value
        visible = true
    }

    function close() {
        visible = false
        payload = null
    }

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
        width: Math.min(parent.width - 120, 760)
        height: layout.height + 80
        anchors.centerIn: parent
        color: "white"
        border.color: "black"
        border.width: 4
        radius: 16

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
                text: dialogRoot.message
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 40

                PushButton {
                    label: dialogRoot.cancelLabel
                    onClicked: dialogRoot.close()
                }

                PushButton {
                    label: dialogRoot.confirmLabel
                    primary: true
                    onClicked: {
                        var value = dialogRoot.payload
                        dialogRoot.close()
                        dialogRoot.confirmed(value)
                    }
                }
            }
        }
    }
}
