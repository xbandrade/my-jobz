#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QList>
#include <QString>
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <stdexcept>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QObject>
#include <QFile>
#include <QLineEdit>
#include <QPushButton>
#include <QClipboard>
#include <QRegularExpression>
#include <QActionGroup>
#include "details_dialog.hpp"
#include "proxy_model.hpp"
#include "database_manager.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
    class mainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void restoreWindow();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onTableCellDoubleClicked(const QModelIndex &index);
    void onDetailsSubmitButtonClicked();
    void onAddNewItemButtonClicked();
    void onDuplicateButtonClicked();
    void onDeleteButtonClicked();
    void onSearchButtonClicked();
    void onClipboardCheckBoxClicked();
    void onClipboardDataChanged();
    void onTrayMessageClicked();
    void onPrevPageButtonClicked();
    void onNextPageButtonClicked();
    void onItemsPerPageChanged();
    void exportToDB();
    void importFromDB();

protected:
    void changeEvent(QEvent *event);

private:
    Ui::mainWindow *ui;
    QSqlDatabase db;
    QSqlQueryModel *model;
    QTableView *tableView;
    DetailsDialog *detailsDialog;
    QSystemTrayIcon *trayIcon;
    SortProxyModel *sortProxyModel;
    QString previousClipboard;
    DatabaseManager* dbManager;
    bool showClipboardHint;
    void clearDialogFields();
};

#endif // MAIN_WINDOW_HPP
