import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.15
import QProxyModel 1.0

RoundPane {
    id: control
    padding: 0

    property alias name: txt.text
    property alias icon: image.source
    property bool expandIcon: false
    property alias textColor: txt.color
    property string buttonId
    property bool menuDisable: false
    property bool menuDeleteDisable: false
    property alias ctrlPaddig: control.padding
    property bool checkable: false
    property bool checked: false
    property string theme: "Dark"
    property bool chldrenList: false
    property alias expandState: expandIconImage.state
    property string uuid
    property bool iconButton: false
    property int iconSize: 0

    QProxyModel{
        id: proxyModel

    }

    signal childItemClick(string uuid, int isGroup, string name);

    onUuidChanged: {
        //console.debug(uuid)
        proxyModel.sourceModel = catalogModel
        proxyModel.filter = "{\"IsGroup\": 0,
                  \"Parent\": \"" + uuid + "\"}"
    }

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

        if(control.checked && !control.chldrenList){
            if(control.theme == "Dark")
                "#424242"
            else
                "#efebe9"
        }else
            undefined
    }
    signal menuTriggered(string name)
    signal imageClick()
    signal clicked(string buttonId)

    function getControlHeight(){
        return colControl.height - (nameItem.height - (nameItem.padding*2))
    }


    ColumnLayout{
        id: colControl
        //anchors.fill: parent
        anchors.fill: parent
        spacing: 12

        RowLayout{
            id: rowControl
            //anchors.fill: parent
            Layout.fillWidth: true
            //height: 30
            Layout.alignment: Qt.AlignCenter
            Image {
                id: image
                sourceSize.height: control.iconSize === 0 ? 48 : control.iconSize //rowControl.implicitHeight * 2 //parent.height
                Layout.alignment: Qt.AlignCenter

                MouseArea {
                    //id: area
                    anchors.fill: parent;
                    //hoverEnabled: true;
                    //acceptedButtons: Qt.NoButton;
                    cursorShape: Qt.PointingHandCursor;

                    onClicked: {
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
//                Component.onCompleted: {
//                    if(control.iconButton){
//                        Layout.alignment = Qt.AlignCenter
//                    }
//                }
            }

            Text {
                id: txt;
                textFormat: Text.RichText
                Layout.fillWidth: true
                clip: true
                visible: !control.iconButton

                MouseArea{
                    id:mouseArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor;

                    onClicked: {
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
                            text: "??????????????"
                            onTriggered: {
                                control.menuTriggered("mnuOpen")
                            }
                        }
                        Action {
                            text: "?????????????????? ?? ????????????????????"
                            onTriggered: {
                                control.menuTriggered("mnuSaveAs")
                            }
                        }
                        Action { text: "????????????????????" }
                        Action { text: "??????????????????" }
                        Action { text: "??????????????"; enabled: !menuDeleteDisable; onTriggered: console.debug("delete")}
                    }

                }
            }

            Image {
                id: expandIconImage
                sourceSize.height: txt.implicitHeight
                Layout.alignment: Qt.AlignRight
                visible: control.expandIcon
                source: control.theme === "Dark" ? "qrc:/images/left_collapse_lite.svg" : "qrc:/images/left_collapse.svg"
                state: "collapsed"

                MouseArea {
                    //id: area
                    anchors.fill: parent;
                    //hoverEnabled: true;
                    //acceptedButtons: Qt.NoButton;
                    cursorShape: Qt.PointingHandCursor;

                    onClicked: {
    //                    if (control.checked)
    //                        control.setChecket();
    //                    control.imageClick()
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

                states: [
                    State {
                        name: "collapsed"
                        PropertyChanges {
                            target: expandIconImage;
                            rotation: 0
                        }
                    },
                    State {
                        name: "expanded"
                        PropertyChanges {
                            target: expandIconImage;
                            rotation: -90
                        }
                    }
                ]
                transitions: [
                    Transition {
                        from: "expanded"
                        to: "collapsed"
                        RotationAnimator {
                            target: expandIconImage;
                            from: -90;
                            to: 0;
                            duration: 200
                            running: true
                        }
                    },
                    Transition {
                        from: "collapsed"
                        to: "expanded"
                        RotationAnimator {
                            target: expandIconImage;
                            from: 0;
                            to: -90;
                            duration: 200
                            running: true
                        }

                    }

                ]

            }

        }

        Pane {
            id: nameItem
            visible: false
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 0

            ListView{
                id: childrenList
                displayMarginBeginning:  -30
                displayMarginEnd: -30
                currentIndex: -1
                anchors.fill: parent
                model: proxyModel
                spacing: 2
                delegate:
                    ItemDelegate {
                    id: itemChildTExt
                    text: qsTr(model.SecondField)
                    highlighted: ListView.isCurrentItem
                    width: nameItem.width - nameItem.leftPadding - itemChildTExt.rightPadding

    //                        Binding {
    //                                target: contentItem
    //                                property: "color"
    //                                value: "green"

    //                            }


                    onClicked: {
                        control.childItemClick(model.Ref, model.IsGroup, model.SecondField)
                    }
            }
                ScrollBar.vertical: ScrollBar {}

            }
        }

    }



    onChldrenListChanged: {
       nameItem.visible = control.chldrenList
    }

    onStateChanged: {
        if(control.expandIcon)
            expandIconImage.state = control.state === "shown" ? "expanded" : "collapsed"
    }

}
