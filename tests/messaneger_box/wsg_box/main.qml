import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.15
import QtQuick.Controls 2.15


ApplicationWindow {
    width: 640
    height: 480
    minimumHeight: 480
    minimumWidth: 640
    visible: true
    title: qsTr("Hello World")

    Material.theme: Material.Light

    StackView{
        id: stackView
        anchors.fill: parent
        initialItem: "qrc:/chat_page.qml"

    }
}
