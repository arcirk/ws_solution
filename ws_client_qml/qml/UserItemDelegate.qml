import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12
//import QtQuick.Window 2.15

RoundPane {
    id: control
    property alias text: txt.text
    property alias icon: image.source
    property alias ctrlPaddig: control.padding
    Material.elevation: 0
    radius: 1
    RowLayout{
        anchors.fill: parent
        Image {
            id: image
            sourceSize.height: parent.height
        }
        Text {
            id: txt;
            Layout.fillWidth: true
        }
    }
}
