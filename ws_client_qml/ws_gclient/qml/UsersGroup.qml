import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import "qrc:/scripts/scripts.js" as Scripts
import QJsonTableModel 1.0
import QProxyModel 1.0

StackView{
    id: stack
    anchors.fill: parent

    //property QtObject catalogModel//: mainGroupList.ListModel

    property string jsonRespCatalog

    property string theme: "Dark"

    onJsonRespCatalogChanged: {
        //console.debug(jsonRespCatalog.length)
        catalogModel.jsonText = jsonRespCatalog;
    }

    QJsonTableModel{
        id: catalogModel;
        jsonText: jsonRespCatalog

    }

    QProxyModel{
        id: rootCatalogModel
        sourceModel: catalogModel
        filter: "{\"IsGroup\": 1,
                  \"Parent\": \"00000000-0000-0000-0000-000000000000\"}"
    }


    signal select(string uuid, string name)

   UsersList{

        id: userList
        visible: false
        //mainModel: catalogModel

        onSelectedUser: function(uuid, name){
            stack.select(uuid, name);
        }
   }

    initialItem: ItemGroupPage {
        id: mainGroupList
        theme: stack.theme
        rootModel: rootCatalogModel
        onSelectPage: function(title, uuid){
            userList.setPropertyModel(catalogModel, uuid)
            stack.push(userList, {
                           theme: stack.theme,
                           pageTitle: title,
                           visible: true
                       })
         }

    }

    pushEnter: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 0
            to:1
            duration: 200
        }
    }
    pushExit: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 1
            to:0
            duration: 200
        }
    }
    popEnter: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 0
            to:1
            duration: 200
        }
    }
    popExit: Transition {
        PropertyAnimation {
            property: "opacity"
            from: 1
            to:0
            duration: 200
        }
    }

    onThemeChanged: {
        stack.currentItem.theme = stack.theme;
    }

    onCurrentItemChanged: {
        stack.currentItem.theme = stack.theme;
    }

}
