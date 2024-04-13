import QtQuick
import QtQuick.Controls

ToolButton {
    property string msg: ""
    property bool allowspin: false
    property bool spin: false

    id: control
    ToolTip.text: msg
    ToolTip.visible: msg ? ma.containsMouse : false
    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        onClicked:  {
            if(allowspin) {
                control.spin = true
                spintimer.start()
            }
            control.clicked()
        }
    }

    RotationAnimation on rotation  {
        running: control.spin
        loops: Animation.Infinite
        from: 0
        to: 360
        duration: 300
    }

    Timer {
        id: spintimer
        interval: 900
        running: false
        repeat: true
        onTriggered: {
            control.spin = false
            spintimer.stop()
        }
    }
}


