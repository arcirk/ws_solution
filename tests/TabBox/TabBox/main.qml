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

    property string myTheme: "Dark"
    Material.theme: myTheme === "Light" ? Material.Light : Material.Dark
    property int theme: Material.theme


    MessageDialog {
        id: dialogMsg

    }

    header: ToolBar{
        //Material.primary: Material.Light
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"
        Row{
        ToolButton{
            text: "группы"
            onClicked: {
                drawer.open()
            }
        }
        ToolButton{
            id: btnTheme
            text: "Тема"

            onClicked: {
                mnuTheme.open()
            }
        }

        Menu {
            id: mnuTheme
            x: btnTheme.x;
            y: btnTheme.y + btnTheme.height;
            Action {
                text: "Темная"
                onTriggered: {
                    mainForm.myTheme = "Dark"
                }
            }
            Action {
                text: "Светлая"
                onTriggered: {
                    mainForm.myTheme = "Light"
                }

            }
        }
        }
    }

    Drawer {

        id: drawer
        width: Math.min(mainForm.width, mainForm.height) / 3 * 2
        height: mainForm.height
        //interactive: stackView.depth === 1

        UsersGroup{
            anchors.fill: parent


        }

    }


    MainForm{
        id: mainChatBox
        theme: myTheme

    }

    onWidthChanged: {
//        if(msgBox.checked()){
//            msgBox.unchecked()
//        }
        mainChatBox.parentSizeChange();
        mainChatBox.smaileBoxVisible(false)
    }
    onHeightChanged: {
//        if(msgBox.checked()){
//            msgBox.unchecked()
//        }
        mainChatBox.parentSizeChange();
        mainChatBox.smaileBoxVisible(false)
    }

}
