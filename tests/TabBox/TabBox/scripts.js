function buttonClicked(buttonId)
{
    //console.debug(buttonId);
    if(buttonId !== "select"){
        let m = "<img width=\"30\" height=\"30\" align=\"middle\" src=\"qrc:/smail/"+buttonId+".svg\">"
        //let m = "<span>&#x"+buttonId+"</span>";
        msgBox.text = msgBox.text + m;
    }

}

function createButtons(parent){
    let arr = ["1F60A", "1F60B", "1F60C", "1F60D", "1F60E", "1F60F"
        , "1F610", "1F611", "1F612", "select"]
    for (let i=0; i< arr.length; ++i){
        var componentPane = Qt.createComponent("qrc:/RoundPane.qml")
        var pane = componentPane.createObject(parent)
        var component = Qt.createComponent("qrc:/IconPane.qml")
        var btn = component.createObject(pane, {buttonId: arr[i]})
        btn.icon = "qrc:/smail/" + arr[i] + ".svg"
        pane.Layout.alignment = Qt.AlignLeft
        pane.Layout.margins = 0
        btn.height = 48
        btn.width = 48
        btn.menuDisable = true
        btn.clicked.connect(buttonClicked)
    }
}
