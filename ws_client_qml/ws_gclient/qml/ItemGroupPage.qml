import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page{

    id: mainGroupPage
    //anchors.fill: parent

   property alias rootModel: listViewGroup.model

    property string theme: "Dark"

    signal selectPage(string title, string uuid)

    property string jsonRespCatalog

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
            text: model.SecondField
            highlighted: ListView.isCurrentItem

            onClicked: {
//                listViewGroup.currentIndex = index
//                stackView.push(model.source)
//                drawer.close()
                //mainGroupPage.selectPage(model.title)
                mainGroupPage.selectPage(model.SecondField, model.Ref)

            }
        }
//        ListModel {
//            id: mainModel
//                    ListElement { title: "Ангарск"}
//                    ListElement { title: "Иркустк"}
//                    ListElement { title: "Чита"}
//                    ListElement { title: "Улан-уде"}
//        }
        //model: catalogModel.subdivisions;//mainModel //pageModel  != null ? pageModel : mainModel

        ScrollIndicator.vertical: ScrollIndicator { }
    }



    onJsonRespCatalogChanged: {

    }
}

