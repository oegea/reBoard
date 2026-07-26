import QtQuick

// Simple modal alert: a message and a single acknowledge button.
Item {
    id: alertRoot

    property string message: ""
    property string acknowledgeLabel: qsTr("OK")

    signal acknowledged()

    visible: false

    function open(text) {
        message = text
        visible = true
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
        opacity: 0.75
    }
    MouseArea {
        anchors.fill: parent
    }

    Rectangle {
        width: Math.min(parent.width - 120, 760)
        height: layout.height + 80
        anchors.centerIn: parent
        color: "white"
        border.color: "black"
        border.width: 4
        radius: 16

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
                text: alertRoot.message
            }

            PushButton {
                anchors.horizontalCenter: parent.horizontalCenter
                label: alertRoot.acknowledgeLabel
                primary: true
                onClicked: {
                    alertRoot.visible = false
                    alertRoot.acknowledged()
                }
            }
        }
    }
}
