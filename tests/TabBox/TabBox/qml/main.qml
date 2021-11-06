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
    minimumWidth: 900

    property string myTheme: "Dark"
    Material.theme: myTheme === "Light" ? Material.Light : Material.Dark
    //property int theme: Material.theme


    MessageDialog {
        id: dialogMsg

    }
    //menuBar
    menuBar: ToolBar{
        id: maniToolBar
        //anchors.left: parent.left
        //anchors.right: parent.right
        width: parent.width
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"
        RowLayout{
            width: parent.width
            ToolButton{
                //text: "группы"
                icon.source: "qrc:/team.svg"
                onClicked: {
                    drawer.open()
                }
            }

            ToolButton{
                id: btnTheme
                //text: "Тема"
                icon.source: "qrc:/images/mbriwebsitetheme_99562.svg"
                //anchors.right: parent.right
                Layout.alignment: Qt.AlignRight

                onClicked: {
                    mnuTheme.open()
                }
            }
//        ToolButton{
//            text: "t"
//            onClicked: {
//                manFuter.height = maniToolBar.height
//            }
//        }

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
        width: 400 //Math.min(mainForm.width, mainForm.height) / 3 * 2
        height: mainForm.height
        //interactive: stackView.depth === 1

        UsersGroup{
            anchors.fill: parent
            theme: mainForm.myTheme


        }

    }


    MainForm{
        id: mainChatBox
        theme: myTheme

    }

    onWidthChanged: {
        //mainChatBox.parentSizeChange();
        //mainChatBox.smaileBoxVisible(false)
    }
    onHeightChanged: {
        //mainChatBox.parentSizeChange();
        //mainChatBox.smaileBoxVisible(false)
    }

//    footer: ToolBar{
//        id: manFuter
//        ToolButton{
//            id: test
//            text: "x"
//            onClicked: {
//                manFuter.height = 0
//            }
//        }
//    }
}
