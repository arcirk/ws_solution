import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Pane {

    id: usersBox

    signal setMessageModel(int index);
    signal removeItem(int index);

    ListView{
        id: listUsers
        anchors.fill: parent
        displayMarginBeginning: 40
        displayMarginEnd: 40
        //verticalLayoutDirection: ListView.BottomToTop
        spacing: 12
        model: usersModel


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
                    Layout.fillWidth: true
                    height: messageText.implicitHeight// + 24
                    ctrlPaddig: 10
                    //icon: "qrc:/user.png"
                    checkable: true
                    checked: usersModel.currentIndex === model.indexDoc ? true : false
                    textColor:{
                        if(mainForm.theme === Material.Dark){
                            if(messageText.checked){
                                "blue"
                            }else
                            {
                                "white"
                            }
                        }else{
                            if(messageText.checked){
                                "blue"
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
                        //console.debug(usersModel.currentIndex)
                        mainForm.title = "Чат - " + model.name
                        usersBox.setMessageModel(model.indexDoc)
                        usersModel.currentIndex = model.indexDoc
                    }

                }
                IconPane{
                    id: btnInfo
                    //name: "x"
                    icon: "qrc:/images/info16.png"
                    ctrlPaddig: 2
                    menuDisable: true
                    width: 20

                    onImageClick:{

                    }
                }
                IconPane{
                    id: btnCloseChat
                    //name: "x"
                    icon: "qrc:/images/deleteUser.png"
                    ctrlPaddig: 2
                    menuDisable: true
                    width: 20// messageText.height
                    //height: messageText.height

                    onImageClick:{
                        let index = model.indexDoc
                        usersBox.removeItem(index)
                        usersModel.remove(index)
                    }
                }
            }
        }
        ScrollBar.vertical: ScrollBar {}
        ScrollBar.horizontal: ScrollBar {}
    }


}
