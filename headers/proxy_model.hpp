#ifndef PROXY_MODEL_HPP
#define PROXY_MODEL_HPP

#include <QSortFilterProxyModel>
#include <QDate>
#include <QRegularExpression>


class DateSortProxyModel : public QSortFilterProxyModel {
public:
    DateSortProxyModel(QObject *parent = nullptr);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
};

#endif // PROXY_MODEL_HPP
