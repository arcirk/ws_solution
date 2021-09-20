/********************************************************************************
** Form generated from reading UI file 'optionsdlg.ui'
**
** Created by: Qt User Interface Compiler version 6.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPTIONSDLG_H
#define UI_OPTIONSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_OptionsDlg
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *btnSelectDir;
    QDoubleSpinBox *ServerPort;
    QLineEdit *ServerHost;
    QLineEdit *ServerBinDir;
    QGroupBox *groupBox_2;
    QLabel *label_4;
    QLabel *label_5;
    QLineEdit *RootUser;
    QLineEdit *Password;
    QPushButton *editPwd;

    void setupUi(QDialog *OptionsDlg)
    {
        if (OptionsDlg->objectName().isEmpty())
            OptionsDlg->setObjectName(QString::fromUtf8("OptionsDlg"));
        OptionsDlg->resize(461, 299);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(OptionsDlg->sizePolicy().hasHeightForWidth());
        OptionsDlg->setSizePolicy(sizePolicy);
        buttonBox = new QDialogButtonBox(OptionsDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(15, 260, 436, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(OptionsDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 441, 136));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 30, 81, 17));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 55, 67, 17));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 85, 121, 17));
        btnSelectDir = new QPushButton(groupBox);
        btnSelectDir->setObjectName(QString::fromUtf8("btnSelectDir"));
        btnSelectDir->setGeometry(QRect(410, 85, 26, 26));
        ServerPort = new QDoubleSpinBox(groupBox);
        ServerPort->setObjectName(QString::fromUtf8("ServerPort"));
        ServerPort->setGeometry(QRect(140, 55, 65, 26));
        ServerPort->setDecimals(0);
        ServerPort->setMaximum(10000.000000000000000);
        ServerPort->setValue(8080.000000000000000);
        ServerHost = new QLineEdit(groupBox);
        ServerHost->setObjectName(QString::fromUtf8("ServerHost"));
        ServerHost->setGeometry(QRect(140, 25, 291, 25));
        ServerBinDir = new QLineEdit(groupBox);
        ServerBinDir->setObjectName(QString::fromUtf8("ServerBinDir"));
        ServerBinDir->setGeometry(QRect(140, 85, 266, 41));
        groupBox_2 = new QGroupBox(OptionsDlg);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 150, 441, 101));
        groupBox_2->setCheckable(false);
        groupBox_2->setChecked(false);
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setEnabled(true);
        label_4->setGeometry(QRect(10, 30, 121, 17));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setEnabled(true);
        label_5->setGeometry(QRect(10, 70, 67, 17));
        RootUser = new QLineEdit(groupBox_2);
        RootUser->setObjectName(QString::fromUtf8("RootUser"));
        RootUser->setEnabled(true);
        RootUser->setGeometry(QRect(140, 30, 291, 25));
        Password = new QLineEdit(groupBox_2);
        Password->setObjectName(QString::fromUtf8("Password"));
        Password->setEnabled(false);
        Password->setGeometry(QRect(140, 65, 266, 25));
        Password->setEchoMode(QLineEdit::Password);
        editPwd = new QPushButton(groupBox_2);
        editPwd->setObjectName(QString::fromUtf8("editPwd"));
        editPwd->setEnabled(true);
        editPwd->setGeometry(QRect(410, 65, 26, 25));
        editPwd->setCheckable(true);

        retranslateUi(OptionsDlg);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, OptionsDlg, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, OptionsDlg, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(OptionsDlg);
    } // setupUi

    void retranslateUi(QDialog *OptionsDlg)
    {
        OptionsDlg->setWindowTitle(QCoreApplication::translate("OptionsDlg", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\260", nullptr));
        groupBox->setTitle(QCoreApplication::translate("OptionsDlg", "\320\241\320\265\321\200\320\262\320\265\321\200", nullptr));
        label->setText(QCoreApplication::translate("OptionsDlg", "\320\245\320\276\321\201\321\202:", nullptr));
        label_2->setText(QCoreApplication::translate("OptionsDlg", "\320\237\320\276\321\200\321\202:", nullptr));
        label_3->setText(QCoreApplication::translate("OptionsDlg", "\320\240\320\260\320\261\320\276\321\207\320\270\320\271 \320\272\320\260\321\202\320\260\320\273\320\276\320\263:", nullptr));
        btnSelectDir->setText(QCoreApplication::translate("OptionsDlg", "...", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("OptionsDlg", "\320\220\320\264\320\274\320\270\320\275\320\270\321\201\321\202\321\200\320\260\321\202\320\276\321\200", nullptr));
        label_4->setText(QCoreApplication::translate("OptionsDlg", "\320\230\320\274\321\217:", nullptr));
        label_5->setText(QCoreApplication::translate("OptionsDlg", "\320\237\320\260\321\200\320\276\320\273\321\214:", nullptr));
        editPwd->setText(QCoreApplication::translate("OptionsDlg", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OptionsDlg: public Ui_OptionsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPTIONSDLG_H
