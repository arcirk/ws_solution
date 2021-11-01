import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page{

    id: mainGroupPage
    anchors.fill: parent

    property QtObject pageModel

    signal selectPage(int index)

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
                mainGroupPage.selectPage(model.index.row)
            }
        }
        ListModel {
            id: mainModel
                    ListElement { title: "Ангарск"}
                    ListElement { title: "Иркустк"}
                    ListElement { title: "Чита"}
                    ListElement { title: "Улан-уде"}
        }
        model: pageModel != null ? pageModel : mainModel

        ScrollIndicator.vertical: ScrollIndicator { }
    }
}

