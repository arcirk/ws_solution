import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.platform

FolderDialog{

    id: selFolder
    property string ref: ""
    property string token: ""
    property string fileName: ""

    signal selectFolder(string name)

    onAccepted: {
        selFolder.selectFolder(selFolder.folder) ;
        //console.log("You chose: " + fds.file);
    }

    onRejected: {
        //labels.text = "";
        //console.log("Canceled");
        //Qt.quit();
    }

}
