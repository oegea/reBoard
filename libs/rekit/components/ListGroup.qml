import QtQuick

// iOS-style grouped list section: optional uppercase title above a rounded
// container of ListRow items.
Item {
    id: group

    property string title: ""
    default property alias rows: rowsColumn.data

    implicitHeight: titleText.height + box.height + (title !== "" ? 12 : 0)
    height: implicitHeight

    Text {
        id: titleText
        text: group.title.toUpperCase()
        visible: group.title !== ""
        height: group.title !== "" ? 40 : 0
        leftPadding: 28
        font.pixelSize: 24
        font.bold: true
        color: "black"
        verticalAlignment: Text.AlignBottom
    }

    Rectangle {
        id: box
        anchors.top: titleText.bottom
        anchors.topMargin: group.title !== "" ? 12 : 0
        width: parent.width
        height: rowsColumn.height
        radius: 16
        color: "white"
        border.color: "black"
        border.width: 3
        clip: true

        Column {
            id: rowsColumn
            width: parent.width
        }
    }
}
