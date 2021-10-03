import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Page {
    id: root

    header: ToolBar {
        ToolButton {
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
        model:  ["Ангарск", "Иркутск", "Улан-Удэ", "Чита", "Хабаровск", "Владивосток"]
        delegate: ItemDelegate {
            Rectangle
              {
                color: "blue"

                width: parent.height
                height: parent.height

                radius: width * 0.5;

                //anchors.horizontalCenter: parent.horizontalCenter;
              }

            text: modelData
            width: listView.width - listView.leftMargin - listView.rightMargin
//            height: avatar.implicitHeight
//            leftPadding: avatar.implicitWidth + 32
            onClicked: root.StackView.view.push("qrc:/ListMessages.qml", { inConversationWith: modelData })

//            Image {
//                id: avatar
//                source: "qrc:/" + model.display.replace(" ", "_") + ".png"
//            }
        }
    }

}
