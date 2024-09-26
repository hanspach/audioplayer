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
            width: parent.width / 2 - 50
            onActivated: {
                var country = currentText !== qsTr("all countries") ? currentText : ""
                var tag = tagcb.currentText !== qsTr("all genres") ? tagcb.currentText : ""
                stationlimitxt.text = ""
                stationlistModel.stationRequest(country,tag)
            }

            Component.onCompleted: {
                countryworker.sendMessage({'countries': countrycb.countries,'text': "", 'ok': false, 'minItems': 100})
            }
        }

        TextField {
           id: stationlimitxt
           width: 100
           placeholderText: "limit station"
           onTextChanged: {
               var country = countrycb.currentText !== qsTr("all countries") ? countrycb.currentText : ""
               var tag = tagcb.currentText !== qsTr("all genres") ? tagcb.currentText : ""
               stationlistModel.stationRequest(country,tag,stationlimitxt.text)
           }
        }

        ComboBox {
           property var tags: []
           id: tagcb
           width: parent.width / 2 - 50
           onActivated: {
               var country = countrycb.currentText !== qsTr("all countries") ? countrycb.currentText : ""
               var tag = currentText !== qsTr("all genres") ? currentText : ""
               stationlimitxt.text = ""
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
            highlightFollowsCurrentItem: false

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
                        initValuesModel.changeMessage("",2500,"yellow")
                        initValuesModel.locationRequest(item.url)
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                width: 20
                policy: ScrollBar.AlwaysOn
            }

            highlight: Rectangle {
                width: listview.width-25; height:20;
                color: "lightblue"; radius: 5
                y: listview.currentItem.y+5
                Behavior on y {
                    SpringAnimation {
                       spring: 3
                       damping: 0.2
                    }
                }
            }
        }

        ListView {
            id: favoritelist
            implicitWidth: 50
            SplitView.preferredWidth: 50
            SplitView.maximumWidth: 50
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
                        initValuesModel.changeMessage(item.name,2500,"yellow")
                        initValuesModel.locationRequest(item.url)
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
        onMessage: (msg) => {
            initValuesModel.addFavorite(msg.item)
            initValuesModel.setFavoriteString()
        }
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
            favoriteworker.sendMessage({model: favoritelistModel, item: listview.curritem});
        }
        return toadd;
    }
}












