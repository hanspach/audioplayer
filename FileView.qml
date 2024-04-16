import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import org.hsoft 1.0


Page {
    required property MediaPlayer player
    property int index: filelistmodel.idx

    ListView {
        id: filelistview
        anchors.fill: parent
        model: filelistmodel
        currentIndex: -1

        delegate: Item {
            width: ListView.view.width - 20
            height: 36

            Row {
                Image {
                    height: 32; width: 32
                    source: img
                }

                Control {
                    topPadding: 8
                    contentItem: Text {
                        Layout.topMargin: 5
                        color: "white"
                        text: name
                    }
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    filelistmodel.setIndex(index)
                    filelistview.currentIndex = index

                    var item = filelistmodel.item(index)
                    changeFile(item)
               }
           }
        }

        section.property: "folder"
        section.delegate: Rectangle {
            width: ListView.view.width - 20
            height: 20
            color: "lightsteelblue"
            RowLayout {
                anchors.fill: parent
                Layout.margins: 5
                Text {
                    text: section
                    font.bold: true
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            width: 20
            policy: ScrollBar.AlwaysOn
        }
        highlight: Rectangle { width: ListView.view.width - 20; height:20; color: "lightgrey"; radius: 5 }

        Component.onCompleted: {filelistmodel.findFiles()}
    }

    footer: Rectangle {
        width: 500
        height: 50
        color: "lightgrey"
        RowLayout {
           anchors.fill: parent

           Text {
               Layout.minimumWidth: 50
               Layout.minimumHeight: 18
               horizontalAlignment: Text.AlignRight
               text: {
                   var m = Math.floor(player.position / 60000)
                   var s = '' + Math.floor(player.position / 1000) % 60
                   return `${m}:${s.padStart(2, 0)}`
               }
           }

           Slider {
               Layout.fillWidth: true
               enabled: player.seekable
               to: 1.0
               value: player.position / player.duration

               onMoved: player.setPosition(value * player.duration)
            }

            Text {
                Layout.minimumWidth: 50
                Layout.minimumHeight: 18
                Layout.rightMargin: 5
                text: {
                    var m = Math.floor(player.duration / 60000)
                    var s = '' + Math.floor(player.duration / 1000) % 60
                    return `${m}:${s.padStart(2, 0)}`
                }
            }

            MsgToolButton {
                icon.source: "qrc:/icons/musicfolder"
                msg: qsTr("open folder")
                onClicked: {}
            }
       }
    }

    Component.onCompleted: {window.toolbarvisible = false }

    FileListModel {
       id: filelistmodel
    }

    onIndexChanged: {
        if(filepage.index !== -1) {
            filelistview.currentIndex = filepage.index
            var item = filelistmodel.item(index)
            changeFile(item)
        }
    }

    function changeFile(item) {
        player.stop()
        player.source = item.url
        player.play()
    }
}

