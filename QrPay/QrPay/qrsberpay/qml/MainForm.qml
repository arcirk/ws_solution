import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Controls.Material.impl 2.15
import QtQuick.Layouts 1.12

Page {

    id: maniPage
    anchors.fill: parent
    padding: 10

    ColumnLayout {
       width: parent.width
       spacing: 2

       Rectangle {
           id: header
           height: 60
           //Layout.alignment: Qt.AlignTop
           Layout.fillWidth: true

           color: "blue"

           Text {
               id: textHeader
               padding: 4
               text: qsTr("<b>Оплата по счету №123 от 10.01.2022</b>")
               anchors.fill: parent
               fontSizeMode: Text.Fit
               font.pixelSize: 10000 // maximum height of the font
               minimumPixelSize: 8 // minimum height of the font
               horizontalAlignment: Text.AlignHCenter
               verticalAlignment: Text.AlignVCenter
               color: "white"
           }
       }

        Rectangle {
            id: desc

            Layout.fillWidth: true
            //Layout.fillHeight: true

            implicitHeight: textDesc.implicitHeight
            color: "white"
            //clip: true
           Text{
               id: textDesc
              anchors.fill: parent
              //height: parent.height
              padding: 5
              text: qsTr("<b>Уважаемый клиент!</b><br>Напоминаем о необходимости оплатить счет на сумму на сумму 306,13 рублей.</br>")
              wrapMode: Text.WordWrap
           }

        }
        Rectangle {
            id: bank
            Layout.fillWidth: true
            //anchors.top: desc.bottom

            color: "white"
            border.color: "red"
            border.width: 2

            implicitHeight: textBankDesc.implicitHeight
            Text{
               id: textBankDesc
               anchors.fill: parent
               //height: parent.height
               padding: 15
               textFormat: Text.RichText
               text: qsTr("<b>Банковские реквизиты для оплаты:</b><br></br>
                            <br>Общество с ограниченной ответственностью \"Гринда Далвест\"</br>
                            <br><table border=\"1\  width=\"100%\" cols=2>
                            <tr><td width=\"100\">ИНН/КПП:</td><td>3801129139/380101001</td></tr>
                            <tr><td width=\"100\">Банк:</td><td>БАЙКАЛЬСКИЙ БАНК ПАО СБЕРБАНК</td></tr>
                            <tr><td width=\"100\">Р/счет:</td><td>40702810818350003339</td></tr>
                            <tr><td width=\"100\">Кор/счет:</td><td>30101810900000000607</td></tr>
                            <tr><td width=\"100\">БИК:</td><td>042520607</td></tr></table></br>")
               wrapMode: Text.WordWrap
            }

        }
        Rectangle {
            width: 170
            height: 170
            border.color: "black"
            border.width: 2
            color: "white"

            Layout.alignment: Qt.AlignCenter

            Image {
                //id: {name}
                height: 160
                width: 160
                //Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                anchors.centerIn: parent
                source: "qrc:/img/exampleQr.png"
            }
        }

    }

}
