import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

SplitView{
    id: mainSplit
    anchors.fill: parent
    orientation: Qt.Horizontal

    property string theme: "Dark"

    function parentSizeChange(){
        if(msgBox.checked()){
            msgBox.unchecked()
        }
    }

    function smaileBoxVisible(value){
        smailBox.visible = value
    }

    ColumnLayout{
        SplitView.fillWidth: true
        SplitView.fillHeight: true
        SplitView.minimumWidth: 600

        SplitView{
            anchors.bottom: smailBox.top
            Layout.fillHeight: true
            Layout.fillWidth: true
            //Layout.minimumWidth: 200
            orientation: Qt.Vertical
            ChatBox{
                id: chatBox
                SplitView.fillWidth: true
                SplitView.fillHeight: true
                Material.background: mainSplit.theme === "Light" ? "#ECEFF1" : undefined
                onWidthChanged: {
                    if(msgBox.checked()){
                        msgBox.unchecked()
                    }
                }

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
            }

        }

        SmaileBox{
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
        theme: mainSplit.theme
        Material.background: mainSplit.theme === "Light" ? "#ECEFF1" : undefined
        SplitView.minimumWidth: 200

        onSetMessageModel: function(index){
            chatBox.listModel.setDocument(index)
        }
        onRemoveItem: function(index){
            chatBox.listModel.remove(index)
        }
    }

}
