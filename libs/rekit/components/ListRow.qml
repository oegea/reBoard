import QtQuick

// One touch-friendly row inside a ListGroup: label on the left, optional
// value or checkmark on the right, hairline separator below.
Item {
    id: row

    property string label: ""
    // Optional second line under the label (smaller, dimmed).
    property string sublabel: ""
    property string value: ""
    property bool checked: false
    property bool showDivider: true

    signal clicked()

    width: parent ? parent.width : 0
    height: sublabel !== "" ? 118 : 88

    // Instant pressed feedback.
    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: rowTapArea.pressed ? 0.12 : 0
    }

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 28
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: rightSide.left
        anchors.rightMargin: 16
        spacing: 6

        Text {
            width: parent.width
            font.pixelSize: 30
            color: "black"
            elide: Text.ElideRight
            text: row.label
        }

        Text {
            visible: row.sublabel !== ""
            width: parent.width
            font.pixelSize: 24
            color: "black"
            opacity: 0.55
            elide: Text.ElideRight
            text: row.sublabel
        }
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

        CheckMark {
            visible: row.checked
            anchors.verticalCenter: parent.verticalCenter
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
        id: rowTapArea
        anchors.fill: parent
        onClicked: row.clicked()
    }
}
