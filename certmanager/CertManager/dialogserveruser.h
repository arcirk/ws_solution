#ifndef DIALOGSERVERUSER_H
#define DIALOGSERVERUSER_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DialogServerUser;
}

class DialogServerUser : public QDialog
{
    Q_OBJECT

public:
    explicit DialogServerUser(const QJsonObject& object, const QString& parentName,  QWidget *parent = nullptr);
    ~DialogServerUser();

private:
    Ui::DialogServerUser *ui;
    QJsonObject _obj;

};

#endif // DIALOGSERVERUSER_H
