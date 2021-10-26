function createButtons(parent){
    for (let i=0; i< 5; ++i){
        var btn = Qt.createQmlObject(IconPane, parent);
        btn.height = 48
        btn.width = 48
        btn.icon = "qrc:/images/images/smail/1F60A.svg"
        btn.anchors.left = smlOne.right
        btn.anchors.top = parent.top
        btn.anchors.leftMargin = 10
    }
}
