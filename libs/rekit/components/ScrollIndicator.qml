import QtQuick

// iOS-style scroll indicator for a Flickable: a thin rounded bar hugging
// the right edge, visible only while the content overflows. Place it as a
// sibling overlapping the Flickable and bind `target`.
Item {
    id: indicator

    property Flickable target: null

    readonly property bool scrollable:
        target !== null && target.contentHeight > target.height

    visible: scrollable
    width: 8

    Rectangle {
        width: parent.width
        radius: width / 2
        color: "black"
        opacity: 0.45

        height: indicator.scrollable
            ? Math.max(48, indicator.height * (indicator.target.height / indicator.target.contentHeight))
            : 0
        y: indicator.scrollable
            ? (indicator.height - height) *
              Math.min(1, Math.max(0, indicator.target.contentY /
                  Math.max(1, indicator.target.contentHeight - indicator.target.height)))
            : 0
    }
}
