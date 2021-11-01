import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15

RoundPane {
    id: control
    padding: 0

    property alias name: txt.text
    property alias icon: image.source
    property alias textColor: txt.color
    property string buttonId
    property bool menuDisable: false
    property bool menuDeleteDisable: false
    property alias ctrlPaddig: control.padding
    property bool checkable: false
    property bool checked: false
    property string theme: "Dark"

    Material.elevation: {
        if(control.checkable)
            if(control.checked)
                1
            else
                6
        else
            6
    }

    radius: 3

    Material.background:{

        if(control.checked){
            if(control.theme == "Dark")
                "#424242"
            else
                "#efebe9"
        }else
            null
    }
    signal menuTriggered(string name)
    signal imageClick()
    signal clicked(string buttonId)


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
//                    if (control.checked)
//                        control.setChecket();
                    control.imageClick()
                    control.clicked(buttonId)
                }
                onEntered: {
                    if (!control.checkable)
                        control.Material.elevation = 1
                }
                onExited: {
                    if (!control.checkable)
                        control.Material.elevation = 6
                }
            }
        }

            Text {
                id: txt;
                textFormat: Text.RichText
                //wrapMode: TextArea.Wrap

                MouseArea{
                    id:mouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor;

                    onClicked: {
    //                    if (control.checked)
    //                        control.setChecket();
                        //console.log("Click")
                        control.clicked(buttonId)
                    }
                    onEntered: {
                        if(!control.checkable){
                            control.Material.elevation = 1
                        }
                    }
                    onExited: {
                        if(!control.checkable)
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
