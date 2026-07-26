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

        Text {
            id: screenTitle
            anchors.top: topBar.bottom
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.leftMargin: 60
            font.pixelSize: 56
            font.bold: true
            color: "black"
            text: content.showingLicense ? qsTr("License") : qsTr("Settings")
        }

        PushButton {
            visible: content.showingLicense
            anchors.verticalCenter: screenTitle.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 60
            width: 180
            height: 72
            label: qsTr("Back")
            onClicked: content.showingLicense = false
        }

        Flickable {
            visible: content.showingLicense
            anchors.top: screenTitle.bottom
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

        Flickable {
            visible: !content.showingLicense
            anchors.top: screenTitle.bottom
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

        HomeMarker {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
