import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Pane{

    id: ctrlMessageList
//    background: Rectangle{
//        color: "green"
//    }
    property string uuid: "f3ccb2f2-d431-11e9-ab42-08606e7d17fa"
    property alias listModel: listView.model

    signal messageClick(string msg);

    ListView {
        id: listView
        objectName: "listMessages"
        anchors.fill: parent
//        Layout.fillWidth: true
//        Layout.fillHeight: true
        //Layout.topMargin: 110
        //Layout.margins: pane.leftPadding + messageField.leftPadding
        displayMarginBeginning: 40
        displayMarginEnd: 40
        verticalLayoutDirection: ListView.BottomToTop
        spacing: 12
        model: msgModel

        delegate: Column {
            anchors.right: sentByMe ? listView.contentItem.right : undefined
            spacing: 6

            readonly property bool sentByMe: model.FirstField !== uuid

            Row {
                id: messageRow
                spacing: 6
                anchors.right: sentByMe ? parent.right : undefined

                Image {
                    id: avatar

                    //source: !sentByMe ? "qrc:/" + model.author.replace(" ", "_") + ".png" : ""
                }
                IconPane{
                    id: messageText
                    name: model.message
                    menuDeleteDisable: sentByMe
                    width: Math.min(messageText.implicitWidth + 24,
                        listView.width - (!sentByMe ? avatar.width + messageRow.spacing : 0))
                    height: messageText.implicitHeight// + 24
                    ctrlPaddig: 10
                    //icon: "qrc:/user.png"

                    textColor: sentByMe ? "black" : "white"
                    Material.background: sentByMe ? "lightgrey" : "steelblue" //sentByMe ? Material.Grey : Material.Blue

                    onMenuTriggered: {
                        ctrlMessageList.messageClick(model.message)
                    }

                    onImageClick: {
                        console.debug("Image click")
                    }
                }
            }
            Label {
                id: timestampText
                text: Qt.formatDateTime(model.date, "d MMM hh:mm")
                color: "lightgrey"
                anchors.right: sentByMe ? parent.right : undefined
            }


        }

        ScrollBar.vertical: ScrollBar {}
    }

}
