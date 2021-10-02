import QtQuick 2.12
import QtQuick.Controls 2.12

Item {

        ListView {
            id: view
            model: dataModel
            //spacing: defMargin/2
            anchors.fill: parent
            clip: true
            delegate: ListDelegate{
                text: model.text
                color: model.color
            }
        }

        footer: TabBar {
            // ...
        }
}
