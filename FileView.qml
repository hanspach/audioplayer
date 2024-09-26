import org.hsoft 1.0
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia


Page {
    required property MediaPlayer player
    property int index: filelistmodel.idx

    header: Row {
         height: 10
         width: parent.width
    }

    ListView {
        id: filelistview
        anchors.fill: parent
        model: filelistmodel

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
                    if(item.isDirectory)
                        filelistmodel.addFiles(item.url)
                    else
                        changeFile(item)
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            width: 20
            policy: ScrollBar.AlwaysOn
        }
        highlight: Rectangle { width: ListView.view.width - 20; height:20; color: "lightgrey"; radius: 5 }

        Component.onCompleted: {
            filelistmodel.findFiles(filelistmodel.standardPath())
            filelistview.currentIndex = -1
        }
    }

    footer: Rectangle {
        width: 500
        height: 50
        color: "#222222"
        RowLayout {
           anchors.fill: parent

            MsgToolButton {
                icon.source: "qrc:/icons/back"
                msg: qsTr("back to folder")
                enabled: filelistmodel.enableBackBtn
                onClicked: {
                    filelistmodel.findFiles()
                }
            }

            Text {
               Layout.minimumWidth: 50
               Layout.minimumHeight: 18
               horizontalAlignment: Text.AlignRight
               color: "white"
               text: {
                   var m = 0
                   var s = "" + 0

                   if(filelistview.currentIndex >= 0) {
                      m = Math.floor(player.position / 60000)
                      s = '' + Math.floor(player.position / 1000) % 60
                   }
                   return `${m}:${s.padStart(2, 0)}`
               }
           }

            BigKnobSlider {
               Layout.fillWidth: true
               enabled: player.seekable
               from: 0
               to: 1.0
               value: enabled ? player.position / player.duration : 0

               onMoved: player.setPosition(value * player.duration)
            }

            Text {
                Layout.minimumWidth: 50
                Layout.minimumHeight: 18
                Layout.rightMargin: 5
                color: "white"
                text: {
                    var m = Math.floor(player.duration / 60000)
                    var s = '' + Math.floor(player.duration / 1000) % 60
                    return `${m}:${s.padStart(2, 0)}`
                }
            }

            MsgToolButton {
                icon.source: "qrc:/icons/musicfolder"
                msg: qsTr("open folder")
                onClicked: {
                    folderDialog.open()
                }
            }
       }
    }

    FolderDialog {
        id: folderDialog
        visible: false
        onAccepted:  {
            filelistmodel.findFiles(folderDialog.selectedFolder)
        }
    }

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

