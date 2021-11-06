import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12


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

    property alias text: txtSendMessage.text

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
        Flickable {
            id: flickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            TextArea.flickable: TextArea{
                id: txtSendMessage
                textFormat: Text.RichText
                //anchors.fill: parent
                //Layout.fillWidth: true
                //Layout.fillHeight: true
                placeholderText: qsTr("Введите сообщение")
                wrapMode: TextArea.Wrap
                Material.accent: Material.Blue
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
                btnSend.newMessage(txtSendMessage.text)
                //console.debug(txtSendMessage.text)
                //listView.model.sendMessage(messageField.text)
                control.sendMessage(txtSendMessage.text)
                txtSendMessage.text = "";
            }
        }

   }

}
