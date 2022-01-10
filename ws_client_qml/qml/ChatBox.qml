import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import MessageListModel 1.0
import QProxyModel 1.0

Pane{

    id: ctrlMessageList

    property string userUuid
    property alias listModel: listView.model

    signal messageClick(string msg);
    signal tokenChanged(string token);
    signal saveAs(string token, string object_name, string msg_ref)

    MessageListModel{
        id: msgModel
        userUuid: ctrlMessageList.userUuid

        onGetMessagesForRecipient: function(uuid){
            wsClient.messages(uuid);
        }

        onTokenChanged: function(token){
            //console.debug("token changed: " + token)
            ctrlMessageList.tokenChanged(token)
        }
    }


    function setMessages(resp){
        msgModel.addDocument(resp)
        //console.debug(msgModel.currentToken);
    }

    function seChatMessages(uuid){
        msgModel.setDocument(uuid)
    }

    function messageReceived(msg, uuid, recipient){
        msgModel.addMessage(msg, uuid, recipient)
    }

    function removeChat(uuid){
        msgModel.remove(uuid)
    }

    function getCurrentChat(){
        return msgModel.currentRecipient;
    }

    ListView {
        id: listView
        objectName: "listMessages"
        anchors.fill: parent
        displayMarginBeginning: 40
        displayMarginEnd: 40
        verticalLayoutDirection: ListView.BottomToTop
        spacing: 12
        model: msgModel

        delegate: Column {
            anchors.right: !sentByMe ? listView.contentItem.right : undefined
            spacing: 6

            readonly property bool sentByMe: model.FirstField !== ctrlMessageList.userUuid

            Row {
                id: messageRow
                spacing: 6
                anchors.right: sentByMe ? parent.right : undefined

                Image {
                    id: avatar

                    //source: !sentByMe ? "qrc:/" + model.author.replace(" ", "_") + ".png" : ""
                }
                IconPane{
                    id: messageText
                    name: model.message
                    menuDeleteDisable: sentByMe
                    width: Math.min(messageText.implicitWidth + 24,
                        listView.width - (!sentByMe ? avatar.width + messageRow.spacing : 0))
                    height: messageText.implicitHeight// + 24
                    ctrlPaddig: 10
                    //icon: "qrc:/user.png"

                    textColor: sentByMe ? "black" : "white"
                    Material.background: sentByMe ? "lightgrey" : "steelblue" //sentByMe ? Material.Grey : Material.Blue

                    onMenuTriggered: function(name) {
                        //console.log("ChatBox::messageText::onMenuTriggered:model.object_name: " + model.object_name)
                        //console.log("ChatBox::messageText::onMenuTriggered:model.msg_ref: " + model.msg_ref)
                        if(name === "mnuOpen")
                            ctrlMessageList.messageClick(model.message)
                        else if (name === "mnuSaveAs")
                            ctrlMessageList.saveAs(model.token, model.object_name, model.msg_ref)
                    }

                    onImageClick: {
                        //console.debug("Image click")
                    }
                }
            }
            Label {
                id: timestampText
                text: Qt.formatDateTime(model.date, "d MMM hh:mm")
                color: "lightgrey"
                anchors.right: sentByMe ? parent.right : undefined
            }


        }

        ScrollBar.vertical: ScrollBar {}
    }


}
