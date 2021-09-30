#ifndef CHATBOXDIALOG_H
#define CHATBOXDIALOG_H

#include <QDialog>
#include <QQuickWidget>

namespace Ui {
class ChatBoxDialog;
}

class ChatBoxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatBoxDialog(QWidget *parent = nullptr);
    ~ChatBoxDialog();

private:
    Ui::ChatBoxDialog *ui;
    QQuickWidget* quickWidget;
};

#endif // CHATBOXDIALOG_H
