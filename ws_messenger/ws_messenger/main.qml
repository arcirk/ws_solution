import QtQuick 2.15
import QtQuick.Controls

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Чат")

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: AuthorizationForm {}
    }

}
