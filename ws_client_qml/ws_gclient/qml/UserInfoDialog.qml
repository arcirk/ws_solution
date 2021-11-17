import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.12
import UserInfo 1.0

Dialog {
    id: usrInfo
    anchors.centerIn: parent

    property alias jsonText: usrInfoObj.jsonText
    property alias city: lblOrg.text
    property alias department: lblDepartment.text
    property alias userName: lblUserName.text

    UserInfo{
        id: usrInfoObj
        onParseJson: {
            usrInfo.city = usrInfoObj.city
            usrInfo.userName = usrInfoObj.userName
            usrInfo.department = usrInfoObj.department
        }
    }

    ColumnLayout{

        Label{
            Layout.fillWidth: true
            id: lblUserName
            //text: usrInfoObj.userName

        }

        Row{
            Layout.fillWidth: true
            Label{
                text: "Город: "
            }
            Label{
                id: lblOrg
                //text: usrInfoObj.organization
            }
        }
        Row{
            Layout.fillWidth: true
            Label{
                text: "Отдел: "
            }
            Label{
                id: lblDepartment;
                //text: usrInfoObj.department
            }
        }

    }


}
