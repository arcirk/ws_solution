import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page{

    id: authorizationForm
            //id: user_name
           width: parent.width
           height: parent.height

    Item {
        anchors.centerIn: parent
        id: item1
        property alias cancelButton: cancelButton
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
                height: 22
                Layout.fillWidth: true
                font.pointSize: 12
            }

            Text {
                id: passwordText
                text: qsTr("Пароль:")
                font.pointSize: 12
            }

            TextField {
                id: password
                width: 300
                height: 26
                Layout.fillWidth: true
                font.pointSize: 12
            }

            Item {
                Layout.fillHeight: true
            }

            RowLayout {
                id: rowLayout
                width: 100
                height: 100

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: cancelButton
                    width: 90
                    height: 30
                    //btnText: qsTr("Cancel")
                    text: qsTr("Отмена")
                    //btnBlue: false
                    onClicked: Qt.quit()
                }

                Button {
                    id: loginButton
                    width: 90
                    height: 30
                    //btnText: qsTr("Login")
                    text: qsTr("ОК")
                    //btnBlue: false
                    onClicked: authorizationForm.StackView.view.push("qrc:/GroupItems.qml", {})
                }
            }
        }
    }

}
