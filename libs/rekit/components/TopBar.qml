import QtQuick

// Thin iOS-inspired status bar. E-paper friendly: the clock refreshes at
// most twice per minute.
Rectangle {
    id: topBar

    height: 56
    color: "white"

    Text {
        id: clockText
        anchors.centerIn: parent
        font.pixelSize: 30
        font.bold: true
        color: "black"
        text: Qt.formatTime(new Date(), "hh:mm")
    }

    Timer {
        interval: 30000
        running: true
        repeat: true
        onTriggered: clockText.text = Qt.formatTime(new Date(), "hh:mm")
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 2
        color: "black"
    }
}
