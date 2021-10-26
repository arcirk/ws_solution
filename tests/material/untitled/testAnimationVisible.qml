import QtQuick 2.4
        import QtQuick.Window 2.0
        import QtQuick.Controls 1.2
        import QtQuick.Layouts 1.1

        Window {
        id: mainWindow
        width: 600
        height: 600
        visible: true

        ColumnLayout
        {
        width: parent ? parent.width : 200
        Label
        {
        Layout.fillWidth: true
        text: "<h2>MyApp</h2>"
        horizontalAlignment: Text.AlignHCenter
        }
        RowLayout
        {
        Layout.fillWidth: true
        Layout.fillHeight: true
        Label
        {
        Layout.fillWidth: true
        text: "Show hidden text"
        }
        Switch
        {
        id: someswitch
        checked: false
        }
        }
        Label
        {
        id: myText
        text: "dummy"
        wrapMode: Text.WordWrap
        clip: true
        Layout.fillWidth: true
        Layout.preferredHeight: 0

        states:
        [
        State
        {
        name: "visible"
        when: someswitch.checked
        PropertyChanges { target: myText; height: implicitHeight }
        }
        ]

        Behavior on height
        {
        NumberAnimation { duration: 100 }
        }
        }
        }
        }