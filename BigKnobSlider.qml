import QtQuick
import QtQuick.Controls

Slider {
    orientation: Qt.Horizontal
    enabled: true
    to: 100.0
    id: control

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 32
        implicitHeight: 32
        radius: 13
        color: control.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "#bdbebf"
    }
}
