#include "comanndlineparser.h"
#include <QRegularExpression>

ComanndLineParser::ComanndLineParser(const QString& resp, int cmd)
{
    _resp = resp;
    _cmd = cmd;
}

void ComanndLineParser::process()
{
    if(_resp.isEmpty()){
        emit finished();
        return;
    }

    switch (_cmd) {
        case 8:{
            qDebug() << " --  ComanndLineParser -- " << qPrintable(_resp);
            QString str = _resp;
            QStringList results;
            int lastIndex = 0;
            int count = 0;



            int j = 0;
            while ((j = str.indexOf("-------", j)) != -1) {

                if(lastIndex > 0){
                    results.append(str.mid(lastIndex, j));
                }

                lastIndex = j;
                count++;
                qDebug() << count << "Found '-------' tag at index position" << j;
                ++j;
            }

            emit endParse(results.join("\n"), _cmd);
            break;

        }default: {
            emit finished();
            return ;
        }
    }

//    if(_cmd == 8){ //csptestGetCertificates

////        int index = 1;
////        QString str = result;

//        QString str = _resp;
//            int j = 0;
//            while ((j = str.indexOf("-------", j)) != -1) {
//                qDebug() << "Found '-------' tag at index position" << j;
//                ++j;
//            }

//    }

}

void ComanndLineParser::stop() {

    //return ;
}
