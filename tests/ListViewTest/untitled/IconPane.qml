import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

RoundPane {
    id: control

    property alias name: txt.text
    property alias icon: image.source
    property alias textColor: txt.color
   // property string buttonId

    Material.elevation: 6
    radius: 3

    RowLayout{
        anchors.fill: parent
        Image {
            id: image
            sourceSize.height: parent.height
        }
        Text {
            id: txt;

            MouseArea{
                anchors.fill: parent
                onClicked: {
                    console.log("Click")
                }
                onEntered: {
                    control.Material.elevation = 1
                }
                onExited: {
                    control.Material.elevation = 6
                }
            }
        }

    }



}
