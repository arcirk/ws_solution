import QtQuick 2.12
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.12

RoundPane {
    id: control
    //property alias name: txt.text
    property alias icon: image.source
    Material.elevation: 6
    radius: 7

    RowLayout{
        anchors.fill: parent
        spacing: 0

        Image {
            id: image
            sourceSize.height: parent.height
            anchors.centerIn: parent
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
        }
//        Text {
//            id: txt;
//        }
    }
}
