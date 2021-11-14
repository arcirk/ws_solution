import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import SelectedUsersModel 1.0

Pane {

    id: usersBox

    property string theme: "Dark"

    signal setMessageModel(var modelindex);
    signal removeItem(string uuid);

    function getDraft(){
        let draft = usersModel.getDraft();
        return  draft
    }

    function saveDraft(uuid, source){
        usersModel.saveDraft(uuid, source)
//        console.log(uuid)
//        console.log(source)
    }

    function setCache(strCache){
        usersModel.jsonText = strCache;
    }


    function updateCache(jsonText){
        wsClient.saveCache(jsonText);
    }

    //вызывается по цепочке main->MainForm->UsersBox
    function setChatItem(uuid, name){
        usersModel.addRow(uuid, name);
        updateCache(usersModel.jsonText)
    }

    signal selectedIem(string name, string uuid)

    function getActiveUsersJsonText(){
        return usersModel.jsonText
    }

    property string userUuid

    SelectedUsersModel{
        id: usersModel
        userUuid: usersBox.userUuid

        onCurrentRowChanged: {
            var modelIndex = usersModel.item(usersModel.currentRow);
            var name = usersModel.data(modelIndex, Qt.UserRole + 1);
            var uuid = usersModel.currentRow;
            usersBox.selectedIem(name, uuid)
        }
    }

    ListView{
        id: listUsers
        anchors.fill: parent
        displayMarginBeginning: 40
        displayMarginEnd: 40
        spacing: 12
        model: usersModel

        signal selectedRow(var modelindex)
        signal removeRow(var modelindex)

        delegate: Column {
            //Layout.fillHeight: true
            Layout.fillWidth: true
            spacing: 6

            Row{
                id: usersRow
                spacing: 6
                Image {
                    id: avatar
                    //source: !sentByMe ? "qrc:/" + model.author.replace(" ", "_") + ".png" : ""
                }
                IconPane{
                    id: messageText
                    name: model.name
                    width: listUsers.width - (btnInfo.width * 2 + 10 + 10)
                    height: messageText.implicitHeight// + 24
                    ctrlPaddig: 5
                    checkable: true
                    theme: usersBox.theme

                    checked: usersModel.currentRow === model.uuid ? true : false

                    textColor:{
                        if(usersBox.theme === "Dark"){
                            if(messageText.checked){
                                "#bbdefb"
                            }else
                            {
                                "white"
                            }
                        }else{
                            if(messageText.checked){
                                "#2196f3"
                            }else
                            {
                                "black"
                            }
                        }
                    }

                    onMenuTriggered: {
                        //ctrlMessageList.messageClick(model.message)
                    }

//                    onImageClick: {
//                        //console.debug("Image click")
//                    }

                    onClicked: {
                        //mainForm.title = "Чат - " + model.name
                        listUsers.selectedRow(model)
                    }

                }
                IconPane{
                    id: btnInfo
                    //name: "x"
                    icon: "qrc:/images/info16.png"
                    ctrlPaddig: 2
                    menuDisable: true
                    width: 20
                    iconButton: true

                    onImageClick:{

                    }
                }
                IconPane{
                    id: btnCloseChat

                    icon: "qrc:/images/deleteUser.png"
                    ctrlPaddig: 2
                    menuDisable: true
                    width: 20
                    iconButton: true

                    onImageClick:{
                        listUsers.removeRow(model)
                    }
                }
            }
        }

        onSelectedRow: function(modelindex){
            model.currentRow = modelindex.uuid
            //usersBox.selectedIem(modelindex)
        }

        onRemoveRow: function(modelindex){
            usersBox.removeItem(modelindex.uuid)
            model.removeRow(modelindex.uuid)
            usersBox.updateCache(model.jsonText)
        }

        ScrollBar.horizontal: ScrollBar {}
        ScrollBar.vertical: ScrollBar {}

    }

}
