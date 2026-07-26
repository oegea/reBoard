import QtQuick

// Rounded-square application icon with its label, springboard style.
// Presentational only: emits `activated` and lets the screen decide.
// Note: appData must NOT be a required property — delegates with required
// properties opt out of the legacy context injection, so `modelData` would
// no longer resolve when AppIcon is used directly as a delegate.
Item {
    id: iconRoot

    property var appData: null
    readonly property bool hasIcon: appData !== null && appData.icon !== ""

    signal activated(var app)

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
            anchors.margins: 16
            source: !iconRoot.hasIcon ? ""
                  : iconRoot.appData.icon.startsWith("qrc:") ? iconRoot.appData.icon
                  : "file://" + iconRoot.appData.icon
            visible: iconRoot.hasIcon
            fillMode: Image.PreserveAspectFit
            smooth: true
        }

        Text {
            anchors.centerIn: parent
            visible: !iconRoot.hasIcon
            text: iconRoot.appData !== null ? iconRoot.appData.initial : ""
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
        text: iconRoot.appData !== null ? iconRoot.appData.name : ""
        font.pixelSize: 28
        color: "black"
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (iconRoot.appData !== null) {
                iconRoot.activated(iconRoot.appData)
            }
        }
    }
}
