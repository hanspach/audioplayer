import QtQuick
import QtQuick.Controls
import QtMultimedia
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    property bool toolbarvisible: true

    id: window
    width: 480
    height: 500
    visible: true
    title: qsTr("SAP - Simple Audio Player")
    onClosing: { initValuesModel.writeJsonFile() }

    header: Rectangle {
        id: headerbar
        color: "lightgrey"
        height: 50
        width: window.width

        Row {
            id: buttonsrow
            spacing: 10

            MsgToolButton {
                icon.source: "qrc:/icons/online"
                msg: qsTr("online radio")
                onClicked: swipeview.currentIndex = 0
            }
            MsgToolButton {
                icon.source: "qrc:/icons/musicfolder"
                msg: qsTr("audio files")
                onClicked: swipeview.currentIndex = 1
            }
            MsgToolButton {
                icon.source: "qrc:/icons/settings"
                msg: qsTr("settings")
                onClicked: swipeview.currentIndex = 2
            }
            MsgToolButton {
                id: playbtn
                icon.source: initValuesModel.playbtnIconsrc
                msg: "play | pause"
                onClicked: {
                    if(player.playbackState === MediaPlayer.PlayingState) {
                        player.pause()
                        initValuesModel.changePlayIcon("qrc:/icons/play")
                        initValuesModel.startStopTimer(0)
                    }
                    else if(player.playbackState === MediaPlayer.PausedState) {
                        player.play()
                        initValuesModel.changePlayIcon("qrc:/icons/pause")
                        initValuesModel.startStopTimer(1);
                    }
                }
            }
            MsgToolButton {
                allowspin: true
                id: addbtn
                icon.source: "qrc:/icons/change"
                msg: qsTr("add to favorites")
                enabled: false
                onClicked: {
                    if(radiopage.addFavItem()) {
                        addbtn.spin = true
                    } else {
                        initValuesModel.changeMessage(qsTr("entry exists already"), 2500)
                    }
                    addbtn.enabled = false
                }
            }
        }

        BigKnobSlider {
            id: volumeSlider
            value: initValuesModel.vlm
            anchors.left: buttonsrow.right
            anchors.right: headerbar.right
            anchors.leftMargin: 5
            anchors.rightMargin: 5
        }

    }

    footer: Rectangle {
        id: toolbar
        color: "lightgrey"
        height: 35
        width: window.width
        visible: toolbarvisible

        Label {
            id: statuslbl
            anchors.right: toolbar.right
            anchors.rightMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            width: 24; height: 24
            background: Rectangle {
                id: statusrect
                width: 24; height: 24
                color: initValuesModel.statusrectcolor
                radius: 4
            }
        }

        Control {
            id: durationlbl
            anchors.right: statuslbl.left
            rightPadding: 5
            topPadding: 8
            contentItem: Text {
                verticalAlignment: Text.AlignVCenter
                text: initValuesModel.durationtxt
            }
        }

        Control {
            id: msglbl
            anchors.left: toolbar.left
            anchors.right: durationlbl.left
            leftPadding: 5
            rightPadding: 5
            topPadding: 8
            contentItem: Text {
                id: msgtxt
                verticalAlignment: Text.AlignVCenter
                text: initValuesModel.msg
            }
            onWidthChanged: {initValuesModel.setMsgBarWidth(width)}
        }
    }

    SwipeView {
        id: swipeview
        anchors.fill: parent
        currentIndex: 0

        RadioView {id: radiopage; player: player}
        FileView  {id: filepage;  player: player}
        SettingsView {}

        onCurrentIndexChanged: {
            if(swipeview.currentIndex === 1)
                window.toolbarvisible = false
            else
                window.toolbarvisible = true
        }
    }

    MediaPlayer {
        objectName: "mediaplayer"
        id: player
        audioOutput: AudioOutput {
            id: audio
            volume: volumeSlider.value/100.0
        }

        onMetaDataChanged: {
            if(player.error === MediaPlayer.NoError) {
                initValuesModel.changeStatusRectColor('green')
            }
        }

        onErrorOccurred: {
            initValuesModel.changeMessage(player.errorString, 5000,"red");
            initValuesModel.startStopTimer(0)
        }

        onMediaStatusChanged: {
            if(mediaStatus === MediaPlayer.EndOfMedia) {
                if(swipeview.currentIndex === 1)
                    initValuesModel.nextEntry()
                else if(swipeview.currentIndex === 0)
                    initValuesModel.locationRequest(initValuesModel.uri);
            }
        }
    }

    function enableAddButton() {
        addbtn.enabled = true
    }
}


