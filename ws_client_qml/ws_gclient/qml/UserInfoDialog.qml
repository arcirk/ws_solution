import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12

Dialog {
    id:usrInfo
    anchors.centerIn: parent

    property alias userName: lblUserName.text

    ColumnLayout{
        Label{
            id: lblUserName

        }
    }


}
