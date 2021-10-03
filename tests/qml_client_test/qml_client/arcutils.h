#ifndef ARCUTILS_H
#define ARCUTILS_H

#include <QObject>

class ArcUtils : public QObject
{
    Q_OBJECT
public:
    explicit ArcUtils(QObject *parent = nullptr);

    Q_INVOKABLE static QString base64_decode(const QString &s);

signals:

};

#endif // ARCUTILS_H
