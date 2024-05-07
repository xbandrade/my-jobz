#ifndef PROXY_MODEL_HPP
#define PROXY_MODEL_HPP

#include <QSortFilterProxyModel>
#include <QDate>
#include <QRegularExpression>


class SortProxyModel : public QSortFilterProxyModel {
public:
    SortProxyModel(QObject *parent = nullptr);
    void setCurrentPage(int page);
    int getCurrentPage();
    void setItemsPerPage(int items);
    int pageCount() const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    int currentPage;
    int itemsPerPage;
};

#endif // PROXY_MODEL_HPP
