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

    signal stateChanged(string newState)
    signal sendMessage(string msg)
    signal messageChanged(string source)

    property alias text: txtSendMessage.text
    property string uuidRecipient

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
                //btnSend.newMessage(txtSendMessage.text)
                //control.sendMessage(txtSendMessage.text)
                wsClient.sendMessage(uuidRecipient, txtSendMessage.text)
                txtSendMessage.text = "";
            }
        }

   }

    SelectFileDialog{
        id: selDlg

        onSelectFile: function(fpath, name){
            let cashe = txtSendMessage.text;

            if(Scripts.isImage(name.toLowerCase().substr(-3, 3) )){
                txtSendMessage.text = Scripts.getImageHtmlString(fpath, emptyImg)//"<img width=\""+ img.width +"\" height=\""+ img.height +"\" align=\"middle\" src=\""+fpath+"\">"
            }
            else
                  txtSendMessage.text = "<a href=\"" + fpath + "\">"+name+"</a>"

            btnSend.newMessage(txtSendMessage.text)
            control.sendMessage(txtSendMessage.text)
            console.log(Qt.resolvedUrl("pics/logo.png"))
            txtSendMessage.text = cashe;
        }
    }

    Image {
        id: emptyImg
        visible: false

    }
}
