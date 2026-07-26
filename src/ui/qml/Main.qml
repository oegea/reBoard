import QtQuick
import QtQuick.Window

Window {
    id: root
    visible: true
    width: 1404
    height: 1872
    color: "white"
    title: "reBoard"

    Connections {
        target: launcher
        function onRequestShowLauncher() {
            board.reload()
            root.visible = true
            root.requestActivate()
        }
        function onRequestHideLauncher() {
            root.visible = false
        }
    }

    // One selectable application icon, shared by the grid and the dock.
    component AppIcon: Item {
        id: iconRoot
        required property var appData
        width: 200
        height: 240

        Rectangle {
            id: iconBox
            width: 140
            height: 140
            radius: 28
            anchors.horizontalCenter: parent.horizontalCenter
            color: "white"
            border.color: "black"
            border.width: 3

            Image {
                anchors.fill: parent
                anchors.margins: 8
                source: iconRoot.appData.icon !== "" ? "file://" + iconRoot.appData.icon : ""
                visible: iconRoot.appData.icon !== ""
                fillMode: Image.PreserveAspectFit
            }

            Text {
                anchors.centerIn: parent
                visible: iconRoot.appData.icon === ""
                text: iconRoot.appData.initial
                font.pixelSize: 64
                font.bold: true
                color: "black"
            }
        }

        Text {
            anchors.top: iconBox.bottom
            anchors.topMargin: 12
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width - 12
            text: iconRoot.appData.name
            font.pixelSize: 28
            color: "black"
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
        }

        MouseArea {
            anchors.fill: parent
            onClicked: launcher.launch(iconRoot.appData.appId)
        }
    }

    // Paginated application grid, springboard style.
    ListView {
        id: pager
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: pageDots.top
        anchors.topMargin: 60
        anchors.leftMargin: 40
        anchors.rightMargin: 40
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        model: board.pages

        delegate: Item {
            width: pager.width
            height: pager.height

            GridView {
                anchors.fill: parent
                interactive: false
                cellWidth: width / 4
                cellHeight: 280
                model: modelData

                delegate: Item {
                    width: GridView.view.cellWidth
                    height: GridView.view.cellHeight
                    AppIcon {
                        anchors.horizontalCenter: parent.horizontalCenter
                        appData: modelData
                    }
                }
            }
        }
    }

    // Page indicator dots.
    Row {
        id: pageDots
        anchors.bottom: dockBar.top
        anchors.bottomMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 20
        visible: board.pages.length > 1

        Repeater {
            model: board.pages.length
            Rectangle {
                width: 16
                height: 16
                radius: 8
                color: index === pager.currentIndex ? "black" : "white"
                border.color: "black"
                border.width: 2
            }
        }
    }

    // Fixed dock at the bottom of the screen.
    Rectangle {
        id: dockBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 300
        color: "white"

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 3
            color: "black"
        }

        Row {
            anchors.centerIn: parent
            spacing: 80

            Repeater {
                model: board.dock
                AppIcon {
                    appData: modelData
                }
            }
        }
    }
}
