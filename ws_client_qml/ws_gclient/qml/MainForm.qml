import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12


    SplitView{
        id: mainSplit
        anchors.fill: parent
        orientation: Qt.Horizontal

        property string theme: "Dark" //pageMainForm.theme
        property string userUuid

        signal connectionBroken;
        signal getUserInfo(string uuid)

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
            if (!activeChats.isAlreadyAdded(recipient))
                activeChats.setChatItem(recipient, recipientName, true);
        }

        function chatBoxRemove(uuid){
            chatBox.removeChat(uuid)
        }

        property string draft: ""
        property string currentChat: ""

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

        function setStatusUser(resp, value){
            activeChats.setStatusUser(resp, value)
        }
        function clientLeave(resp){
            activeChats.clientLeave(resp)
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
                            dialogMsg.textMsg = msg
                            dialogMsg.open()
                        }
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
            }

            onRemoveItem: function(uuid){
                mainSplit.chatBoxRemove(uuid)
                if(mainSplit.currentChat === uuid)
                    mainSplit.currentChat = ""
            }

            onGetUserInfo: function(uuid){
                mainSplit.getUserInfo(uuid)
            }
        }

    }


//}


