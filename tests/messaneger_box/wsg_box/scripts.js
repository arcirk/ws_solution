function createButtons(parent){
    var currObj = null
    let arr = ["1F60A", "1F60B", "1F60C", "1F60D", "1F60E", "1F60F"
        , "1F610", "1F611", "1F612", "1F613"]
    for (let i=0; i< arr.length; ++i){
        var componentPane = Qt.createComponent("qrc:/RoundPane.qml")
        var pane = componentPane.createObject(parent)
        var component = Qt.createComponent("qrc:/IconPane.qml")
        var btn = component.createObject(pane)
        btn.height = 48
        btn.width = 48
        btn.icon = "qrc:/images/images/smail/" + arr[i] + ".svg"
        if(currObj){
            pane.anchors.left = currObj.right
            //pane.left = currObj.right
        }
        pane.objectName = arr[i];
        pane.anchors.top = parent.top

        pane.anchors.leftMargin = 34
        currObj = pane;

    }
}
