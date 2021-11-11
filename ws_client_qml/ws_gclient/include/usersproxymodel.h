//
// Created by arcady on 10.11.2021.
//

#ifndef WS_SOLUTION_USERSPROXYMODEL_H
#define WS_SOLUTION_USERSPROXYMODEL_H

#include <QSortFilterProxyModel>

class UsersModel;
class UsersProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
    //Q_PROPERTY(UsersModel* sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)
    //Q_PROPERTY(QString filer READ filer WRITE setFiler NOTIFY filerChanged)
public:
    explicit UsersProxyModel(QObject* parent = 0);
    bool filterAcceptsRow(int source_row,
                          const QModelIndex &source_parent) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const;

    //UsersModel* sourceModel();
    //void setSourceModel(UsersModel* source);

//    QString filter();
//    void setFilter(const QString& fiterText);

public slots:
    void setRef(const QString& ref);
    void setIsGroup(int isGroup);

private:
    QString m_ref;
    int is_group;
    QMap<QString, QVariant> m_filter;
signals:
    //void sourceModelChanged();
    void filerChanged();
};

#endif //WS_SOLUTION_USERSPROXYMODEL_H
