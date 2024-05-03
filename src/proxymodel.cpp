#include "../headers/proxymodel.hpp"

DateSortProxyModel::DateSortProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {
}

bool DateSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    QVariant leftData = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);
    if (leftData.typeId() == QVariant::String && rightData.typeId() == QVariant::String) {
        QDate leftDate = QDate::fromString(leftData.toString(), "dd/MM/yyyy");
        QDate rightDate = QDate::fromString(rightData.toString(), "dd/MM/yyyy");
        if (leftDate.isValid() && rightDate.isValid()) {
            return leftDate < rightDate;
        }
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

