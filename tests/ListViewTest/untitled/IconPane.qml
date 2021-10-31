import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15

RoundPane {
    id: control

    property alias name: txt.text
    property alias icon: image.source
    property alias textColor: txt.color
    property string buttonId
    property bool menuDisable: false
    property bool menuDeleteDisable: false

    Material.elevation: 6
    radius: 3

    signal menuTriggered(string name)
    signal imageClick()

    RowLayout{
        anchors.fill: parent
        Image {
            id: image
            sourceSize.height: parent.height
            MouseArea {
                //id: area
                anchors.fill: parent;
                //hoverEnabled: true;
                //acceptedButtons: Qt.NoButton;
                cursorShape: Qt.PointingHandCursor;

                onClicked: {
                    control.imageClick()
                }
            }
        }
        Text {
            id: txt;


            MouseArea{
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor;

                onClicked: {
                    //console.log("Click")
                }
                onEntered: {
                    control.Material.elevation = 1
                }
                onExited: {
                    control.Material.elevation = 6
                }
                onPressAndHold: {
                        if (!menuDisable)
                            contextMenu.popup()
                    }

                Menu {
                    id: contextMenu
                    Action {
                        text: "Открыть"
                        onTriggered: {
                            control.menuTriggered("mnuOpen")
                        }
                    }
                    Action { text: "Копировать" }
                    Action { text: "Переслать" }
                    Action { text: "Удалить"; enabled: !menuDeleteDisable; onTriggered: console.debug("delete")}

//                    MenuSeparator { }

//                    Menu {
//                        title: "Find/Replace"
//                        Action { text: "Find Next" }
//                        Action { text: "Find Previous" }
//                        Action { text: "Replace" }
//                    }
                }
            }
        }

    }

}
