#include "dialogcontainerinfo.h"
#include "ui_dialogcontainerinfo.h"
#include <QLabel>

DialogContainerInfo::DialogContainerInfo(const QJsonObject& info, const QString& name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogContainerInfo)
{
    ui->setupUi(this);

    ui->lblTitle->setText(name);
    loadInfo(info);
}

DialogContainerInfo::~DialogContainerInfo()
{
    delete ui;
}

void DialogContainerInfo::loadInfo(const QJsonObject &info)
{
    auto cnt = ui->gridLayout;
    int row = 0;
    foreach(auto itr, info.keys()){
        auto item = new QLabel();
        item->setText(itr + ":");
        cnt->addWidget(item, row, 0, Qt::AlignTop);
        auto val = info.value(itr);
        if (val.isObject()){
            auto lbl = new QLabel();
            QString str;
            auto obj = val.toObject();
            int i = 0;
            foreach(auto key , obj.keys()){
                QString n = i == 0 ? "" : "\n";
                str.append(n + key + ": " + obj.value(key).toString());
                i++;
            }
            lbl->setText(str);
            cnt->addWidget(lbl, row, 1);
        }else
        {
           auto lbl = new QLabel();
           lbl->setText(val.toString());
           cnt->addWidget(lbl, row, 1);
        }

        row++;
    }
}
