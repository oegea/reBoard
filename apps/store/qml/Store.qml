import QtQuick
import QtQuick.Window
import "qrc:/rekit/components"

Window {
    id: root
    visible: true
    width: 1404
    height: 1872
    color: "white"
    title: "App Store"

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

        // Header: large title for the catalog, nav-bar style in the detail.
        Item {
            id: header
            anchors.top: topBar.bottom
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.right: parent.right
            height: 80

            Text {
                visible: !storeVm.detailVisible
                anchors.left: parent.left
                anchors.leftMargin: 60
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 56
                font.bold: true
                color: "black"
                text: qsTr("App Store")
            }

            BackButton {
                visible: storeVm.detailVisible
                anchors.left: parent.left
                anchors.leftMargin: 48
                anchors.verticalCenter: parent.verticalCenter
                onClicked: storeVm.closeDetail()
            }

            Text {
                visible: storeVm.detailVisible
                anchors.centerIn: parent
                width: parent.width / 2
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
                font.pixelSize: 44
                font.bold: true
                color: "black"
                text: storeVm.detail.name !== undefined ? storeVm.detail.name : ""
            }
        }

        // ------------------------- Catalog page -------------------------

        Text {
            visible: !storeVm.detailVisible && storeVm.loading
            anchors.centerIn: parent
            font.pixelSize: 34
            color: "black"
            text: qsTr("Loading…")
        }

        Column {
            visible: !storeVm.detailVisible && !storeVm.loading && storeVm.errorMessage !== ""
            anchors.centerIn: parent
            spacing: 40
            width: parent.width - 240

            Text {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                font.pixelSize: 32
                color: "black"
                text: qsTr("The catalog could not be loaded.") + "\n" + storeVm.errorMessage
            }
            PushButton {
                anchors.horizontalCenter: parent.horizontalCenter
                label: qsTr("Retry")
                primary: true
                onClicked: storeVm.reload()
            }
        }

        Text {
            visible: !storeVm.detailVisible && !storeVm.loading &&
                     storeVm.errorMessage === "" && storeVm.sections.length === 0
            anchors.centerIn: parent
            font.pixelSize: 32
            color: "black"
            opacity: 0.6
            text: qsTr("No applications in the catalog yet.")
        }

        Flickable {
            id: catalogFlick
            visible: !storeVm.detailVisible && !storeVm.loading && storeVm.errorMessage === ""
            anchors.top: header.bottom
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 60
            anchors.rightMargin: 60
            contentHeight: catalogColumn.height + 80
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Column {
                id: catalogColumn
                width: parent.width
                spacing: 48

                Repeater {
                    model: storeVm.sections
                    ListGroup {
                        title: modelData.category
                        width: catalogColumn.width

                        Repeater {
                            model: modelData.apps
                            ListRow {
                                label: modelData.name
                                sublabel: modelData.summary
                                showDivider: index < parent.count - 1
                                onClicked: storeVm.openApp(modelData.appId)
                            }
                        }
                    }
                }
            }
        }

        ScrollIndicator {
            target: catalogFlick
            visible: catalogFlick.visible && catalogFlick.contentHeight > catalogFlick.height
            anchors.top: catalogFlick.top
            anchors.bottom: catalogFlick.bottom
            anchors.right: parent.right
            anchors.rightMargin: 20
        }

        // ------------------------- Detail page --------------------------

        Flickable {
            id: detailFlick
            visible: storeVm.detailVisible
            anchors.top: header.bottom
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 60
            anchors.rightMargin: 60
            contentHeight: detailColumn.height + 80
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Column {
                id: detailColumn
                width: parent.width
                spacing: 36

                Text {
                    visible: storeVm.detail.loading === true
                    font.pixelSize: 32
                    color: "black"
                    text: qsTr("Loading…")
                }

                Text {
                    visible: storeVm.detail.category !== undefined &&
                             storeVm.detail.loading !== true
                    font.pixelSize: 28
                    color: "black"
                    opacity: 0.6
                    text: (storeVm.detail.category !== undefined ? storeVm.detail.category : "")
                          + (storeVm.detail.version !== undefined && storeVm.detail.version !== ""
                             ? "  ·  v" + storeVm.detail.version : "")
                }

                Text {
                    visible: storeVm.detail.description !== undefined &&
                             storeVm.detail.loading !== true
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: 30
                    color: "black"
                    text: storeVm.detail.description !== undefined ? storeVm.detail.description : ""
                }

                Text {
                    visible: storeVm.detail.error !== undefined && storeVm.detail.error !== ""
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.pixelSize: 28
                    color: "black"
                    text: storeVm.detail.error !== undefined ? storeVm.detail.error : ""
                }

                Item {
                    width: parent.width
                    height: 100
                    visible: storeVm.detail.loading !== true

                    PushButton {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 320
                        height: 90
                        visible: storeVm.detail.available === true &&
                                 storeVm.detail.installed !== true
                        label: storeVm.busy ? qsTr("Installing…") : qsTr("Install")
                        primary: !storeVm.busy
                        onClicked: {
                            if (!storeVm.busy) {
                                storeVm.install()
                            }
                        }
                    }

                    PushButton {
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 320
                        height: 90
                        visible: storeVm.detail.installed === true
                        label: qsTr("Remove")
                        onClicked: storeVm.removeInstalled()
                    }

                    Text {
                        anchors.centerIn: parent
                        visible: storeVm.detail.available === false &&
                                 storeVm.detail.installed !== true
                        font.pixelSize: 30
                        color: "black"
                        opacity: 0.6
                        text: qsTr("Not available for this device")
                    }
                }
            }
        }

        ScrollIndicator {
            target: detailFlick
            visible: detailFlick.visible && detailFlick.contentHeight > detailFlick.height
            anchors.top: detailFlick.top
            anchors.bottom: detailFlick.bottom
            anchors.right: parent.right
            anchors.rightMargin: 20
        }

        HomeMarker {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
