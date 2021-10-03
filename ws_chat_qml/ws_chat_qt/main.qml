import QtQuick
import QtQuick.Controls
import UserMessages 1.0


ApplicationWindow {
    id: window
    width: 540
    height: 960
    visible: true

    StackView {
        id: stackView
        anchors.fill: parent
        initialItem: TreeGroupPage {}
    }
}
