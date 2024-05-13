#ifndef PROXY_MODEL_HPP
#define PROXY_MODEL_HPP

#include <QDate>
#include <QRegularExpression>
#include <QSortFilterProxyModel>

class SortProxyModel : public QSortFilterProxyModel {
public:
    SortProxyModel(QObject *parent = nullptr);
    int getCurrentPage();
    void setCurrentPage(int page);
    int getItemsPerPage();
    void setItemsPerPage(int items);
    int pageCount() const;
    void setHideFinished(bool hide);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    int currentPage;
    int itemsPerPage;
    bool hideFinishedCheckBox;
};

#endif // PROXY_MODEL_HPP
