import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root

    header: ToolBar {

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
//                   root.StackView.view.push("qrc:/ListMessages.qml", { inConversationWith: model.text })
//                else
                    model.setFiler(model.parent)
            }


            //height: avatar.implicitHeight
            leftPadding: avatar.implicitWidth + 32

        }
    }

}
