import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import QJsonTableModel 1.0
import QProxyModel 1.0

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


    MessageDialog {
        id: dialogMsg
        width: 400

    }

    property bool connectState: false

    property string tmpModelText


    menuBar: ToolBar{
        id: maniToolBar
        width: parent.width
        Material.background: myTheme === "Light" ? "#efebe9" : "#424242"
        RowLayout{
            width: parent.width
            ToolButton{
                icon.source: "qrc:/team.svg"
                enabled: mainForm.connectState ? true : false
                onClicked: {
                    drawer.open()
                }
            }

            Row{
                id: rightButtons
                Layout.alignment: Qt.AlignRight
                ToolButton{
                    id: btnTheme
                    icon.source: "qrc:/images/mbriwebsitetheme_99562.svg"
                    onClicked: {
                        mnuTheme.open()
                    }
                }
                ToolButton{
                    id: btnExit
                    //text: "Выход"
                    enabled: mainForm.connectState ? true : false
                    icon.source: "qrc:/images/exit.svg"
                    onClicked: {
                        //mnuTheme.open()
                       mainForm.connectState = false
                       mainStack.pop()
                    }
                }
            }


            Menu {
                id: mnuTheme
                x: rightButtons.x;
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

    StackView{
        id: mainStack
        anchors.fill: parent

        MainForm{
            id: mainChatBox
            theme: myTheme
            //visible: false

        }

        initialItem: AuthForm{

            onAccept: {
                mainForm.connectState = true
                mainStack.push(mainChatBox)
            }

        }
    }

    onWidthChanged: {
        //mainChatBox.parentSizeChange();
        //mainChatBox.smaileBoxVisible(false)
    }
    onHeightChanged: {
        //mainChatBox.parentSizeChange();
        //mainChatBox.smaileBoxVisible(false)
    }


    onConnectStateChanged: {

    }
}
