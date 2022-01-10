import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import QJsonTableModel 1.0
import QProxyModel 1.0

Page{

    id: departmentUsers

    property string theme: parent.theme

    property string pageTitle: "Пользователи подразделения"

    property QtObject mainPage

    //property alias filter: currentList.filter
    //property alias sourceModel: currentList.sourceModel

    function setPropertyModel(mainModel, uuid){
        currentList.sourceModel = mainModel
        currentList.filter = "{\"IsGroup\": 1,
                               \"Parent\": \"" + uuid + "\"}"
    }

    signal selectedUser(string uuid, string name);

    header: ToolBar{
        id: barButtoms
        Material.background: departmentUsers.theme === "Light" ? "#efebe9" : "#424242"

        ToolButton{
            id: btnPageBack
            icon.source: "qrc:/images/ic_back.svg"
            onClicked: departmentUsers.StackView.view.pop()
        }

        Text{
            anchors.centerIn: parent
            text: departmentUsers.pageTitle
            color: departmentUsers.theme === "Light" ? "#424242" : "#efebe9"
        }

    }

    QProxyModel{
        id: currentList

    }

    ListView {
        id: view

        focus: true
        anchors.margins: 10
        anchors.fill: parent
        spacing: 5
        model: currentList

        property QtObject selDelegate;



        delegate: UserItemCatalogDelegate{
                id: delegate
                usersList: departmentUsers
                theme: departmentUsers.theme
                textColor: departmentUsers.theme === "Light" ? "#424242" : "#efebe9"
                expandIcon:true


                onSelectedChanged: function(item){
                    if(view.selDelegate){
                        view.selDelegate.checked = false;
                        view.selDelegate.hidden = true;
                        view.selDelegate.expandState = "collapsed"
                    }
                    if(view.selDelegate !== item)
                        view.selDelegate = item
                    else{
                        view.selDelegate = null
                    }
                }
                onItemClick: function(uuid, isGroup, name){
                    //console.debug(uuid + " --- " + isGroup);
                    if(isGroup === 0)
                        departmentUsers.selectedUser(uuid, name);
                }

        }

    }

//    onThemeChanged: {
//        //btnPageBack.icon.source = departmentUsers.theme === "Dark" ? "qrc:/images/ic-back_97586_White.svg" : "qrc:/images/ic-back_97586.svg"
//        console.debug(btnPageBack.icon.source)
//    }

}
