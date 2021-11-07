import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    //id: control
    visible: false
    title: "Сообщение"
    anchors.centerIn: parent

    property string textMsg

    contentItem: Flickable {
                    implicitWidth: 200
                    implicitHeight: 200
        //width: 200; height: 200
       contentWidth: 400; contentHeight: 400

//            Image { id: image; source: "bigImage.png" }
//        Pane.flickable: Pane{
//            //color: "lightskyblue"
//            implicitWidth: 400
//            implicitHeight: 400

            Text{
                id: textFld
                text: textMsg
                color: "navy"
                anchors.centerIn: parent
                //wrapMode: TextArea.Wrap
                textFormat: Text.RichText

//            }

        }



    }
    onTextMsgChanged: {
        console.log(textMsg)
    }
}
