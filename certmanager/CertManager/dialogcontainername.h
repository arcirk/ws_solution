#ifndef DIALOGCONTAINERNAME_H
#define DIALOGCONTAINERNAME_H

#include <QDialog>

namespace Ui {
class DialogContainerName;
}

class DialogContainerName : public QDialog
{
    Q_OBJECT

public:
    explicit DialogContainerName(const QString& currName, QWidget *parent = nullptr);
    ~DialogContainerName();

    void accept() override;

    QString name() const;

    QString keyName();

private slots:

    void onCheckEncodeToggled(bool checked);

    void onContainerNameTextChanged(const QString &arg1);

private:
    Ui::DialogContainerName *ui;
    QString new_name;
    QString key_name;
    QString end_date;
    QString _name;
    QString fromBase64(const QString &value);
    bool isBase64(const QString &value);
    bool isCyrillic(const QString &source);
};

#endif // DIALOGCONTAINERNAME_H
