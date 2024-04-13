import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 1

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            id: listview
            currentIndex: -1
            model: fruitModel

            delegate:  Item  {
               id: delegate
                width: ListView.view.width
                height: 20

                Image {
                    id: imgid
                    source: img
                    height: 20; width: 20
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    anchors.left: imgid.right
                    anchors.leftMargin: 5
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    text: name
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: { listview.currentIndex = index}
                }

            }
            highlight: Rectangle { width: 500; height:20; color: "lightblue"; radius: 5 }

        }

    }

    ListModel {
        id: fruitModel
        property string language: "en"
        ListElement {
            img: "qrc:/icons/audio"
            name: "Apple"

        }
        ListElement {
            img: "qrc:/icons/audio"
            name: "Orange"

        }
        ListElement {
            img: "qrc:/icons/audio"
            name: "Banana"

        }
    }

}

