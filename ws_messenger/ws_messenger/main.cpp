#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QQmlContext>

#include "groupsmodel.h"
#include "customproxymodel.h"

CustomProxyModel * getItemModel(){

    //auto model = new QStandardItemModel(6, 1);
    auto model = new GroupsModel();
    model->setRows();
    auto proxyModel = new CustomProxyModel();

//    QList<QString> list;
//    list << "Ангарск" << "Иркутск" << "Ангарск" << "Улан-Удэ" << "Чита" << "Хабаровск" << "Владивосток";

//    for (int i = 0; i < list.length() ; ++i) {
//        QStandardItem *itemVal = new QStandardItem(list[i]);
//        model->setItem(i, 1, itemVal);
//    }

    proxyModel->setSourceModel(model);

    return proxyModel;

}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    CustomProxyModel* model = getItemModel();
    model->setFilter("0");
//    model->setFilterFixedString("0");
//    model->setFilterRole(GroupsModel::Roles::ParentRole);

    QQmlContext  * context = engine.rootContext();
    context->setContextProperty("groupModel", model);

    return app.exec();
}
