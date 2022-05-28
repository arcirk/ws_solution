#include "registry.h"
#include <QDir>
#include <QTextCodec>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <fstream>

std::string Registry::exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
#ifdef Q_OS_WINDOWS
    FILE* pipe = _popen(cmd, "r");
#else
    FILE* pipe = popen(cmd, "r");
#endif
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
#ifdef Q_OS_WINDOWS
       _pclose(pipe);
#else
       pclose(pipe);
#endif
        throw;
    }
#ifdef Q_OS_WINDOWS
    _pclose(pipe);
#else
     pclose(pipe);
#endif
    return result;
}

std::string Registry::execute( std::string cmd )
{
    std::string file_name = "result.txt" ;
    std::system( ( cmd + " > " + file_name ).c_str() ) ; // redirect output to file

    // open file for input, return string containing characters in the file
    std::ifstream file(file_name) ;
    return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() } ;
}

Registry::Registry(QObject *parent)
    : QObject{parent}
{

}

QStringList Registry::currentUserContainers(const QString& sid)
{
    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys").arg(sid);
    QSettings reg = QSettings(root, QSettings::NativeFormat);
    return reg.childGroups();
}

//QString Registry::currentSID(const QString& name )
//{

//    //auto cmd = QProcess();
//    return "";
//}

//QString Registry::currentWindowsName()
//{
//    QProcess myProcess;

//      QString program;
//      program.append("powershell");
//      QStringList arguments;


//      arguments<<"WHOAMI";
//      QString commandLine = "WHOAMI /USER";
//        commandLine.append("\n");

//      myProcess.start(program);
//      myProcess.write(commandLine.toUtf8());

//      //myProcess.start(program, arguments);

//      if(!myProcess.waitForStarted(500)) //default wait time 30 sec
//      {
//          qDebug() << "Cannot execute:" << program;
//      }
//      myProcess.waitForFinished(500);
//      myProcess.setProcessChannelMode(QProcess::MergedChannels);

////      myProcess.start(program, arguments);
////      myProcess.waitForFinished();

////      QString strTemp = QString::fromLocal8Bit(myProcess.readAllStandardOutput());
////      qDebug() << strTemp;
//      if(myProcess.exitStatus() == QProcess::NormalExit
//         && myProcess.exitCode() == QProcess::NormalExit){;
//          QString r = QString::fromStdString(myProcess.readAllStandardOutput().toStdString());
//          qDebug() << qPrintable(r);
//          qDebug() << "--------------";
//          //QString r1(qPrintable(r));
//          //qDebug() << r;
//          r.replace("\r", "");
//          QStringList l = r.split("\n");
//          qDebug() << l[l.size() - 2].replace("\\", " ");

////          QTextStream stream(myProcess);
////          while (!stream.atEnd()) {
////          qDebug()<<stream.readLine();
////          }

//          //result = r.contains("active (running)") !=  0;
//      } else {
//          qDebug() << qPrintable(QString::fromStdString(myProcess.readAllStandardError().toStdString()));
//      }
//      myProcess.close();

//      return "";

//}

QStringList Registry::currentUser(CertUser * usr)
{
//    QProcess cmd;
//    QString program;
//    program.append("powershell");

//    QStringList lstResult;
//    try {
//        //QString commandLine = "chcp 1251 | WHOAMI /USER";
//        QString commandLine = "WHOAMI /USER";
//        commandLine.append("\n");

//        cmd.start(program);
//        cmd.write(commandLine.toUtf8());

//        if(!cmd.waitForStarted(500))
//        {
//            qCritical() << __FUNCTION__ << "Cannot execute:" << program;
//            cmd.close();
//            return lstResult;
//        }
//        cmd.waitForFinished(500);
//        cmd.setProcessChannelMode(QProcess::MergedChannels);

//        if(cmd.exitStatus() == QProcess::NormalExit
//           && cmd.exitCode() == QProcess::NormalExit){;
//            //QString r = QString::fromLocal8Bit(cmd.readAll());
//            QString r = QTextCodec::codecForName("CP866")->toUnicode(cmd.readAllStandardOutput());
//            //qDebug() << __FUNCTION__ << qPrintable(r);
//            r.replace("\r", "");
//            lstResult = r.split("\n");
//            if(lstResult.size() - 2 < 0){
//                qCritical() << __FUNCTION__ << "Cannot execute:" << program;
//                cmd.close();
//                return lstResult;
//            }
//            QString result = lstResult[lstResult.size() - 2];
//            result.replace("\\", " ");
//            QStringList mResult = result.split(" ");
//            usr->setDomain(mResult[0]);
//            usr->setName(mResult[1]);
//            usr->setSid(mResult[2]);
//        } else {
//            qCritical() << __FUNCTION__ << qPrintable(QString::fromStdString(cmd.readAllStandardError().toStdString()));
//        }
//    }  catch (std::exception& e) {
//        qCritical() << e.what();
//    }

//    cmd.close();

    QStringList lstResult;

    //QTextCodec *codec = QTextCodec::codecForName("CP866");
    QTextCodec *codec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForLocale(codec);

    std::string _usr = exec( "echo %username%" );
    std::string::iterator it = std::remove(_usr.begin(), _usr.end(), '\n');
    _usr.erase(it, _usr.end());
    usr->setName(QString::fromStdString(_usr));

    std::string whoami = "whoami /user";
    std::string result = execute( whoami );

//    std::string wmic =  "wmic useraccount where name='";
//    wmic.append(_usr);
//    wmic.append("' get sid");
//    std::string result = execute( wmic );


//    QByteArray encodedString = result.data();

    QTextDecoder *decoder = codec->makeDecoder(QTextCodec::IgnoreHeader);

    QString _result = QString::fromStdString(result);// codec->toUnicode(encodedString);
    qDebug() << qPrintable(decoder->toUnicode(result.data()));
    qDebug() << qPrintable(QString::fromUtf8(result.data()));
    qDebug() << qPrintable(QString::fromLocal8Bit(result.data()));
    qDebug() << qPrintable(codec->makeDecoder()->toUnicode(result.data(), strlen(result.data())));

   // QString __result = QString::fromUtf8(result.data(), result.size());// QTextCodec::codecForName("CP866")->toUnicode(QString::fromStdString(result).toLatin1());//
    //QString _result = QString::fromStdString(result);
    _result.replace("\\", "");
    _result.replace("\r\n", "");
    _result.replace(" ", "");
    _result.replace("SID", "SID ");

    lstResult = _result.split(" ");
    if(lstResult.size() > 1){
        usr->setDomain(lstResult[0]);
        //usr->setName(lstResult[1]);
        usr->setSid(lstResult[1]);
    }
    return lstResult;
}

