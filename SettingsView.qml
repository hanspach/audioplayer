import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    property string favoritestring: initValuesModel.favoritestring
    property var deletedFavorites: []
    property bool again: false
    id: setpage

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 5

            Text {
                id: countrytxt
                color: "lightblue"
                text: qsTr("choose your country")
            }

            ComboBox {
                property var countries: []
                id: choosecb
                Layout.fillWidth: true

                onActivated: {
                    if(setpage.again && choosecb.currentText !== initValuesModel.defaultcountry)
                        changebtn.enabled = true
                    setpage.again = true
                }
                Component.onCompleted: {
                    chooseworker.sendMessage({'countries': choosecb.countries,'text': "", 'ok': false, 'minItems': 100})
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 5

            Text {
                color: "lightblue"
                text: qsTr("volume")
            }

            BigKnobSlider {
                Layout.fillWidth: true
                id: setslider
                value: initValuesModel.vlm
                onValueChanged: {
                    if(setpage.again && initValuesModel.vlm !== value)
                        changebtn.enabled = true
                    setpage.again = true
                }
            }

            Text {
                Layout.rightMargin: 17
                color: "lightblue"
                text: "" + parseInt(setslider.value)
            }
        }

        RowLayout  {
            spacing: 5

            ListView {
                flickableDirection: Flickable.VerticalFlick
                boundsBehavior: Flickable.StopAtBounds
                Layout.fillHeight: true
                clip: true
                id: setfavlistview
                currentIndex: -1
                model: setfavmodel

                delegate: Item  {
                    width: ListView.view.width
                    height: 36
                    anchors.topMargin: 2
                    anchors.bottomMargin: 5

                    Image {
                        id: setfavimg
                        source: favicon
                        height: 32; width: 32
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        anchors.left: setfavimg.right
                        anchors.leftMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        color: "white"
                        text: name
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            setfavlistview.currentIndex = index
                            trashBtnCtrl(index)
                        }
                    }
                }
                Layout.fillWidth: true
                ScrollBar.vertical: setsb
                highlight: Rectangle { width: 500; height:20; color: "lightblue"; radius: 5 }
            }


            ListView {
                id: trashlv
                flickableDirection: Flickable.VerticalFlick
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                width: 50
                Layout.fillHeight: true
                model: trashmodel

                delegate: Item  {
                    width: ListView.view.width
                    height: 36
                    anchors.topMargin: 2
                    anchors.bottomMargin: 5

                    MsgToolButton {
                        id: trashbtn
                        height: 32; width: 32
                        anchors.leftMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        icon.source: img
                        msg: qsTr("delete entry")
                        enabled: active
                        onClicked: {
                            var item = setfavmodel.get(index)
                            setpage.deletedFavorites.push({'favicon': item.favicon,'name': item.name, 'url': item.url})
                            setfavmodel.remove(index,1)
                            trashmodel.remove(index,1)
                            if(setpage.again)
                                changebtn.enabled = true
                            setpage.again = true
                        }
                    }
                }
                ScrollBar.vertical: setsb
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.margins: 5

            Text {
                Layout.fillWidth: true
                color: "lightblue"
                text: qsTr("apply changes?")
            }

            MsgToolButton {
                allowspin: true
                id: changebtn
                height: 32; width: 32
                Layout.alignment: Qt.AlignRight
                icon.source: "qrc:/icons/change"
                enabled: false
                onClicked: {
                    doChanges()
                }
            }
        }
    }

    ListModel {
        id: setfavmodel
    }

    ListModel {
        id: trashmodel
    }

    ScrollBar {
        id: setsb
    }

    onFavoritestringChanged: {
        if(initValuesModel.favoritestring !== "") {
            setfavmodel.clear()
            trashmodel.clear()
            const ja = JSON.parse(initValuesModel.favoritestring)
            ja.forEach((obj) => {
               if(obj.favicon === "")
                 obj.favicon = "qrc:/icons/default"
               setfavmodel.append(obj)
               trashmodel.append({img: "qrc:/icons/trash", active: false})
            })
        }
    }

    WorkerScript {
        id: chooseworker
        source: "qrc:/scripts/countryrequest.mjs"
        onMessage: (msg)=> {
            if(msg.ok) {
                choosecb.model = msg.countries
                const idx = msg.countries.findIndex((e) => e === initValuesModel.defaultcountry)
                choosecb.currentIndex = idx;
            } else {
                initValuesModel.changeMessage(msg.txt, 5000, "red")
            }
        }
    }

    function trashBtnCtrl(idx) {
        trashlv.currentIndex = idx
        for(var i=0; i < trashmodel.count; i++) {
            var item = trashmodel.get(i)
            if(i === idx)
                item.active = true
            else
                item.active = false
        }
    }

    function doChanges() {
        initValuesModel.setDefaultCountry(choosecb.currentText)
        initValuesModel.setVolume(setslider.value)
        setpage.deletedFavorites.forEach((item) => initValuesModel.removeFavorite(item))
        initValuesModel.setFavoriteString()
        setpage.deletedFavorites.splice(0, setpage.deletedFavorites.length)
        changebtn.enabled = false
    }
}


