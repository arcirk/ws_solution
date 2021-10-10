import QtQuick
import QtQuick.Controls

StackView {
    id: stackView
    anchors.fill: parent
    initialItem: AuthPage {}

    function createPage(pageName, inConversationWith){
        let page = Qt.createComponent("qrc:/qml/ChatPage.qml");
        page.pageName = pageName;
        page.inConversationWith = inConversationWith;
        return page;
    }

    Connections{
        target: wsClient
        onNextChatPage: function(pageName, presentation) {
////            if(wsClient.activePage === "ChatClient")
////                stackView.push("qrc:/qml/NextChatPage.qml", {})
////            else
//                //stackView.pop();
//            let m = stackView.find(function(item) {
//                            return item.name === pageName;
//                        });
//            if(m !== null)
//                stackView.pop(m);
//            else
//                stackView.push("qrc:/qml/ChatPage.qml", {pageName: pageName, inConversationWith: presentation})

//                //stackView.replace("qrc:/qml/ChatPage.qml", {pageName: pageName, inConversationWith: presentation})

            stackView.push(createPage(), {pageName: pageName, inConversationWith: presentation})
            //stackView.replace(createPage(), stackView.currentItem)
        }



        onSetPage:function(index){
//            stackView.pop(stackView.find(function(item) {
//                return item.name === pageName;
//            }));

//            stackView.pop(stackView.get(index), false);
            let m = stackView.get(index);
            if(m !== null)
                stackView.pop(m);
            else
                stackView.push(createPage());


        }
    }
}
