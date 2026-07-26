import QtQuick
import "qrc:/rekit/components"

// Early-iOS-inspired lock screen (story 007, phase 1): big clock, date and
// slide to unlock. Shown by the board when the power button is pressed.
Rectangle {
    id: lock

    signal unlocked()

    color: "white"

    function refreshClock() {
        clockText.text = Qt.formatTime(new Date(), "hh:mm")
        dateText.text = Qt.formatDate(new Date(), "dddd, d MMMM")
    }

    onVisibleChanged: {
        if (visible) {
            refreshClock()
            slider.reset()
        }
    }

    // Swallow every touch not aimed at the slider.
    MouseArea {
        anchors.fill: parent
    }

    Rectangle {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 3
        color: "black"
    }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.16
        spacing: 8

        Text {
            id: clockText
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 190
            font.weight: Font.Light
            color: "black"
            text: Qt.formatTime(new Date(), "hh:mm")
        }

        Text {
            id: dateText
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 40
            color: "black"
            text: Qt.formatDate(new Date(), "dddd, d MMMM")
        }
    }

    Timer {
        interval: 30000
        running: lock.visible
        repeat: true
        onTriggered: lock.refreshClock()
    }

    SlideToUnlock {
        id: slider
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 140
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width - 240
        onUnlocked: lock.unlocked()
    }
}
