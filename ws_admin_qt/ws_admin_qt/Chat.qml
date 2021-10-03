import QtQuick
import QtQuick.Controls

Item {

//        ListView {
//            id: view
//            model: dataModel
//            //spacing: defMargin/2
//            anchors.fill: parent
//            clip: true
//            delegate: ListDelegate{
//                text: model.text
//                color: model.color
//            }
//        }

    id: window
    width: 540
    height: 960
    visible: true

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: GroupPage {}
    }

}
