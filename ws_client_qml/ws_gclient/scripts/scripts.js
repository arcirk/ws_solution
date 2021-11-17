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
        var componentPane = Qt.createComponent("qrc:/qml/RoundPane.qml")
        var pane = componentPane.createObject(parent)
        var component = Qt.createComponent("qrc:/qml/IconPane.qml")
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


function createGroupPage(index, parent){

   var componentPage = Qt.createComponent("qrc:/qml/ItemGroupPage.qml")
   var page = componentPage.createObject(parent)

   console.debug(index)

    return page;
}

function getActualHeight(item, actualHeight, itemCount, groupCount){
    let h = actualHeight*itemCount
    if(h < (item.height - (groupCount * actualHeight)))
        return h //textItem.implicitHeight * 3 //view.height - (textItem.implicitHeight * 2)
    else
       return item.height - (groupCount * actualHeight)
 }

function getImageHtmlString(source, img){

    img.source = source;

    if(img.heght > 400){
        img.sourceSize.height = 400
    }
    if(img.width > 400){
        img.sourceSize.width = 400
    }

    var result = "<img id=\"ImageMsg\" width=\""+ img.width +"\" height=\""+ img.height +"\" align=\"middle\" src=\""+source+"\">";

    return result
}

function isImage(source){
    let result = {}
    result["png"] = "png"
    result["jpg"] = "jpg"
    result["svg"] = "svg"
    result["jpeg"] = "jpeg"

    return result[source] !== undefined
}
