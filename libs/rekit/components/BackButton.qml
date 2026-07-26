import QtQuick

// iOS-style back control: drawn left chevron + label, borderless, with a
// generous touch target. Place at the top-left of a page.
Item {
    id: back

    property string label: qsTr("Back")

    signal clicked()

    width: chevron.width + labelText.width + 60
    height: 72

    Item {
        id: chevron
        width: 26
        height: 44
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            x: 2
            y: 8
            width: 30
            height: 9
            radius: 4
            color: "black"
            rotation: -45
        }
        Rectangle {
            x: 2
            y: 26
            width: 30
            height: 9
            radius: 4
            color: "black"
            rotation: 45
        }
    }

    Text {
        id: labelText
        anchors.left: chevron.right
        anchors.leftMargin: 14
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 34
        color: "black"
        text: back.label
    }

    MouseArea {
        anchors.fill: parent
        anchors.margins: -12
        onClicked: back.clicked()
    }
}
