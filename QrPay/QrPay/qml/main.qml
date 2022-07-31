import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import arcirk.WebSocketClient 1.0

ApplicationWindow {
    width: 480
    height: 800
    visible: true
    title: qsTr("Qr Pay")

    property string myTheme: "Dark"
    property string currentPage: "PageStart"
    property string oldPage: "PageStart"
    //property QtObject wsClient: value

    Material.theme: myTheme === "Light" ? Material.Light : Material.Dark

    property WebSocketClient wsClient: WebSocketClient{
        id: webSocketClient
        url: "ws://192.168.10.12:8080"

        onConnectionSuccess: {
            console.log("onConnectionSuccess");
        }

        onDisplayError: function(what, err){

        }
    }

    menuBar: ToolBar{
        id: mainToolBar
        width: parent.width
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"

        RowLayout{
           width: parent.width

           ToolButton{
               id: btnPageQr
               icon.source: "qrc:/img/qr16.png"

               onClicked: {
                    if(stackView.currentItem != pageQr){
                       var item = stackView.find(function(item) {
                           return item.objectName === "pageQr";
                       })
                       if(item === null)
                           stackView.push(pageQr);
                       else
                          stackView.pop(item)
                    }//else
                    //    stackView.pop()

               }
           }
           ToolButton{
              id: btnShareQr
              icon.source: "qrc:/img/shareFiles.png"
           }
           ToolButton{
              id: btnDeleteQr
              icon.source: "qrc:/img/deleteQr.png"
              onClicked: {
                  stackView.pop(null)
              }
           }
           Label{
               id: lblTitle
               Layout.fillWidth: true
               text:  ""
           }

           Row{
               Layout.alignment: Qt.AlignRight

               ToolButton{
                   id: btnConnectionState
                   icon.source: "qrc:/img/lan_check_online.png"
               }
               ToolButton{
                   id: btnOptions
                   icon.source: "qrc:/img/setting.png"

                   onClicked: {
                       if(stackView.currentItem != pageOptions){
                           var item = stackView.find(function(item) {
                               return item.objectName === "pageOptions";
                           })
                           if(item === null)
                               stackView.push(pageOptions);
                           else{
                                  stackView.pop(item)
                           }
                       }else
                           stackView.pop()
                   }
               }
           }
        }
    }

    StackView{
        id: stackView
        anchors.fill: parent

        initialItem: PageStart{
            id: pageStart
        }


    }

    PageQr{
        objectName: "pageQr"
        id: pageQr
        visible: false
    }
    PageOptions{
        objectName: "pageOptions"
        id: pageOptions
        theme: myTheme
        visible: false
    }

    onWindowStateChanged: {
        console.log( "onWindowStateChanged (Window), state: " + windowState );
        if(!wsClient.isStarted() && windowState === 0){
            wsClient.open("admin", "admin", "qt_qr_pay_app");
        }
    }

}
