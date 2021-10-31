import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12
import QJsonTableModel 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    //property QJsonTableModel msgModel

    property string uuid: "f3ccb2f2-d431-11e9-ab42-08606e7d17fa"

    //Material.theme: Material.Dark
    Material.theme: btnTheme.checked ? Material.Light : Material.Dark


    ItemMessageDialog {
        id: dialogMsg
    }


    ColumnLayout {
        anchors.fill: parent

        ToolBar{
            id: toolBar
            Layout.fillWidth: true

            ToolButton{
                id: btnTheme
                text: "..."
                checkable: true
            }
        }
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            //Layout.topMargin: 110
            Layout.margins: pane.leftPadding + messageField.leftPadding
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
                        //menuDisable: sentByMe
                        menuDeleteDisable: sentByMe
                        width: Math.min(messageText.implicitWidth + 24,
                            listView.width - (!sentByMe ? avatar.width + messageRow.spacing : 0))
                        height: messageText.implicitHeight// + 24

                        icon: "qrc:/user.png"

                        textColor: sentByMe ? "black" : "white"
                        Material.background: sentByMe ? "lightgrey" : "steelblue" //sentByMe ? Material.Grey : Material.Blue

                        onMenuTriggered: {
                            dialogMsg.textMsg = model.message
                            dialogMsg.open()
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

        Pane {
            id: pane
            Layout.fillWidth: true

            RowLayout {
                width: parent.width

                TextArea {
                    id: messageField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Compose message")
                    wrapMode: TextArea.Wrap
                    Material.accent: Material.Blue
                }

                Button {
                    //ToDo: эмитация вызова добавления сообщения
                    signal newMessage(string msg)
                    id: sendButton
                    objectName: "SendButton"
                    text: qsTr("Send")
                    enabled: messageField.length > 0
                    onClicked: {
                        listView.model.sendMessage(messageField.text)
                        sendButton.newMessage(messageField.text)
                        messageField.text = ""

                    }
                }
            }
        }
    }

}
