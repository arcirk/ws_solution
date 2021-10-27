import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15
import "qrc:/scripts.js" as Scripts

Page{

    ColumnLayout{

        Material.theme: Material.Light
        anchors.fill: parent

        Rectangle{
            id: chatBar
            anchors.top: parent.top
            anchors.bottom: smailPane.top
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "blue"

            SplitView{
                anchors.fill: parent
                orientation: Qt.Vertical
                Rectangle{
                    SplitView.minimumWidth: 300
                    SplitView.minimumHeight: 300
                    SplitView.fillHeight: true
                    SplitView.fillWidth: true
                    color: "red"
                    Pane {
                        anchors.centerIn: parent
                        width: 300
                        height: 200

                        Material.elevation: 8
                        Material.background: Material.Blue
                    }
                }

                Rectangle{
                    id: pane
                    SplitView.minimumWidth: 300
                    SplitView.maximumHeight: 400
                    SplitView.minimumHeight: 70
                    Layout.fillWidth: true

                    RowLayout {
                        anchors.fill: parent
                        ToolButton{
                            id: btnSmail
                            icon.source: "qrc:/image/smile-svgrepo-com.svg"
                            checkable: true
                            anchors.margins: 0
                            onCheckedChanged: {
                                if(btnSmail.checked){
                                    smailPane.state = "shown"
                                }else
                                    smailPane.state = "hidden"
                            }

                        }
                    }

                }

            }

        }

        Rectangle{
            id:smailPane
            height: 70
            Layout.fillWidth: true
            Layout.maximumHeight: 70
            anchors.bottom: parent.bottom
            state: "hidden"
            Pane {
                anchors.fill: parent
                Material.background: Material.Grey
                ColumnLayout{
                    id: mainPane
                    Layout.fillWidth: true

                   RowLayout{
                       Layout.fillWidth: true
                       Component.onCompleted: Scripts.createButtons(mainPane)
                       onClick: function(objName){

                       }
                   }

                }
                ToolButton {
                    id: btnCloseSmilePane
                    anchors.top: parent.top
                    anchors.right: parent.right
                    text: "x"

                    onClicked: {
                        btnSmail.checked = false
                        smailPane.state = "hidden"
                    }

                }

           }
            states: [
                State {
                    name: "shown"
                    //when: btnSmail.checked
                    PropertyChanges { target: smailPane; height: 70 }
                },
                State {
                    name: "hidden"
                    //when: !btnSmail.checked
                    PropertyChanges { target: smailPane; height: 0 }
                }
            ]
            transitions: [
                Transition {
                    from: "hidden"
                    to: "shown"
                    NumberAnimation {
                        target: smailPane
                        property: "height"
                        duration: 200
                        easing.type: Easing.InOutQuad
                        from: 0
                        to: 70
                    }
                },
                Transition {
                    from: "shown"
                    to: "hidden"
                    NumberAnimation {
                        target: smailPane
                        property: "height"
                        duration: 200
                        easing.type: Easing.InOutQuad
                        from: 70
                        to: 0
                    }

                }
            ]
        }

    }

    //При изменении размера происходит сброс height smailPane на 0, не знаю как исправить
    //пока этот костыль
    onWidthChanged: {
        //console.debug("onWidthChanged")
        if(btnSmail.checked){
            btnSmail.checked = false
            smailPane.state = "hidden"
        }
    }
    onHeaderChanged: {
        //console.debug("onWidthChanged")
        if(btnSmail.checked){
            btnSmail.checked = false
            smailPane.state = "hidden"
        }
    }
}



