import QtQuick

// E-paper friendly button: outlined by default, solid black when primary.
Rectangle {
    id: button

    property string label: ""
    property bool primary: false

    signal clicked()

    // Instant pressed feedback (inverted) — essential on e-paper, where
    // waiting for the action's own refresh feels unresponsive.
    readonly property bool pressed: tapArea.pressed

    width: 220
    height: 80
    radius: 12
    color: (primary !== pressed) ? "black" : "white"
    border.color: "black"
    border.width: 3

    Text {
        anchors.centerIn: parent
        font.pixelSize: 30
        font.bold: true
        color: (button.primary !== button.pressed) ? "white" : "black"
        text: button.label
    }

    MouseArea {
        id: tapArea
        anchors.fill: parent
        onClicked: button.clicked()
    }
}
