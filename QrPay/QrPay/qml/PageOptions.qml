import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page {

    id: page
    property string theme: "Dark"


    GridLayout{
        columns: 2
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.margins: 10
        columnSpacing: 20
        Text{
            //font.pixelSize: 20
            text: "Сервер:"
            color: page.theme === "Light" ? "#424242" : "#efebe9"
        }

        TextField{
            id: txtServer
            //font.pixelSize: 20
            text: "ws://<domainname>"
            color: page.theme === "Light" ? "#424242" : "#efebe9"
            Material.accent: Material.Blue
            Layout.fillWidth: true
        }

        Text{
            //font.pixelSize: 20
            text: "Пользователь:"
            color: page.theme === "Light" ? "#424242" : "#efebe9"
        }

        TextField{
            id: txtUser
            //font.pixelSize: 20
            text: "admin"
            color: page.theme === "Light" ? "#424242" : "#efebe9"
            Material.accent: Material.Blue
            Layout.fillWidth: true
        }

        Text{
            //font.pixelSize: 20
            text: "Пароль:"
            color: page.theme === "Light" ? "#424242" : "#efebe9"
        }

        RowLayout{
            Layout.fillWidth: true
            TextField{
                id: txtPass
                //font.pixelSize: 20
                text: "admin"
                color: page.theme === "Light" ? "#424242" : "#efebe9"

                echoMode: TextInput.Password
                Material.accent: Material.Blue
                placeholderText: qsTr("Пароль")
                enabled: pwdEditPass.checked
                Layout.fillWidth: true
            }
            ToolButton{
                id: pwdViev
                icon.source: "qrc:/img/viewPwd1.png"
                checkable: true
                enabled: pwdEditPass.checked
                Material.accent: Material.Blue
                Layout.alignment: Qt.AlignRight
                onCheckedChanged: {
                    pwdViev.icon.source = pwdViev.checked ? "qrc:/img/viewPwd.png" : "qrc:/img/viewPwd1.png"
                }
                onClicked: {
                    txtPass.echoMode = pwdViev.checked ? TextInput.Normal : TextInput.Password
                }
            }
            ToolButton{
                id: pwdEditPass
                icon.source: "qrc:/img/itemEdit.png"
                checkable: true
                Material.accent: Material.Blue
                Layout.alignment: Qt.AlignRight
                onCheckedChanged: {
                    txtPass.enabled = pwdEditPass.checked
                    if(pwdEditPass.checked){
                        if(txtPass.text === "***")
                            txtPass.text = "";
                    }else{
                        if(txtPass.text === "")
                            txtPass.text = "***"
                        pwdViev.checked = false
                    }

                }
            }
        }
    }

}
