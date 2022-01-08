import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import "qrc:/scripts/scripts.js" as Scripts

Pane{

    id: control

    function unchecked(){
        btnSmail.checked = false
    }

    function checked(){
        return btnSmail.checked
    }

    signal stateChanged(string newStae)
    signal sendMessage(string msg)
    signal messageChanged(string source)

    signal selectFile(string fileName, string ref)

    property alias text: txtSendMessage.text
    property string uuidRecipient
    property string roomToken
    property string refMessage

    RowLayout {
        anchors.fill: parent
        ToolButton{
            id: btnSmail
            icon.source: "qrc:/images/smile-svgrepo-com.svg"
            checkable: true
            anchors.margins: 0
            Material.accent: Material.Blue

            onCheckedChanged: {
                if(btnSmail.checked){
                    control.stateChanged("shown")
                }else{control.stateChanged("hidden")}
            }
        }
        ToolButton{
            id: selFile
            icon.source: "qrc:/images/paperclip_material.svg"
            Material.accent: Material.Blue
            onClicked: {
                selDlg.open()
            }
        }

        Flickable {
            id: flickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            TextArea.flickable: TextArea{
                id: txtSendMessage
                textFormat: Text.RichText
                placeholderText: qsTr("Введите сообщение")
                wrapMode: TextArea.Wrap
                Material.accent: Material.Blue
                focus: true
                selectByMouse: true

                onTextChanged: {
                    control.messageChanged(txtSendMessage.text)
                }
            }
            ScrollBar.vertical: ScrollBar { }
        }

        ToolButton{
            signal newMessage(string msg)
            id: btnSend
            objectName: "btnSend"
            icon.source: "qrc:/images/wondicon-ui-free-send_111204.svg"
            Layout.alignment: Qt.AlignRight
            enabled: txtSendMessage.length > 0

            onClicked: {
                wsClient.sendMessage(uuidRecipient, txtSendMessage.text)
                txtSendMessage.text = "";
            }
        }

   }

    SelectFileDialog{
        id: selDlg

        onSelectFile: function(fpath, name){

//            if(!wsClient.verifyLocalRoomCacheDirectory(roomToken))
//            {
//               console.log("SelectFileDialog: error verify chache directory!")
//                return
//            }

//            //копируем файл в кеш пользователя с начала
//            let res = wsClient.saveFileToUserCache(roomToken, fpath, "");
//            if(!res)
//                return;

//            if(roomToken === undefined)
//            {
//                console.log("не инициализирован токен чата!")
//                return
//            }
            if(roomToken.length === 0)
            {
                console.log("не инициализирован токен чата!")
                return
            }
            let cache = txtSendMessage.text;
            let msg_ref = wsClient.getRandomUUID();
            txtSendMessage.text = wsClient.getObjectHtmlSource(fpath)
            wsClient.sendMessage(uuidRecipient, txtSendMessage.text, wsClient.getFileName(fpath), msg_ref)
            txtSendMessage.text = cache;
            control.selectFile(fpath, msg_ref);
        }
    }

//    Image {
//        id: emptyImg
//        visible: false

//    }
}
