#include "../headers/proxy_model.hpp"

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

bool DateSortProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    if (filterRegularExpression().pattern().isEmpty()) {
        return true;
    }
    for (int column = 0; column < sourceModel()->columnCount(); ++column) {
        QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
        QString data = sourceModel()->data(index).toString();
        if (data.contains(filterRegularExpression())) {
            return true;
        }
    }
    return false;
}
