import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import UserMessages 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Чат")

    header: Label {
        text: view.currentItem.title
        horizontalAlignment: Text.AlignHCenter
    }

    MenuBar {
        id: menuBar
        width: parent.width

        Menu {
            id: fileMenu
            title: qsTr("File")
            // ...
        }

        Menu {
            id: editMenu
            title: qsTr("&Edit")
            // ...
        }

        Menu {
            id: viewMenu
            title: qsTr("&View")
            // ...
        }

        Menu {
            id: helpMenu
            title: qsTr("&Help")
            // ...
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical
        anchors.topMargin: 30

        Item {
            SplitView.fillHeight: true

            ColumnLayout {

               anchors.fill: parent
               anchors.margins: 10
               spacing: 10

               ListView {

                   Layout.fillWidth: true
                   Layout.fillHeight: true
                   clip: true
                    ScrollBar.vertical: ScrollBar {}

                   model: UserMessagesModel{}

                   delegate: RowLayout {
                       width: parent.width

                       CheckBox {
                           checked: model.done
                           onClicked: model.done
                       }
                       TextField {
                           text: model.sender
                           onEditingFinished: model.sender = text
                           Layout.fillWidth: true
                       }
                   }
                }
            }
        }
        Item {
            SplitView.minimumHeight: 25
            SplitView.preferredHeight: 50
            SplitView.maximumHeight: 200
            //color: "lightgreen"
        }




//               }
//               RowLayout {
//                   SplitView.minimumHeight: 50
//                   id: buttonsRow
//                   Button {
//                       text: "Open dump file"
//                   }
//                   Button {
//                       text: "Copy raw data to clipboard"
//                   }
//               }
//            }
    }

    footer: TabBar {
        // ...
    }

}
