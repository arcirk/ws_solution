import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15

ColumnLayout{

    Material.theme: Material.Light

    SplitView {
        id: splitView
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true
        Layout.fillHeight: true

        //anchors.fill: parent
        //anchors.left: parent.left
        //anchors.right: parent.right
        //anchors.bottom: smailPane.top

        orientation: Qt.Horizontal

        Rectangle {
            id: centerItem
            SplitView.minimumWidth: 300
            SplitView.fillWidth: true
            color: "lightgray"
            Text {
                text: "View 2"
                anchors.centerIn: parent
            }
            SplitView {
                anchors.fill: parent
                orientation: Qt.Vertical

                Rectangle{
                    SplitView.minimumWidth: 300
                    SplitView.minimumHeight: 300
                    SplitView.fillHeight: true
                    SplitView.fillWidth: true
                    color: "lightgray"
                    Pane {
                        anchors.centerIn: parent
                        width: 300
                        height: 200

                        Material.elevation: 8
                        Material.background: Material.Blue
                    }
                }
                Pane {
                    id: pane
                    SplitView.minimumWidth: 300
                    SplitView.maximumHeight: 400
                    SplitView.minimumHeight: 100
                    Layout.fillWidth: true

                    RowLayout {
                        width: parent.width
                        height: parent.height

                        TextArea {
                            id: messageField
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            placeholderText: qsTr("Compose message")
                            wrapMode: TextArea.Wrap
                        }

                        Button {
                            id: sendButton
                            text: qsTr("Send")
                            enabled: messageField.length > 0
                            Material.elevation: 8
                        }
                    }
                }


            }

        }

    }
    Rectangle{
        id:smailPane
        Layout.fillWidth: true
        height: 70
        anchors.bottom: parent.bottom
        Pane {

            anchors.fill: parent
            Material.background: Material.Grey

//            Rectangle {
//                id: closePane
//                width: 24
//                height: 24
//                //icon.source: "qrc:/images/images/closePane.png"
//                anchors.right: parent.right
//                //text: "x"
//                //highlighted: true
//                Material.background: Material.Grey

//            }
            Text
            {
                id: itemTextID
                width: parent.width - 20
                text: "Это пример 🙂😉 I forgot!"
                renderType: Text.NativeRendering
                textFormat: TextEdit.RichText
                wrapMode: Text.WordWrap
                font.family: "Segoe UI Emoji"
                font.pixelSize: 12
                anchors.margins: 10
                anchors.left: parent.left
                anchors.top: parent.top
                color: "#101010"
                font.hintingPreference: Font.PreferNoHinting
            }
            ToolButton {
                anchors.top: parent.top
                anchors.right: parent.right
//                width: 24
//                height: 36

                //Material.elevation: 8
                //Material.background: Material.Grey
                text: "x"

            }
        }
    }
}
