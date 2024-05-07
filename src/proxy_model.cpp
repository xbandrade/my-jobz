#include "../headers/proxy_model.hpp"

SortProxyModel::SortProxyModel(QObject *parent) : QSortFilterProxyModel(parent), currentPage(1), itemsPerPage(10) {
}

bool SortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
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

bool SortProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    int totalRows = sourceModel()->rowCount();
    int start = totalRows - currentPage * itemsPerPage;
    int end = start + itemsPerPage;
    return sourceRow >= start && sourceRow < end;
}


void SortProxyModel::setCurrentPage(int page) {
    if (page <= 0 || page > pageCount() || page == currentPage)
        return;
    currentPage = page;
    invalidate();
}

int SortProxyModel::getCurrentPage() {
    return currentPage;
}

void SortProxyModel::setItemsPerPage(int items) {
    if (items <= 0 || items == itemsPerPage)
        return;
    itemsPerPage = items;
    invalidate();
}

int SortProxyModel::pageCount() const {
    return (sourceModel()->rowCount() + itemsPerPage - 1) / itemsPerPage;
}

