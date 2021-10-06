import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root

    header: ToolBar {
        id: tBar
        ToolButton {
            height: 25
            text: qsTr("Выход")
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            onClicked: root.StackView.view.pop();
        }
        Label {
            text: qsTr("Филиалы")
            font.pixelSize: 20
            anchors.centerIn: parent
        }
    }

//    Row {
//        id: pathRow
//        anchors.top: tBar.bottom
//        anchors.left: parent.left
//        anchors.right: parent.right
//        height: 25

//    }

//    RowLayout {
//        id: pathRow
//        spacing: 10
//        anchors.horizontalCenter: parent.horizontalCenter
//        ToolButton {
//            property int counter: 1
//            text: "Add"
//            onClicked: {
//                var btn = buttonComponent.createObject(toolBar)
//                btn.tooltip = qsTr("Button #%1").arg(++counter)
//                toolBar.addButton(btn)
//                tableView.currentRow = toolBar.count - 1
//            }
//        }
//        ToolButton {
//            text: "Remove"
//            enabled: toolBar.count > 0 && tableView.currentRow !== -1
//            onClicked: {
//                toolBar.removeButton(toolBar.buttons[tableView.currentRow])
//            }
//        }
//        ToolButton {
//            text: "Clear"
//            enabled: toolBar.count > 0
//            onClicked: {
//                toolBar.clear()
//            }
//        }
//    }
    ListView {
        id: listView
        anchors.fill: parent
        topMargin: 48
        leftMargin: 48
        bottomMargin: 48
        rightMargin: 48
        spacing: 20
        model:  groupModel //["Ангарск", "Иркутск", "Улан-Удэ", "Чита", "Хабаровск", "Владивосток"]
        delegate: ItemDelegate {

            Rectangle
              {
                id: avatar
                color: Qt.rgba(Math.random(), Math.random(), Math.random(), 1);//random returns a random number from 0 to 1
                width: parent.height
                height: parent.height

                radius: width * 0.5;

                Label {
                    //id: _av
                    text: qsTr(model.text.substring(0,1)) //qsTr(modelData.substring(0,1))
                    color: "white"
                    anchors.centerIn: parent
                }
              }

            text: model.text//modelData
            width: listView.width - listView.leftMargin - listView.rightMargin
            onClicked: {
//                if(model.rowCount() === 0)
                   root.StackView.view.push("qrc:/ListMessages.qml", { inConversationWith: model.text })
//                else

                //groupModel.setFilter(model.uuid)

                //                    var btn = tabButton.createObject(ToolButton, {text: "ToolButton " + (++number)})
//                    tBar.bottomInset(btn);
                    //var btn = pathRow.createObject(ToolButton, {text: "ToolButton " + (++number)})

            }


            //height: avatar.implicitHeight
            leftPadding: avatar.implicitWidth + 32

        }
    }

}
