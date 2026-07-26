import QtQuick
import QtQuick.Window
import "qrc:/rekit/components"

Window {
    id: root
    visible: true
    width: 1404
    height: 1872
    color: "white"
    title: "Settings"

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

        // Simple two-page navigation: the main list and the license viewer.
        property bool showingLicense: false

        // Header: iOS large title on the main page; nav-bar style (back at
        // the left, centered title) inside the license page.
        Item {
            id: header
            anchors.top: topBar.bottom
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.right: parent.right
            height: 80

            Text {
                visible: !content.showingLicense
                anchors.left: parent.left
                anchors.leftMargin: 60
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 56
                font.bold: true
                color: "black"
                text: qsTr("Settings")
            }

            BackButton {
                visible: content.showingLicense
                anchors.left: parent.left
                anchors.leftMargin: 48
                anchors.verticalCenter: parent.verticalCenter
                onClicked: content.showingLicense = false
            }

            Text {
                visible: content.showingLicense
                anchors.centerIn: parent
                font.pixelSize: 44
                font.bold: true
                color: "black"
                text: qsTr("License")
            }

            // Hairline that anchors the header once content scrolls under
            // it (iOS convention); invisible at the top of the page.
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: 2
                color: "black"
                opacity: (content.showingLicense ? licenseFlick.contentY
                                                 : mainFlick.contentY) > 4 ? 0.5 : 0
            }
        }

        Flickable {
            id: licenseFlick
            visible: content.showingLicense
            anchors.top: header.bottom
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 60
            anchors.rightMargin: 60
            contentHeight: licenseBody.height + 80
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Text {
                id: licenseBody
                width: parent.width
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pixelSize: 24
                font.family: "monospace"
                color: "black"
                text: settingsVm.licenseText
            }
        }

        ScrollIndicator {
            target: licenseFlick
            visible: licenseFlick.visible &&
                     licenseFlick.contentHeight > licenseFlick.height
            anchors.top: licenseFlick.top
            anchors.bottom: licenseFlick.bottom
            anchors.right: parent.right
            anchors.rightMargin: 20
        }

        Flickable {
            id: mainFlick
            visible: !content.showingLicense
            anchors.top: header.bottom
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 60
            anchors.rightMargin: 60
            contentHeight: groups.height + 80
            clip: true
            boundsBehavior: Flickable.StopAtBounds

            Column {
                id: groups
                width: parent.width
                spacing: 48

                ListGroup {
                    title: qsTr("About")
                    width: parent.width
                    ListRow { label: qsTr("Version"); value: settingsVm.version }
                    ListRow { label: qsTr("Build"); value: settingsVm.build }
                    ListRow {
                        label: qsTr("License")
                        value: settingsVm.license
                        showDivider: false
                        onClicked: content.showingLicense = true
                    }
                }

                ListGroup {
                    title: qsTr("Storage")
                    width: parent.width
                    ListRow { label: qsTr("Available"); value: settingsVm.storageFree }
                    ListRow { label: qsTr("Total"); value: settingsVm.storageTotal; showDivider: false }
                }

                ListGroup {
                    title: qsTr("Language")
                    width: parent.width
                    ListRow {
                        label: qsTr("System language")
                        checked: settingsVm.language === "system"
                        onClicked: settingsVm.language = "system"
                    }
                    ListRow {
                        label: "English"
                        checked: settingsVm.language === "en"
                        onClicked: settingsVm.language = "en"
                    }
                    ListRow {
                        label: "Español"
                        checked: settingsVm.language === "es"
                        showDivider: false
                        onClicked: settingsVm.language = "es"
                    }
                }

                ListGroup {
                    title: qsTr("Applications")
                    width: parent.width
                    Repeater {
                        model: settingsVm.applications
                        ListRow {
                            label: modelData.name
                            value: modelData.appId
                            showDivider: index < settingsVm.applications.length - 1
                        }
                    }
                }
            }
        }

        ScrollIndicator {
            target: mainFlick
            visible: mainFlick.visible && mainFlick.contentHeight > mainFlick.height
            anchors.top: mainFlick.top
            anchors.bottom: mainFlick.bottom
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
