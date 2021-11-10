import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import QProxyModel 1.0

Page{

    id: mainGroupPage
    //anchors.fill: parent

    property QProxyModel pageModel
    property string theme: "Dark"

    signal selectPage(string title)

    header: ToolBar{
        anchors.left: mainGroupPage.left
        anchors.right: mainGroupPage.right
        Material.background: mainGroupPage.theme === "Light" ? "#efebe9" : "#424242"

//        ToolButton{
//            id: btnPageBack
//            text: "x"
//        }

        Text{
            anchors.centerIn: parent
            text: "Подразделения"
            color: mainGroupPage.theme === "Light" ? "#424242" : "#efebe9"
        }
    }

    ListView {
        id: listViewGroup

        focus: true
        currentIndex: -1
        anchors.fill: parent

        delegate: ItemDelegate {
            width: parent.width
            text: model.title
            highlighted: ListView.isCurrentItem

            onClicked: {
//                listViewGroup.currentIndex = index
//                stackView.push(model.source)
//                drawer.close()
                mainGroupPage.selectPage(model.title)

            }
        }
//        ListModel {
//            id: mainModel
//                    ListElement { title: "Ангарск"}
//                    ListElement { title: "Иркустк"}
//                    ListElement { title: "Чита"}
//                    ListElement { title: "Улан-уде"}
//        }
        model: mainGroupPage.pageModel // != null ? pageModel : mainModel

        ScrollIndicator.vertical: ScrollIndicator { }
    }
}

