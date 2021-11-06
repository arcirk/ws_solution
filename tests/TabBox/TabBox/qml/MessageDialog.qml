import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    //id: control
    visible: false
    title: "Blue sky dialog"
    anchors.centerIn: parent

    property string textMsg

    contentItem: Rectangle {
        color: "lightskyblue"
        implicitWidth: 400
        implicitHeight: 100
        Text {
            text: textMsg//"Hello blue sky!"
            color: "navy"
            anchors.centerIn: parent
            //wrapMode: TextArea.Wrap
            textFormat: Text.RichText

        }
    }
}
