import QtQuick
import QtQuick.Controls
import QtWebSockets

import arc.utils 1.0

ApplicationWindow {

    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    ArcUtils{
        id: utils
    }

    WebSocket {
        id: socket
        url: "ws://192.168.43.18:8080"
        onTextMessageReceived: {
            let strMessage = "";
            let vMsg = message.split("\n");
            if (vMsg.length > 1)
                strMessage = vMsg[0]
            else
                strMessage = message;

            strMessage = utils.base64_decode(strMessage);
            messageBox.text = messageBox.text + "\nReceived message: " + strMessage

        }
        onStatusChanged: if (socket.status == WebSocket.Error) {
                             console.log("Error: " + socket.errorString)
                         } else if (socket.status == WebSocket.Open) {
                             socket.sendTextMessage("Hello World")
                         } else if (socket.status == WebSocket.Closed) {
                             messageBox.text += "\nSocket closed"
                         }
        active: false
    }

    Text {
        id: messageBox
        text: socket.status == WebSocket.Open ? qsTr("Sending...") : qsTr("Welcome!")
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            socket.active = !socket.active
            //secureWebSocket.active =  !secureWebSocket.active;
            //Qt.quit();
        }
    }
}
