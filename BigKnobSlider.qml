import QtQuick
import QtQuick.Controls

Slider {
    orientation: Qt.Horizontal
    enabled: true
    from: 0
    to: 110.0
    id: control

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 16
        implicitHeight: 16
        radius: 5
        color: control.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: "#bdbebf"
    }
}
