#ifndef COMANNDLINEPARSER_H
#define COMANNDLINEPARSER_H

#include <QObject>
#include <QThread>
#include <QObject>

class ComanndLineParser : public QObject
{
    Q_OBJECT
public:
    explicit ComanndLineParser(const QString& resp, int cmd);

protected:

private:
    int _cmd;
    QString _resp;

public slots:
    void process(); 	/*  создает и запускает построитель отчетов */
    void stop();    	/*  останавливает построитель отчетов */


signals:
    void endParse(const QVariant& result, int cmd);
    void finished();

};

#endif // COMANNDLINEPARSER_H
