import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15

Page{

    id: authPage
    objectName: "authPage"
    width: parent.width
    height: parent.height

    Item {
        anchors.centerIn: parent
        id: item1
        //property alias cancelButton: cancelButton
        property alias loginButton: loginButton
        property alias userName: userName
        property alias password: password

        ColumnLayout {
            width: 300
            id: columnLayout
            anchors.centerIn: parent
            anchors.topMargin: 20
            anchors.top: parent.top
            anchors.bottomMargin: 20
            anchors.bottom: parent.bottom
            anchors.rightMargin: 20
            anchors.right: parent.right
            anchors.leftMargin: 20
            anchors.left: parent.left

            Image {
                id: image
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                //source: "qrc:/icon.svg"
            }

            Rectangle {
                id: rectangle
                height: 30
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                anchors.right: parent.right
                anchors.left: parent.left
                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#25a6e2"
                    }
                    GradientStop {
                        color: "#188bd0"
                    }
                }

                Text {
                    id: textArea
                    x: 54
                    y: 5
                    color: "#ffffff"
                    text: qsTr("Авторизация")
                    font.pointSize: 12
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            Item {
                width: 20
                height: 20
            }

            Text {
                id: userNameText
                text: qsTr("Пользователь:")
                font.pointSize: 12
            }

            TextField {
                id: userName
                width: 300
                height: 26
                Layout.fillWidth: true
                font.pointSize: 12
                //text: wsClient.user
            }

            Text {
                id: passwordText
                text: qsTr("Пароль:")
                font.pointSize: 12
            }

            RowLayout{
                TextField {
                    id: password
                    width: 300
                    height: 26
                    Layout.fillWidth: true
                    font.pointSize: 12
                    enabled: false
                    //text: wsClient.hash
                    echoMode: TextInput.Password

                }

                ToolButton {
                    id: editPass
                    checkable: true
                    icon.source: "qrc:/images/tbglacti.png"
                    onCheckedChanged: {
                        password.enabled = editPass.checked
                        if(editPass.checked){
                           password.text = "";
                        }else
                           password.text = wsClient.hash
                    }
                }
            }



            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                id: rowLayout
                objectName: "rowLayoutObj"
                width: 100
                height: 100

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: loginButton
                    width: 90
                    height: 30
                    text: qsTr("Войти")
                    onClicked:{
//                        if(editPass.checked){
//                            wsClient.open(userName.text, password.text)
//                        }else
//                            wsClient.open(userName.text, "")
//                        password.enabled = false
//                        userName.enabled = false
//                        loginButton.enabled = false
                        authPage.StackView.view.push("qrc:/chatPage.qml", {})

                    }
                }

//                Connections{
//                    target: wsClient
//                    onQmlError:{
//                        rowLayout.doError(what, err)
//                    }
//                    onConnectionSuccess: {
//                        wsClient.activePage = "ChatPage"
//                        authorizationForm.StackView.view.push("qrc:/qml/ChatPage.qml", {})
//                    }
//                    onCloseConnection: {
//                        password.enabled = false
//                        editPass.checked = false
//                        userName.enabled = true
//                        loginButton.enabled = true
//                    }
//                }

//                function doError(what, err){
//                    if(what ==="connect"){
//                        password.enabled = false
//                        editPass.checked = false
//                        userName.enabled = true
//                        loginButton.enabled = true
//                    }
//                }

            }
        }
    }

}
