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

    MessageListModel{
        id: msgModel
        userUuid: ctrlMessageList.userUuid

        onGetMessagesForRecipient: function(uuid){
            wsClient.messages(uuid);
        }
    }


    function setMessages(resp){
        msgModel.addDocument(resp)
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
//        Layout.fillWidth: true
//        Layout.fillHeight: true
        //Layout.topMargin: 110
        //Layout.margins: pane.leftPadding + messageField.leftPadding
        displayMarginBeginning: 40
        displayMarginEnd: 40
        verticalLayoutDirection: ListView.BottomToTop
        spacing: 12
        model: msgModel

        delegate: Column {
            anchors.right: sentByMe ? listView.contentItem.right : undefined
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

                    onMenuTriggered: {
                        ctrlMessageList.messageClick(model.message)
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
