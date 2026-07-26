import QtQuick

// Drawn checkmark — the device fonts lack the "✓" glyph (it renders as a
// box), so selection marks are drawn with rectangles instead.
Item {
    id: mark

    property int thickness: 6

    width: 34
    height: 34

    Rectangle {
        x: 2
        y: 18
        width: 14
        height: mark.thickness
        radius: mark.thickness / 2
        color: "black"
        rotation: 45
    }

    Rectangle {
        x: 8
        y: 14
        width: 26
        height: mark.thickness
        radius: mark.thickness / 2
        color: "black"
        rotation: -50
    }
}
