import QtQuick 2.12
import QtQuick.Controls 2.12

Item {

    ListModel {
            id: dataModel

            ListElement {
                color: "orange"
                text: "first"
            }
            ListElement {
                color: "lightgreen"
                text: "second"
            }
            ListElement {
                color: "orchid"
                text: "third"
            }
            ListElement {
                color: "tomato"
                text: "fourth"
            }
        }

        ListView {
            id: view
            model: dataModel
            spacing: defMargin/2
            clip: true
            delegate: ListDelegate{
                text: model.text
                color: model.color
            }
        }


}
