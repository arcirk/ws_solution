import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import "qrc:/scripts.js" as Scripts

StackView{
    id: stack
    initialItem: ItemGroupPage {
         onSelectPage: function(index){
             let page = Scripts.createGroupPage(model.index, parent)
             stack.push(page)
         }
    }
    anchors.fill: parent



}
