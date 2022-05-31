#ifndef DIALOGSELECTDEVICE_H
#define DIALOGSELECTDEVICE_H

#include <QDialog>

namespace Ui {
class DialogSelectDevice;
}

class DialogSelectDevice : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectDevice(QWidget *parent = nullptr, const QString& tTitle = "Реестр пользователя");
    ~DialogSelectDevice();

    void accept() override;

    int currentSelection();

private slots:

private:
    Ui::DialogSelectDevice *ui;
    int _currentSelection;
};

#endif // DIALOGSELECTDEVICE_H
