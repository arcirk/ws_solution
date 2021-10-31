import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: mainForm
    width: 800
    height: 600
    visible: true
    title: qsTr("Чат")
    minimumHeight: 600
    minimumWidth: 800
    Material.theme: Material.Dark

    property int theme: Material.theme

    MessageDialog {
        id: dialogMsg
    }

    SplitView{

        anchors.fill: parent
        orientation: Qt.Horizontal

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

                    onStateChanged: function(newState){
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

                onClosePane: {
                    msgBox.unchecked()
                }

            }
        }

        UsersBox{
            SplitView.minimumWidth: 200

            onSetMessageModel: function(index){
                //chatBox.listModel = newModel;
                chatBox.listModel.setDocument(index)
            }
        }

    }

    onWidthChanged: {
        if(msgBox.checked()){
            msgBox.unchecked()
        }
    }
    onHeightChanged: {
        if(msgBox.checked()){
            msgBox.unchecked()
        }
    }

}
