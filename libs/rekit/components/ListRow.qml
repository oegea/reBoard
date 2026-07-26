import QtQuick

// One touch-friendly row inside a ListGroup: label on the left, optional
// value or checkmark on the right, hairline separator below.
Item {
    id: row

    property string label: ""
    property string value: ""
    property bool checked: false
    property bool showDivider: true

    signal clicked()

    width: parent ? parent.width : 0
    height: 88

    Text {
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: rightSide.left
        anchors.rightMargin: 16
        font.pixelSize: 30
        color: "black"
        elide: Text.ElideRight
        text: row.label
    }

    Row {
        id: rightSide
        anchors.right: parent.right
        anchors.rightMargin: 28
        anchors.verticalCenter: parent.verticalCenter
        spacing: 12

        Text {
            visible: row.value !== ""
            font.pixelSize: 28
            color: "black"
            opacity: 0.6
            text: row.value
        }

        Text {
            visible: row.checked
            font.pixelSize: 30
            font.bold: true
            color: "black"
            text: "✓"
        }
    }

    Rectangle {
        visible: row.showDivider
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.right: parent.right
        height: 2
        color: "black"
        opacity: 0.25
    }

    MouseArea {
        anchors.fill: parent
        onClicked: row.clicked()
    }
}