bool Registry::deleteContainer(CertUser *usr, const QString &containerName)
{
    const QString root = QString("\\HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Crypto Pro\\Settings\\Users\\%1\\Keys").arg(usr->sid());
    QSettings settings(root, QSettings::NativeFormat);

    settings.remove(containerName);
    settings.sync();

    if(settings.status() != QSettings::NoError){
        return false;
    }

    return true;
}

QStringList Registry::localUsers()
{


    QStringList lstResult;
/*
    QProcess cmd;
    QString program;
    program.append("cmd");
    //QString commandLine = QString("net user");
    QString commandLine = QString("wmic useraccount get name");
    commandLine.append("\n");

    cmd.start(program);
    cmd.write(commandLine.toUtf8());

    if(!cmd.waitForStarted(1000))
    {
        qCritical() << __FUNCTION__ << "Cannot execute:" << program;
        cmd.close();
        return lstResult;
    }
    //cmd.waitForFinished(500);
    cmd.setProcessChannelMode(QProcess::MergedChannels);

    if(cmd.exitStatus() == QProcess::NormalExit
       && cmd.exitCode() == QProcess::NormalExit){;
        QString r = QTextCodec::codecForName("CP866")->toUnicode(cmd.readAllStandardOutput());
        r.replace("\r", "");
        lstResult = r.split("\n");
    } else {
        qDebug() << qPrintable(QString::fromStdString(cmd.readAllStandardError().toStdString()));
    }
    cmd.close();

    qDebug() << __FUNCTION__ << "---------------";

    QStringList result{};
    for (int i = lstResult.size()-2; i >= 0; i--) {
        if(lstResult[i].trimmed() == "Name")
            break;
        if(lstResult[i].trimmed().isEmpty())
            continue;
        result.append(lstResult[i].trimmed());
        qDebug() << lstResult[i];
    }*/
    return lstResult;
}

void Registry::usersFromRegistry(const QString rootNode, const QString &group, QStringList lst)
{
    QSettings settings = QSettings(QString(rootCertNode + "\\%1").arg(group), QSettings::NativeFormat);
    if(settings.value("ProfileImagePath").isValid()){
        QString home = QDir::homePath();

    }
}

QStringList Registry::outputCmd(const QString cmd)
{

    QStringList lstResult;

//    QProcess process;
//    process.start("cmd");
//    process.write(cmd.toUtf8());
//    //process.waitForFinished(-1);
//    if(!process.waitForStarted(500))
//    {
//        qCritical() << __FUNCTION__ << "Cannot execute: cmd";
//        process.close();
//        return lstResult;
//    }
//    process.waitForFinished(500);
//    process.setProcessChannelMode(QProcess::MergedChannels);

//    if(process.exitStatus() == QProcess::NormalExit
//       && process.exitCode() == QProcess::NormalExit){;
//        QString r = QTextCodec::codecForName("CP866")->toUnicode(process.readAllStandardOutput());
//        r.replace("\r", "");
//        lstResult = r.split("\n");
//    } else {
//        qDebug() << qPrintable(QString::fromStdString(process.readAllStandardError().toStdString()));
//    }
//    process.close();
////    QString out = QTextCodec::codecForName("CP866")->toUnicode(process.readAllStandardOutput());
////    process.close();
////    out.replace("\r", "");
////    lstResult = out.split("\n");
////    qDebug() << out;
    return lstResult;

}
