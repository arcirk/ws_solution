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

    void accept() override;

    QJsonObject resultObject() const;

    QString hash();

private slots:
    void on_btnPwdEdit_clicked();

    void on_btnPwdEdit_toggled(bool checked);

    void on_btnPwdView_toggled(bool checked);

private:
    Ui::DialogServerUser *ui;
    QJsonObject _obj;

};

#endif // DIALOGSERVERUSER_H
