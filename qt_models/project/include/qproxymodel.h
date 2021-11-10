//
// Created by arcady on 10.11.2021.
//

#ifndef QJSONMODEL_QPROXYMODEL_H
#define QJSONMODEL_QPROXYMODEL_H
#include <QSortFilterProxyModel>

class QProxyModel : public QSortFilterProxyModel{
    Q_OBJECT
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    //Q_PROPERTY(QAbstractTableModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

public:
    explicit QProxyModel(QObject* parent= nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const  override;

    QString filter();
    void setFilter(const QString& filterText);

signals:
    void filterChanged();

private:
    QString m_FilterText;
    QMap<QString, QVariant> m_filter;

    void set_filter();

};


#endif //QJSONMODEL_QPROXYMODEL_H
