#ifndef DIALOGCONTAINERINFO_H
#define DIALOGCONTAINERINFO_H

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class DialogContainerInfo;
}

class DialogContainerInfo : public QDialog
{
    Q_OBJECT

public:
    explicit DialogContainerInfo(const QJsonObject& info, const QString& name, QWidget *parent = nullptr);
    ~DialogContainerInfo();

private:
    Ui::DialogContainerInfo *ui;

    void loadInfo(const QJsonObject& info);

};

#endif // DIALOGCONTAINERINFO_H
