#ifndef PROXYMODEL_HPP
#define PROXYMODEL_HPP

#include <QSortFilterProxyModel>
#include <QDate>

class DateSortProxyModel : public QSortFilterProxyModel {
public:
    DateSortProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // PROXYMODEL_HPP
