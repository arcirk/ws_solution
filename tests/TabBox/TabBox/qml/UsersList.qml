import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page{

    id: departmentUsers

    property string theme: "Dark"

    property string pageTitle: "Пользователи подразделения"

    property QtObject mainPage

    header: ToolBar{
        id: barButtoms
        Material.background: departmentUsers.theme === "Light" ? "#efebe9" : "#424242"

        ToolButton{
            id: btnPageBack
            icon.source: "qrc:/images/ic-back_97586.svg"
            onClicked: departmentUsers.StackView.view.pop()
        }

        Text{
            anchors.centerIn: parent
            text: departmentUsers.pageTitle
            color: departmentUsers.theme === "Light" ? "#424242" : "#efebe9"
        }

    }

    ListModel {
            id: dataModel
            ListElement {
                text: "Администрация"
                property bool isGroup: true
                property bool selected: true
            }
            ListElement {
                text: "Отдел продаж"
                property bool isGroup: false
                property bool selected: false
            }
            ListElement {
                text: "Склад"
                property bool isGroup: false
                property bool selected: false
            }
            ListElement {
                text: "Отдел поставок"
                property bool isGroup: false
                property bool selected: false
            }
//            ListElement {
//                text: "     Борисова"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Борисова"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Борисова"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Борисова"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "Отдел поставок"
//                property bool isGroup: true
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Шестакова"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Судаков"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Судаков"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Судаков"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Судаков"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Судаков"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Судаков"
//                property bool isGroup: false
//                property bool visible: true
//            }
//            ListElement {
//                text: "     Судаков"
//                property bool isGroup: false
//                property bool visible: true
//            }
        }
//        ListModel{
//            id: dataItemModel
//                        ListElement {
//                            text: "     Борисоглебский"
//                            property bool isGroup: false
//                            property bool visible: true
//                        }
//                        ListElement {
//                            text: "     Судаков"
//                            property bool isGroup: false
//                            property bool visible: true
//                        }
//        }

    ListView {
        id: view

        focus: true
        //currentIndex: -1
        anchors.margins: 10
        anchors.fill: parent
        spacing: 5
        model: dataModel

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

        }

    }

}
