import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1

//import QtQuick.Controls.Material 2.15
//import QtQuick.Controls.Material.impl 2.15
//import QtQuick.Layouts 1.12

    FileDialog {

        function basename(str)
        {
            return (String(str).slice(String(str).lastIndexOf("/")+1))
        }

        signal selectFile(string fpath, string name)

        id:fds
        title: "Выберете файл"
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        //nameFilters: ["JSON File (* .json)"]
        onAccepted: {
            fds.selectFile(fds.file, basename(fds.file)) ;
            //console.log("You chose: " + fds.file);
        }

        onRejected: {
            labels.text = "";
            //console.log("Canceled");
            Qt.quit();
        }

    }

