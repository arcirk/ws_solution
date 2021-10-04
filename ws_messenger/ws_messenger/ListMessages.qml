import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Page {
    id: listMessages

    property string inConversationWith

    header: ToolBar {
        ToolButton {
            height: 25
            text: qsTr("<<Назад")
            anchors.left: parent.lieft
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            onClicked: listMessages.StackView.view.pop()
        }

        Label {
            id: pageTitle
            text: inConversationWith
            font.pixelSize: 20
            anchors.centerIn: parent
        }
    }

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: pane.leftPadding + messageField.leftPadding
            displayMarginBeginning: 40
            displayMarginEnd: 40
            verticalLayoutDirection: ListView.BottomToTop
            spacing: 12
            model: 10
            delegate: Row {
                readonly property bool sentByMe: index % 2 == 0

                anchors.right: sentByMe ? listView.contentItem.right : undefined
                spacing: 6

                Rectangle {
                    id: avatar
                    width: height
                    height: parent.height
                    color: "grey"
                    visible: !sentByMe
                }

                Rectangle {
                    width: 80
                    height: 40
                    color: sentByMe ? "lightgrey" : "steelblue"

                    Label {
                        anchors.centerIn: parent
                        text: index
                        color: sentByMe ? "black" : "white"
                    }
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
                    placeholderText: qsTr("Введите сообщение ...")
                    wrapMode: TextArea.Wrap
                }

                Button {
                    id: sendButton
                    text: qsTr("Отправить")
                    enabled: messageField.length > 0
                }
            }
        }
    }
}

