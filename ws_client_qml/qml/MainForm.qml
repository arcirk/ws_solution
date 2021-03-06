import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import Qt.labs.platform

    SplitView{
        id: mainSplit
        anchors.fill: parent
        orientation: Qt.Horizontal

        property string theme: "Dark" //pageMainForm.theme
        property string userUuid

        signal connectionBroken;
        signal getUserInfo(string uuid)
        //signal resetUnReadMsgFromData(string uuid)

        function setCache(strCache){
            activeChats.setCache(strCache);
        }

        function webSocketError(what, err){
            console.error(what + ": " + err);
            if(!wsClient.started)
                mainSplit.connectionBroken()
        }

        function websocketSetMessages(resp){
            chatBox.setMessages(resp)
        }

        //
        function messageReceived(msg, uuid, recipient, recipientName){
            chatBox.messageReceived(msg, uuid, recipient);
            if(mainSplit.currentChat !== recipient && mainSplit.currentChat.length > 0){
                //счетчик не прочитанных сообщений
                activeChats.setCountUnReadMsg(recipient);
            }
            if (!activeChats.isAlreadyAdded(recipient)){
                activeChats.setChatItem(recipient, recipientName, true);
                //activeChats.setStatusUser(resp, true)
            }
        }

        function chatBoxRemove(uuid){
            chatBox.removeChat(uuid)
        }

        property string draft: ""
        property string currentChat: ""
        property string currentToken: ""

        function setChatItem(uuid, name){
            activeChats.setChatItem(uuid, name);
        }

        signal activeChatsSelected(string name, string uuid)

        function getActiveUsersJsonText(){
            return activeChats.getActiveUsersJsonText()
        }

        function resetActiveUsers(resp){
            activeChats.resetActiveUsers(resp)
        }

//        function setStatusUser(resp, value){
//            activeChats.setStatusUser(resp, value)
//        }

        function updateStatusUser(resp){
            activeChats.setStatusUser(resp)
        }

        function clientLeave(resp){
            activeChats.clientLeave(resp)
        }

        function setUnreadMessages(resp){
            activeChats.setUnreadMessages(resp)
        }

        function setToken(token){
            activeChats.setToken(token)
        }

        function setProgressValue(value){
            progress.value = value
            if(value !== 1 && value !== 0)
                progress.visible = true;
            else
                progress.visible = false;
        }

        MessageDialog {
            property string ref: ""
            property string token: ""
            property string fileName: ""
            property string folder: ""
            id: msgDialog
            text: "Файл уже существует в выбранном каталоге!"
            informativeText: "Файл будет перезаписан, продолжить?"
            buttons: (MessageDialog.Ok | MessageDialog.Cancel)

            onAccepted: {
                wsClient.downloadFile(msgDialog.token, msgDialog.fileName, msgDialog.ref, msgDialog.folder)
            }

        }
        SelectFolderDialog{
            id: saveDlg

            onSelectFolder: function(fpath){

                if(wsClient.fileExists(fpath + wsClient.separator() + fileName)){
                    msgDialog.fileName = saveDlg.fileName;
                    msgDialog.token = saveDlg.token;
                    msgDialog.ref = saveDlg.ref;
                    msgDialog.folder = fpath;
                    msgDialog.open();
                }else
                    wsClient.downloadFile(saveDlg.token, saveDlg.fileName, saveDlg.ref, fpath)
            }
        }



        Page{
            SplitView.fillWidth: true
            SplitView.fillHeight: true
            SplitView.minimumWidth: 500
            ColumnLayout{

                anchors.fill: parent

                SplitView{
                    anchors.bottom: smailBox.top
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    orientation: Qt.Vertical
                    ChatBox{
                        id: chatBox
                        userUuid: mainSplit.userUuid
                        SplitView.fillWidth: true
                        SplitView.fillHeight: true
                        Material.background: mainSplit.theme === "Light" ? "#ECEFF1" : undefined

                        onMessageClick: function(msg) {
                            //console.debug(msg)
                            dialogMsg.textMsg = msg
                            dialogMsg.open()
                        }
                        onTokenChanged: function(token){
                            mainSplit.setToken(token)
                        }
                        onSaveAs: function(token, object_name, msg_ref){
                            saveDlg.fileName = object_name;
                            saveDlg.token = token;
                            saveDlg.ref = msg_ref;
                            saveDlg.title = "Выберете директорию";
                            saveDlg.open();
                        }
                    }
                    ProgressBar{
                        id: progress
                        value: 0.5
                    }
                    MessageBox{
                        id: msgBox
                        objectName: "msgBox"
                        SplitView.minimumHeight: 70
                        SplitView.maximumHeight: 200
                        Material.background: mainSplit.theme === "Light" ? "#ECEFF1" : undefined

                        onStateChanged: function(newState){
                            smailBox.visible = true;
                            smailBox.state = newState
                        }

                        onSendMessage: function(msg){
                            chatBox.listModel.sendMessage(msg)
                            if(msgBox.checked()){
                                msgBox.unchecked()
                                smailBox.stateChanged("hidden")
                            }
                        }

                        onMessageChanged: function(source){
                            //activeChats.setCurrentDraft(source)
                            mainSplit.draft = source;
                        }

                        onSelectFile: function(fileName, ref){
                            //console.debug(activeChats.getCurrentToken())
                            wsClient.uploadFile(activeChats.getCurrentToken(), fileName, ref)
                        }
                    }

                }
            }

            footer: SmaileBox{
                id: smailBox
                Layout.alignment: Qt.AlignBottom
                Layout.fillWidth: true
                height: 70
                Layout.maximumHeight: 70
                anchors.bottom: parent.bottom
                Material.background: mainSplit.theme === "Light" ? "#ECEFF1" : undefined
                onClosePane: {
                    msgBox.unchecked()
                }

            }
        }

//        signal requestUserData(string uuid, string param)

//        function updateUserData(resp){
//            activeChats.updateUserData(resp)
//        }

        UsersBox{
            id: activeChats
            theme: mainSplit.theme
            Material.background: mainSplit.theme === "Light" ? "#ECEFF1" : undefined
            SplitView.minimumWidth: 300
            userUuid: mainSplit.userUuid


            onSelectedIem: function(name, uuid){
                if(mainSplit.currentChat.length > 0)
                    activeChats.saveDraft(mainSplit.currentChat, msgBox.text)
                mainSplit.activeChatsSelected(name, uuid)
                mainSplit.currentChat = uuid
                msgBox.text = activeChats.getDraft();
                chatBox.seChatMessages(uuid)
                msgBox.uuidRecipient = mainSplit.currentChat;
                msgBox.roomToken = activeChats.getCurrentToken();

            }

            onRemoveItem: function(uuid){
                mainSplit.chatBoxRemove(uuid)
                if(mainSplit.currentChat === uuid){
                    mainSplit.currentChat = ""
                    mainSplit.currentToken = ""
                }

            }

            onGetUserInfo: function(uuid){
                mainSplit.getUserInfo(uuid)
            }

//            onRequestUserData: function(uuid, param){
//                mainSplit.requestUserData(uuid, param)
//            }
        }

    }


//}


