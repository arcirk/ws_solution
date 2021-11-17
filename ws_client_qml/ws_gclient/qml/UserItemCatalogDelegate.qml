import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import "qrc:/scripts/scripts.js" as Scripts
import QProxyModel 1.0
import QJsonTableModel 1.0

IconPane{

    id: itemUserDelegate
    state: hidden ? "hidden" : "shown"

    property QtObject usersList
    property bool hidden: true
    property int itemCount: 12 //количество элементов в дочернем списке
    property int ctrlHeight: 50 //высота контрола по умолчанию
    property int groupCount: 4 // количество элементов в главном скиске

    signal selectedChanged(QtObject item)

    width: parent.width
    name: model.SecondField
    uuid:{
        model.Ref

    }

    onClicked: {
            hidden = !hidden
            itemUserDelegate.checked = !hidden
            itemUserDelegate.selectedChanged(itemUserDelegate);
    }


    signal itemClick(string uuid, int isGroup, string name)
    onChildItemClick: function(uuid, isGroup, name) {
        itemUserDelegate.itemClick(uuid, isGroup, name)
    }

    chldrenList: true
    checkable: true
    //checked: model.selected
    ctrlPaddig: 10

    states: [
        State {
            name: "shown"
            PropertyChanges {
                target: itemUserDelegate;
                height: {
                    //Scripts.getActualHeight(usersList, itemUserDelegate.ctrlHeight, itemUserDelegate.itemCount)//
                        let h = itemUserDelegate.ctrlHeight*itemUserDelegate.itemCount
                        if(h < (usersList.height - (groupCount * ctrlHeight)))
                            h //textItem.implicitHeight * 3 //view.height - (textItem.implicitHeight * 2)
                        else
                           usersList.height - (groupCount * ctrlHeight)
                        }
            }
        },
        State {
            name: "hidden"
            PropertyChanges {
                target: itemUserDelegate;
                height: itemUserDelegate.implicitHeight - 10 }
        }
    ]

    transitions: [
        Transition {
            from: "hidden"
            to: "shown"
            NumberAnimation {
                target: itemUserDelegate
                property: "height"
                duration: 200
                easing.type: Easing.InOutQuad
                from: itemUserDelegate.implicitHeight - 10
                to: Scripts.getActualHeight(usersList, itemUserDelegate.ctrlHeight, itemUserDelegate.itemCount, itemUserDelegate.groupCount)
            }
        },
        Transition {
            from: "shown"
            to: "hidden"
            NumberAnimation {
                target: itemUserDelegate
                property: "height"
                duration: 200
                easing.type: Easing.InOutQuad
                from: Scripts.getActualHeight(usersList, itemUserDelegate.ctrlHeight, itemUserDelegate.itemCount, itemUserDelegate.groupCount)
                to: itemUserDelegate.implicitHeight - 10
            }

        }

    ]
}
