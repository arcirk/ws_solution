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

    property alias myHost: wsClient.host//srvSettingsDlg.serverAdrr
    property alias myPort: wsClient.port //srvSettingsDlg.serverPort

    Material.theme: myTheme === "Light" ? Material.Light : Material.Dark

    MessageDialog {
        id: dialogMsg
        width: 400


    }

    WebSocket{
        id: wsClient
//        host: mainForm.myHost
//        port: mainForm.myPort
        //catalog: catalogModel

        onCloseConnection: {
            authForm.enabledForm = true
        }

        onConnectionSuccess: {
            mainForm.connectState = true
            mainChatBox.userUuid = wsClient.uuidUser
            mainStack.push(mainChatBox)
            mainForm.title = wsClient.user
//            console.log("uuid: " + wsClient.uuidSession)
//            console.log("uuid_user: " + wsClient.uuidUser)
//            console.log("user: " + wsClient.user)

        }
        onQmlError: function(what, err){

            let error = err;
            mainStack.currentItem.webSocketError(what, error)
            mainForm.connectState = wsClient.started
//            if(!wsClient.isOpen() && mainStack.currentItem == mainChatBox){
//                mainStack.pop()
//                authForm.enabledForm = true
//                if(err === "End of file"){
//                    error = "Соединение разорвано!"
//                }
//            }


        }

        onResetUsersCatalog: function(resp){
            drowerContent.jsonRespCatalog = resp;
        }

        onGetUserCache: function(resp){
            mainChatBox.setCache(resp)
        }

        onSetMessages: function(resp){
            mainStack.currentItem.websocketSetMessages(resp)
        }

        onMessageReceived: function(msg, uuid, recipient){
            mainChatBox.messageReceived(msg, uuid, recipient)
        }

        onConnectedStatusChanged: function(val){
            console.debug("connection status: " + val)
            btnConected.icon.source = val ? "qrc:/images/server_network_icon_138203.svg" : "qrc:/images/server_network_off_icon_136233.svg"
            srvSettingsDlg.enabled = !val
        }

    }

    ServerSettingsDialog{

        id: srvSettingsDlg
        serverAdrr: wsClient.host
        serverPort: wsClient.port
        //enabled: false

        onClosed: {
            if(!wsClient.started){
                wsClient.host = srvSettingsDlg.serverAdrr
                wsClient.port = srvSettingsDlg.serverPort
            }
        }
    }

    onClosing: {
        if(wsClient.started){
            var cache = mainChatBox.getActiveUsersJsonText()
            if(cache)
                wsClient.saveCache(cache)
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
            Label{
                id: chatName
                Layout.fillWidth: true
                text:  ""
            }

            Row{
                id: rightButtons
                Layout.alignment: Qt.AlignRight

                ToolButton{
                    id: btnConected
                    icon.source: "qrc:/images/server_network_off_icon_136233.svg"

                    onClicked: {
                        srvSettingsDlg.open()
                    }
                }
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
                        if(wsClient.started){
                            var cache = mainChatBox.getActiveUsersJsonText()
                            if(cache)
                                wsClient.saveCache(cache)
                            //wsClient.close();
                        }                        //mnuTheme.open()

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

            onSelect: function(uuid, name){
                mainChatBox.setChatItem(uuid, name)
                drawer.close();

            }

        }

    }

    StackView{
        id: mainStack
        anchors.fill: parent

        signal connectionBroken

        MainForm{
            id: mainChatBox
            theme: myTheme

            onActiveChatsSelected: function(name, uuid){
                mainChatBox.enabled = uuid.length > 0
                chatName.text = "Чат : " + name
            }

            onConnectionBroken: {
                mainStack.connectionBroken()
            }
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

        onConnectionBroken: {
            mainStack.pop()
            authForm.enabledForm = true
        }

    }

}
