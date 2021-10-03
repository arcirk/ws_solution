#ifndef ARCUTILS_H
#define ARCUTILS_H

#include <QObject>
#include <iostream>
#include <string>
#include <sstream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>

class ArcUtils : public QObject
{
    Q_OBJECT
public:
    explicit ArcUtils(QObject *parent = nullptr);

    static QString base64_decode(const QString &s);

signals:

};

#endif // ARCUTILS_H
