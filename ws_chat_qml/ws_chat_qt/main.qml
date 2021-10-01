import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import UserMessages 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ListView {
        implicitWidth: 250
        implicitHeight: 250
        clip: true

        model: UserMessagesModel{}

        delegate: RowLayout {
            //width: parent.width

            CheckBox {
                checked: model.done
                onClicked: model.done
            }
            TextField {
                text: model.sender
                onEditingFinished: model.sender = text
                Layout.fillWidth: true
            }
        }
    }
}
