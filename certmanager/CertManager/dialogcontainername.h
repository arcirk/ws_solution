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

private slots:
    void on_lineContainerName_textChanged(const QString &arg1);

private:
    Ui::DialogContainerName *ui;
};

#endif // DIALOGCONTAINERNAME_H
