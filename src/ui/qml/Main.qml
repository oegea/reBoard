import QtQuick
import QtQuick.Window
import "components"

Window {
    id: root
    visible: true
    width: 1404
    height: 1872
    color: "white"
    title: "reBoard"

    // All UI lives inside `content`, which rotates as a whole so the board,
    // the top bar and the home marker follow the current orientation.
    Item {
        id: content
        readonly property bool rotated: uiRotation === 90 || uiRotation === 270
        width: rotated ? root.height : root.width
        height: rotated ? root.width : root.height
        anchors.centerIn: parent
        rotation: uiRotation

        TopBar {
            id: topBar
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
        }

        // Paginated application grid, springboard style.
        ListView {
            id: pager
            anchors.top: topBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: pageDots.top
            anchors.topMargin: 40
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
                    cellWidth: width / (content.rotated ? 6 : 4)
                    cellHeight: 280
                    model: modelData

                    delegate: Item {
                        width: GridView.view.cellWidth
                        height: GridView.view.cellHeight
                        AppIcon {
                            anchors.horizontalCenter: parent.horizontalCenter
                            appData: modelData
                            onActivated: (app) => hintDialog.openFor(app)
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

        // Fixed dock at the visual bottom of the screen.
        Rectangle {
            id: dockBar
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 290
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
                        onActivated: (app) => hintDialog.openFor(app)
                    }
                }
            }
        }

        HomeMarker {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Pre-launch hint: how to come back to the launcher (story 004).
        HintDialog {
            id: hintDialog
            anchors.fill: parent
            onConfirmed: (app) => launcher.launch(app.appId)
        }
    }
}
