import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import org.example 1.0

Page {
    required property MediaPlayer player
    property string country:        initValuesModel.defaultcountry
    property string favoritestring: initValuesModel.favoritestring
    property string url: initValuesModel.poolurlstring

    header: Row {
        ComboBox {
            property var countries: []
            id: countrycb
            width: parent.width / 2
            onActivated: {
                var country = currentText !== qsTr("all countries") ? currentText : ""
                var tag = tagcb.currentText !== qsTr("all genres") ? tagcb.currentText : ""
                stationlistModel.stationRequest(country,tag)
            }

            Component.onCompleted: {
                countryworker.sendMessage({'countries': countrycb.countries,'text': "", 'ok': false, 'minItems': 100})
            }
        }

        ComboBox {
           property var tags: []
           id: tagcb
           width: parent.width / 2
           onActivated: {
               var country = countrycb.currentText !== qsTr("all countries") ? countrycb.currentText : ""
               var tag = currentText !== qsTr("all genres") ? currentText : ""
               stationlistModel.stationRequest(country,tag)
            }

            Component.onCompleted: {
              tagworker.sendMessage({'tags': tagcb.tags,'text': "", 'ok': false, 'minItems': 150})
            }
        }
    }

    SplitView {
        anchors.fill: parent
        ListView {
            property var curritem

            SplitView.fillWidth: true
            SplitView.fillHeight: true
            clip: true
            id: listview
            currentIndex: -1
            model: stationlistModel

            delegate: Item  {
               id: delegate
                width: ListView.view.width
                height: 36
                anchors.topMargin: 2
                anchors.bottomMargin: 5

                Image {
                    id: imgid
                    source: img
                    height: 32; width: 32
                    anchors.leftMargin: 5
                }

                Control {
                    topPadding: 6
                    leftPadding: 37
                    contentItem: Text {
                        color: "white"
                        text: name
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        listview.currentIndex = index
                        var item = stationlistModel.item(index)

                        listview.curritem = {name: item.name, url: item.url, favicon: item.favicon}
                        window.enableAddButton()
                        initValuesModel.changeMessage("",0,"yellow")
                        initValuesModel.locationRequest(item.url)
                        changeStation(item.url)
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                width: 20
                policy: ScrollBar.AlwaysOn
            }

            highlight: Rectangle { width: 500; height:20; color: "lightblue"; radius: 5 }
        }

        ListView {
            id: favoritelist
            implicitWidth: 60
            SplitView.preferredWidth: 60
            SplitView.maximumWidth: 60
            SplitView.fillHeight: true
            model: favoritelistModel

            delegate: Item {
                id: favitem
                width: 50
                height: 50
                anchors.margins: 5

                Image {
                    id: favimg
                    source: favicon
                    height: 44; width: 44
                    //anchors.verticalCenter: parent.verticalCenter
                }

                MouseArea {
                    id: maf
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        if(index !== -1) {
                            const item = favoritelistModel.get(index)
                            tt.text = item.name
                            tt.visible = true
                        }
                    }
                    onExited: {
                        tt.visible = false
                    }

                    onClicked: {
                        const item = favoritelistModel.get(index)
                        listview.currentIndex = -1
                        initValuesModel.changeMessage(item.name,5000,"yellow")
                        initValuesModel.locationRequest(item.url)
                        changeStation(item.url)
                    }

                    ToolTip {
                        id: tt
                    }
                }
            }
        }
    }

    StationListModel {
        id: stationlistModel
    }

    ListModel {
        id: favoritelistModel
    }

    onUrlChanged: {
        if(radiopage.url !== "") {
            changeStation(radiopage.url)
        }
    }

    onFavoritestringChanged: {
        if(radiopage.favoritestring !== "") {
            favoritelistModel.clear()
            const ja = JSON.parse(radiopage.favoritestring)
            ja.forEach((obj) => {
                if(obj.favicon === "")
                    obj.favicon = "qrc:/icons/default"
                favoritelistModel.append(obj)
            })
        }
    }

    WorkerScript {
        id: countryworker
        source: "qrc:/scripts/countryrequest.mjs"
        onMessage: (msg)=> {
            if(msg.ok) {
                msg.countries.unshift(qsTr("all countries"))
                countrycb.model = msg.countries
                const idx = msg.countries.findIndex((e) => e === radiopage.country)
                countrycb.currentIndex = idx;
                stationlistModel.stationRequest(countrycb.currentText, "")
            } else {
                initValuesModel.changeMessage(msg.txt, 5000, "red")
            }
        }
    }

    WorkerScript {
        id: tagworker
        source: "qrc:/scripts/tagrequest.mjs"
        onMessage: (msg)=> {
            if(msg.ok) {
                msg.tags.unshift(qsTr("all genres"))
                tagcb.model = msg.tags
            } else {
                initValuesModel.changeMessage(msg.txt, 5000, "red")
            }
        }
    }

    WorkerScript {
        id: favoriteworker
        source: "qrc:/scripts/addFavorite.mjs"
    }

    function changeStation(url) {
        player.stop();
        player.source = url;
        player.play();
    }

    function addFavItem() {
        var toadd = true;

        for(var i=0; i < favoritelistModel.count; i++) {
            if(favoritelistModel.get(i).name === listview.curritem.name) {
                toadd = false;
                break;
            }
        }
        if(toadd) {
            initValuesModel.addFavorite(listview.curritem)
            favoriteworker.sendMessage({model: favoritelistModel, item: listview.curritem});
            initValuesModel.setFavoriteString()
        }
        return toadd;
    }
}












