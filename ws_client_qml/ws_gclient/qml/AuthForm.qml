import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15

Page{

    id: authPage
    objectName: "authPage"
    anchors.fill: parent

    property string theme: "Dark"

    property alias saveHash: chSave.checked
    property string user
    property alias autoConnect: chAutoConnect.checked
    property alias pwdEdit: editPass.checked
    property alias enabledForm: columnLayout.enabled

    signal accept(string user, string pwd);
    function webSocketError(what, err){
        columnLayout.enabled = true
        txtError.text = err
    }
    function websocketSetMessages(resp){
        //для совместимости
    }
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
                Layout.alignment: Qt.AlignCenter
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                anchors.right: parent.right
                anchors.left: parent.left
                //Layout.alignment: Qt.AlignRight && Qt.AlignLeft
                Layout.fillHeight: true

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: "#25a6e2"
                    }
                    GradientStop {
                        color: "#188bd0"
                    }
                }

                Label {
                    id: textArea
                    x: 54
                    y: 5
                    //color: "#ffffff"
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

            Label {
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
                Material.accent: Material.Blue
                text: user
                placeholderText: qsTr("Имя пользователя")
            }

            Label {
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
                    text: chSave.checked ? "**" : ""
                    echoMode: TextInput.Password
                    Material.accent: Material.Blue
                    placeholderText: qsTr("Пароль")

                }
                ToolButton {
                    id: viewPass
                    checkable: true
                    checked: true
                    icon.source: !viewPass.checked ? "qrc:/images/view_hide_icon_124813.svg" : "qrc:/images/view_show_icon_124811.svg"
                    enabled: editPass.checked
                    Material.accent: Material.Blue
                    onCheckedChanged: {
                        password.echoMode = !viewPass.checked ? TextInput.Normal : TextInput.Password
                    }
                }
                ToolButton {
                    id: editPass
                    checkable: true
                    icon.source: "qrc:/images/edit.svg"
                    Material.accent: Material.Blue
                    onCheckedChanged: {
                        password.enabled = editPass.checked
                        if(editPass.checked){
                           password.text = "";
                        }else{
                            password.text = "***"
                            viewPass.checked = false
                        }

                    }
                }
            }


            CheckBox{
                id: chSave
                text: "Запомнить"
                Material.accent: Material.Blue
            }
            CheckBox{
                id: chAutoConnect
                text: "Подключатся автоматически"
                Material.accent: Material.Blue
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
                        authPage.accept(userName.text, password.text);
                        txtError.text = ""
                        columnLayout.enabled = false;

                    }
                }

            }

            Label {
                id: txtError
                font.pointSize: 12
                color: "red"
                visible: true
                Layout.maximumWidth: columnLayout.width
                wrapMode: Text.WordWrap

            }
        }

        Component.onCompleted: {

            if(autoConnect)
                authPage.accept(userName.text, password.text);

        }
    }

}
