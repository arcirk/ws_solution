import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import arcirk.bWebSocket 1.0

ApplicationWindow {
    id: mainForm
    width: 800
    height: 600
    visible: true
    title: qsTr("Чат")
    minimumHeight: 600
    minimumWidth: 900

    property string myTheme: "Dark"

    property string myHost: "192.168.43.18"
    property int myPort: 8080

    Material.theme: myTheme === "Light" ? Material.Light : Material.Dark

    MessageDialog {
        id: dialogMsg
        width: 400

    }

    WebSocket{
        id: wsClient
        host: mainForm.myHost
        port: mainForm.myPort
        catalog: catalogModel

        onCloseConnection: {
            authForm.enabledForm = true
        }

        onConnectionSuccess: {
            mainForm.connectState = true
            mainStack.push(mainChatBox)

        }
        onQmlError: function(what, err){
            mainStack.currentItem.webSocketError(what, err)
        }

    }

    property bool connectState: false

    menuBar: ToolBar{
        id: maniToolBar
        width: parent.width
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"
        RowLayout{
            width: parent.width
            ToolButton{
                icon.source: "qrc:/team.svg"
                enabled: mainForm.connectState ? true : false
                onClicked: {
                    drawer.open()
                }
            }

            Row{
                id: rightButtons
                Layout.alignment: Qt.AlignRight
                ToolButton{
                    id: btnTheme
                    icon.source: "qrc:/images/mbriwebsitetheme_99562.svg"
                    onClicked: {
                        mnuTheme.open()
                    }
                }
                ToolButton{
                    id: btnExit
                    //text: "Выход"
                    enabled: mainForm.connectState ? true : false
                    icon.source: "qrc:/images/exit.svg"
                    onClicked: {
                        //mnuTheme.open()
                       mainForm.connectState = false
                       mainStack.pop()
                       wsClient.close()
                    }
                }
            }


            Menu {
                id: mnuTheme
                x: rightButtons.x;
                y: btnTheme.y + btnTheme.height;
                Action {
                    text: "Темная"
                    onTriggered: {
                        mainForm.myTheme = "Dark"
                    }
                }
                Action {
                    text: "Светлая"
                    onTriggered: {
                        mainForm.myTheme = "Light"
                    }

                }
            }
        }
    }

    Drawer {

        id: drawer
        width: 400 //Math.min(mainForm.width, mainForm.height) / 3 * 2
        height: mainForm.height
        //interactive: stackView.depth === 1       

        UsersGroup{
            id: drowerContent
            anchors.fill: parent
            theme: mainForm.myTheme
            //catalogModel: mainForm.catalogModel


        }

    }

    StackView{
        id: mainStack
        anchors.fill: parent

        MainForm{
            id: mainChatBox
            theme: myTheme
            //visible: false

        }

        initialItem: AuthForm{
            id:authForm
            saveHash: wsClient.saveHash
            autoConnect: wsClient.autoConnect
            pwdEdit: wsClient.pwdEdit
            user: wsClient.user
            enabledForm: true

            onAccept: function(user, pwd){
                wsClient.open(user, pwd)
            }

            onPwdEditChanged: {
                wsClient.pwdEdit = authForm.pwdEdit
            }
            onSaveHashChanged: {
                wsClient.saveHash = authForm.saveHash
            }
            onAutoConnectChanged: {
                wsClient.autoConnect = authForm.autoConnect
            }

        }
    }

    onWidthChanged: {
        //mainChatBox.parentSizeChange();
        //mainChatBox.smaileBoxVisible(false)
    }
    onHeightChanged: {
        //mainChatBox.parentSizeChange();
        //mainChatBox.smaileBoxVisible(false)
    }


    onConnectStateChanged: {

    }

    Component.onCompleted: {
        console.debug(mainForm.catalogModel)
    }
}
