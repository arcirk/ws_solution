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


function createGroupPage(index, parent){

   let arr = new Array[5];
   arr[0] = ["Администрация", "Борисоглебский", "Хаданович", "Отдел продаж", "Борисова", "Отдел поставок", "Судаков"];
   arr[1] = ["Администрация", "Карелина", "Отдел продаж", "Менеджер", "Склад", "МТЗ"];
   arr[2] = ["Администрация", "Вертегова", "Отдел продаж", "Менеджер Чита", "Склад", "МТЗ"];
   arr[3] = ["Администрация", "Димиденко", "Отдел продаж", "Менеджер Улан-Удэ", "Склад", "МТЗ"];
   arr[4] = ["Администрация", "Шворень", "Отдел продаж", "Менеджер Хабаровск", "Склад", "МТЗ"];
   arr[5] = ["Администрация", "Былинкина", "Отдел продаж", "Менеджер Владивосток", "Склад", "МТЗ"];

   var componentPage = Qt.createComponent("qrc:/ItemGroupPage.qml")
   var page = componentPage.createObject(parent)

}