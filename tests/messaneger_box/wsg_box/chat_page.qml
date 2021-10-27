import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15
//import QtQuick.Controls.Styles 1.4
import "qrc:/scripts/scripts.js" as Scripts

ColumnLayout{

    Material.theme: Material.Light

    anchors.fill: parent

    SplitView {
        id: splitView
        Layout.alignment: Qt.AlignCenter
        Layout.fillWidth: true
        anchors.bottom: smailPane.top
        anchors.top: parent.top

        orientation: Qt.Horizontal

        Rectangle {
            id: centerItem
            SplitView.minimumWidth: 300
            SplitView.fillWidth: true
            color: "lightgray"
//            Text {
//                text: "View 2"
//                anchors.centerIn: parent
//            }

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
                    SplitView.minimumHeight: 70
                    Layout.fillWidth: true

                    RowLayout {
                        width: parent.width
                        height: parent.height

                        ToolButton{
                            id: btnSmail
                            icon.source: "qrc:/images/images/smile-svgrepo-com.svg"
                            checkable: true
                            anchors.margins: 0
                            onCheckedChanged: {
                                if(btnSmail.checked){
                                    smailPane.state = "shown"
                                }else
                                    smailPane.state = "hidden"
                            }

                        }

                        TextArea {
                            id: messageField
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            placeholderText: qsTr("Введите сообщение")
                            wrapMode: TextArea.Wrap
                            background: Rectangle {color: "#FFFAD9"}
                            //anchors.margins: 0
                            //Layout.margins: 0

//                                style: TextAreaStyle {
////                                        textColor: "#333"
////                                        selectionColor: "steelblue"
////                                        selectedTextColor: "#eee"
//                                        backgroundColor: "#FFFAD9"
//                                    }

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
        Layout.maximumHeight: 70
        anchors.bottom: parent.bottom
        Material.background: Material.Grey
//        state: "hidden"

//        Pane {
//            anchors.fill: parent
//            ColumnLayout{
//                id: mainPane

//                Row{
//                    Component.onCompleted: Scripts.createButtons(mainPane)
//                }

//            }
//            ToolButton {
//                id: btnCloseSmilePane
//                anchors.top: parent.top
//                anchors.right: parent.right
//                text: "x"

//                onClicked: {
//                    btnSmail.checked = false
//                    smailPane.state = "hidden"
//                }

//            }

//       }

//        states: [
//            State {
//                name: "shown"
//                //when: btnSmail.checked
//                PropertyChanges { target: smailPane; height: 70 }
//            },
//            State {
//                name: "hidden"
//                //when: !btnSmail.checked
//                PropertyChanges { target: smailPane; height: 0 }
//            }
//        ]

//        transitions: [
//            Transition {
//                from: "hidden"
//                to: "shown"
//                NumberAnimation {
//                    target: smailPane
//                    property: "height"
//                    duration: 200
//                    easing.type: Easing.InOutQuad
//                    from: 0
//                    to: 70
//                }
//            },
//            Transition {
//                from: "shown"
//                to: "hidden"
//                NumberAnimation {
//                    target: smailPane
//                    property: "height"
//                    duration: 200
//                    easing.type: Easing.InOutQuad
//                    from: 70
//                    to: 0
//                }

//            }
//        ]

    }

}
