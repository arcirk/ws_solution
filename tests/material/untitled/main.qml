import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true

    Material.theme: Material.Light

    menuBar: ToolBar {

        id: rootToolBar

        signal channelsVisible(real checked)
        signal activeChannelsVisible(real checked)

        RowLayout{
            width: parent.width
            ToolButton{
                id: btnChannels
                icon.source: "qrc:/images/tree.png"
                checkable: true
                checked: true
                onClicked: {
                    //treeChannes.visible = btnChannels.checked
                    rootToolBar.channelsVisible(btnChannels.checked)
                }
            }
            Label{
                Layout.fillWidth: true
               text: "Test Material"
            }
            ToolButton{
                id: btnActiveChannels
                anchors.right: parent.right
                text: "Активные подключения"
                checkable: true
                checked: true
                onClicked: {
                    //listActivesChannels.visible = btnActiveChannels.checked
                    rootToolBar.activeChannelsVisible(btnActiveChannels.checked)
                }
            }
        }
    }

    StackView{
        id: stackView
        anchors.fill: parent
        initialItem: "qrc:/authPage.qml"

    }


}
