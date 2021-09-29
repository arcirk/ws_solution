import QtQuick 2.4
//import QtQuick.Dialogs 1.2

ChatForm {
    id: chatForm
    width: 250
    height: 300

    function show(x){
        //calendar.selectedDate = x
        chatForm.open()
    }
}
