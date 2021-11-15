import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

Dialog {

    id:serverSettings
    visible: false
    title: "Настройки подключения"
    anchors.centerIn: parent

    property alias serverAdrr: srvAdress.text
    property alias serverPort: srvPort.value

    contentItem: Pane{

        ColumnLayout{
           Layout.fillWidth: true
           spacing: 6
            //Row{

                Label {
                    id: lblAdress
                    text: qsTr("Адрес:")
                    //anchors.centerIn: parent
                }

                TextArea{
                    id: srvAdress
                    Layout.fillWidth: true
                    placeholderText: qsTr("Адрес сервера")
                    Material.accent: Material.Blue
                }

            //}
            //Row{

                Label {
                    id: lblPort
                    text: qsTr("Порт:")
                    //anchors.centerIn: parent
                }

                SpinBox{
                    id: srvPort
                    from: 0
                    to: 9999
                    Layout.fillWidth: true
                    Material.accent: Material.Blue
                    editable: true

                }

            //}
        }

    }
}
