import QtQuick

// E-paper friendly button: outlined by default, solid black when primary.
Rectangle {
    id: button

    property string label: ""
    property bool primary: false

    signal clicked()

    width: 220
    height: 80
    radius: 12
    color: primary ? "black" : "white"
    border.color: "black"
    border.width: 3

    Text {
        anchors.centerIn: parent
        font.pixelSize: 30
        font.bold: true
        color: button.primary ? "white" : "black"
        text: button.label
    }

    MouseArea {
        anchors.fill: parent
        onClicked: button.clicked()
    }
}
