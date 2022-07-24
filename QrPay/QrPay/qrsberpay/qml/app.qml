import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: appWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("Qr pay")

    property int request_NO_RESPONSE_IMAGE: 0;
    property int request_NO_RESPONSE_PDF: -1;
    property int request_NO_RESPONSE_JPEG: -2;
    property int request_NO_RESPONSE_DOCX: -3;

    property int request_EDIT_FILE_IMAGE: 42;
    property int request_EDIT_FILE_PDF: 44;
    property int request_EDIT_FILE_JPEG: 45;
    property int request_EDIT_FILE_DOCX: 46;

    property int request_VIEW_FILE_IMAGE: 22;
    property int request_VIEW_FILE_PDF: 21;
    property int request_VIEW_FILE_JPEG: 23;
    property int request_VIEW_FILE_DOCX: 24;

    property int request_SEND_FILE_IMAGE: 11;
    property int request_SEND_FILE_PDF: 10;
    property int request_SEND_FILE_JPEG: 12;
    property int request_SEND_FILE_DOCX: 13;

    property int index_PNG: 0
    property int index_JPEG: 1
    property int index_DOCX: 2
    property int index_PDF: 3

    property var theModel: ["Image (PNG)","Image (JPEG)", "Document (DOCX)", "PDF"]

    menuBar: ToolBar{
        id: maniToolBar
        width: parent.width
        //Material.background: myTheme === "Light" ? "#efebe9" : "#424242"

        RowLayout{

            Layout.fillWidth: true

            ToolButton{
                icon.source: "qrc:/img/share.png"
                onClicked: {
                    shareUtils.sendFile(copyFileFromAppDataIntoDocuments(request_NO_RESPONSE_IMAGE), "Поделиться", "image/png", request_NO_RESPONSE_IMAGE, appWindow.useAltImpl)
                }
            }
        }

    }

     StackView{
         id: mainStack
        anchors.fill: parent
        //width: parent.width
        initialItem: MainForm{
            id:mainForm
        }
     }

     function onShareEditDone(requestCode) {
         console.log ("share done: "+ requestCode)
         if(requestCode === request_EDIT_FILE_PDF || requestCode === request_EDIT_FILE_IMAGE) {
             popup.labelText = "Edit Done"
             popup.open()
             requestEditDone(requestCode)
             return
         }
         popup.labelText = "Done"
         popup.open()
     }
     Timer {
         id: delayDeleteTimer
         property int theRequestCode
         interval: 500
         repeat: false
         onTriggered: {
             requestCanceledOrViewDoneOrSendDone(theRequestCode)
         }
     }

     function onShareFinished(requestCode) {
         console.log ("share canceled: "+ requestCode)
         if(requestCode === request_VIEW_FILE_PDF || requestCode === request_VIEW_FILE_IMAGE || requestCode === request_VIEW_FILE_JPEG || requestCode === request_VIEW_FILE_DOCX) {
             popup.labelText = "View finished or canceled"
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         if(requestCode === request_EDIT_FILE_PDF || requestCode === request_EDIT_FILE_IMAGE || requestCode === request_EDIT_FILE_JPEG || requestCode === request_EDIT_FILE_DOCX) {
             popup.labelText = "Edit canceled"
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         // Attention using ACTION_SEND it could happen that the Result comes back too fast
         // and immediately deleting the file would cause that target app couldn't finish
         // copying or printing the file
         // workaround: use a Timer
         // curious: this problem only happens if going the JAVA way
         // it doesn't happen the JNI way
         if(requestCode === request_SEND_FILE_PDF || requestCode === request_SEND_FILE_IMAGE || requestCode === request_SEND_FILE_JPEG || requestCode === request_SEND_FILE_DOCX) {
             popup.labelText = "Sending File finished or canceled"
             popup.open()
             if(appWindow.useAltImpl) {
                 requestCanceledOrViewDoneOrSendDone(requestCode)
             } else {
                 delayDeleteTimer.theRequestCode = requestCode
                 delayDeleteTimer.start()
             }
             return
         }
         popup.labelText = "canceled"
         popup.open()
     }
     function onShareNoAppAvailable(requestCode) {
         console.log ("share no App available: "+ requestCode)
         if(requestCode === request_VIEW_FILE_PDF || requestCode === request_VIEW_FILE_IMAGE || requestCode === request_VIEW_FILE_JPEG || requestCode === request_VIEW_FILE_DOCX) {
             popup.labelText = "No App found (View File)"
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         if(requestCode === request_EDIT_FILE_PDF || requestCode === request_EDIT_FILE_IMAGE || requestCode === request_EDIT_FILE_JPEG || requestCode === request_EDIT_FILE_DOCX) {
             popup.labelText = "No App found (Edit File)"
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         if(requestCode === request_SEND_FILE_PDF || requestCode === request_SEND_FILE_IMAGE || requestCode === request_SEND_FILE_JPEG || requestCode === request_SEND_FILE_DOCX) {
             popup.labelText = "No App found (Send File)"
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         popup.labelText = "No App found"
         popup.open()
     }
     function onShareError(requestCode, message) {
         console.log ("share error: "+ requestCode + " / " + message)
         if(requestCode === request_VIEW_FILE_PDF || requestCode === request_VIEW_FILE_IMAGE || requestCode === request_VIEW_FILE_JPEG || requestCode === request_VIEW_FILE_DOCX) {
             popup.labelText = "(View File) " + message
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         if(requestCode === request_EDIT_FILE_PDF || requestCode === request_EDIT_FILE_IMAGE || requestCode === request_EDIT_FILE_JPEG || requestCode === request_EDIT_FILE_DOCX) {
             popup.labelText = "(Edit File) " + message
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         if(requestCode === request_SEND_FILE_PDF || requestCode === request_SEND_FILE_IMAGE || requestCode === request_SEND_FILE_JPEG || requestCode === request_SEND_FILE_DOCX) {
             popup.labelText = "(Send File) " + message
             popup.open()
             requestCanceledOrViewDoneOrSendDone(requestCode)
             return
         }
         popup.labelText = message
         popup.open()
     }

     function copyFileFromAppDataIntoDocuments(requestId) {
         return myApp.filePathDocumentsLocation(requestId)
     }

     // we must delete file from DOCUMENTS
     // edit canceled, view done, send done or no matching app found
     function requestCanceledOrViewDoneOrSendDone(requestId) {
         myApp.deleteFromDocumentsLocation(requestId)
     }
     // we must copy file back from DOCUMENTS into APP DATA and then delete from DOCUMENTS
     function requestEditDone(requestId) {
         myApp.updateFileFromDocumentsLocation(requestId)
     }

     function onNoDocumentsWorkLocation() {
         popup.labelText = qsTr("Cannot access external folders and files without checked permissions")
         popup.open()
     }

     function onFileReceivedAndSaved(url) {
         //onFileUrlReceived(url)
     }

     Popup {
         id: popup
         closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape
         x: 16
         y: 16
         implicitHeight: 160
         implicitWidth: appWindow.width * .9
         property alias labelText: popupLabel.text
         Column {
             anchors.right: parent.right
             anchors.left: parent.left
             spacing: 20
             Label {
                 id: popupLabel
                 topPadding: 8
                 leftPadding: 8
                 rightPadding: 8
                 width: parent.width
                 text: qsTr("Cannot copy to Documents work folder\nPlease check permissions\nThen restart the App")
                 wrapMode: Text.WrapAtWordBoundaryOrAnywhere
             }
             Button {
                 id: okButton
                 text: "OK"
                 onClicked: {
                     popup.close()
                 }
             } // okButton
         } // row button
     } // popup

     Connections {
         target: shareUtils
         onShareEditDone: appWindow.onShareEditDone(requestCode)
     }
     Connections {
         target: shareUtils
         onShareFinished: appWindow.onShareFinished(requestCode)
     }
     Connections {
         target: shareUtils
         onShareNoAppAvailable: appWindow.onShareNoAppAvailable(requestCode)
     }
     Connections {
         target: shareUtils
         onShareError: appWindow.onShareError(requestCode, message)
     }

     // noDocumentsWorkLocation
     Connections {
         target: myApp
         onNoDocumentsWorkLocation: appWindow.onNoDocumentsWorkLocation()
     }

     // called from outside
     Connections {
         target: shareUtils
         onFileUrlReceived: appWindow.onFileUrlReceived(url)
     }

     Connections {
         target: shareUtils
         onFileReceivedAndSaved: appWindow.onFileReceivedAndSaved(url)
     }

}
