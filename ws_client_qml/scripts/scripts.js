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

//function getImageHtmlString(source, format, mime){

//    let base64 = wsClient.getImageSource(source, format);

//    var result = "";
//    if(base64.length > 0)
//        result = "<img src=\"data:" + mime + ";base64," + base64 + "\">";


//    return result
//}

//function isImage(source, format, mime){
//    // let format = source.toUpperCase().substr(-3, 3);
//    // let b = wsClient.isImage(source);
//    // let result = {}
//    // result["PNG"] = "PNG"
//    // result["JPG"] = "JPG"
//    // result["SVG"] = "SVG"
//    // result["JPEG"] = "JPEG"
//    //
//    // return result[source] !== undefined
//    let _format;
//    let _mime;
//    let result = wsClient.isImage(source, _format, _mime);
//    return result;
//}

