import QtQuick
import QtQuick.Controls

StackView {
    id: stackView
    anchors.fill: parent
    initialItem: AuthPage {}

    Connections{
        target: wsClient
        onNextChatPage: function(pageName, presentation) {
//            if(wsClient.activePage === "ChatClient")
//                stackView.push("qrc:/qml/NextChatPage.qml", {})
//            else
                //stackView.pop();
            let m = stackView.find(function(item) {
                            return item.name === pageName;
                        });
            if(m !== null)
                stackView.pop(m);
            else
                stackView.push("qrc:/qml/ChatPage.qml", {pageName: pageName, inConversationWith: presentation})

                //stackView.replace("qrc:/qml/ChatPage.qml", {pageName: pageName, inConversationWith: presentation})
        }

        onSetPage:function(index){
//            stackView.pop(stackView.find(function(item) {
//                return item.name === pageName;
//            }));

//            stackView.pop(stackView.get(index), false);
            let m = stackView.get(index);
            if(m !== null)
                stackView.pop(m);


        }
    }
}
