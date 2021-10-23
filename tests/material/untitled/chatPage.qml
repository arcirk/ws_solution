import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15

Page{

    id: chatPage

    SplitView {
        id: splitView
        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle {
            id: treeChannes
            width: 200
            SplitView.minimumWidth: 100
            color: "lightblue"

            Text {
                text: "View 1"
                anchors.centerIn: parent
            }

            property bool stateVisible: true

            states: [
                State { when: treeChannes.stateVisible;
                        PropertyChanges{target: treeChannes; visible: true}
                        PropertyChanges {   target: treeChannes; opacity: 1.0    }

                },
                State { when: !treeChannes.stateVisible;
                        PropertyChanges{target: treeChannes; visible: false}
                        PropertyChanges {   target: treeChannes; opacity: 0.0    }

                }
            ]

            transitions: [
                Transition {
                    NumberAnimation { property: "opacity"; duration: 500}
                }

            ]


        }
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

                Rectangle{
                    SplitView.minimumWidth: 300
                    SplitView.maximumHeight: 400
                    SplitView.minimumHeight: 100
                    SplitView.fillWidth: true
                    color: "lightgray"
                    Text {
                        text: "Send message"
                        anchors.centerIn: parent
                    }
                }

            }

        }
        ListView {
            id: listActivesChannels
            width: 200
            SplitView.minimumWidth: 100

            property bool stateVisible: true

            states: [
                State { when: listActivesChannels.stateVisible;
                        PropertyChanges{target: listActivesChannels; visible: true}
                        PropertyChanges {   target: listActivesChannels; opacity: 1.0    }

                },
                State { when: !listActivesChannels.stateVisible;
                        PropertyChanges{target: listActivesChannels; visible: false}
                        PropertyChanges {   target: listActivesChannels; opacity: 0.0    }

                }
            ]

            transitions: [
                Transition {
                    NumberAnimation { property: "opacity"; duration: 500}
                }

            ]

        }

    }

    Connections{
        target: rootToolBar
        onChannelsVisible: function(checked){
           treeChannes.stateVisible = checked;
        }
        onActiveChannelsVisible: function(checked){
            listActivesChannels.visible = checked;
        }
    }

}
