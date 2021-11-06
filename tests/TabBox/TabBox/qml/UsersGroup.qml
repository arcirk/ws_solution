import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import "qrc:/scripts/scripts.js" as Scripts

StackView{
    id: stack
    anchors.fill: parent

    property string theme: "Dark"
    Material.theme: stack.theme === "Light" ? Material.Light : Material.Dark

    initialItem: ItemGroupPage {

        theme: stack.theme
        onSelectPage: function(title){
             //let page = Scripts.createGroupPage(index, parent)
             stack.push("qrc:/qml/UsersList.qml", {
                            theme: stack.theme,
                            pageTitle: title
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
