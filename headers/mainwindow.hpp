#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

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
#include "detailsdialog.hpp"
#include "proxymodel.hpp"

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
    void onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onTrayButtonClicked();
    void onTableCellDoubleClicked(const QModelIndex &index);
    void onDetailsSubmitButtonClicked();
    void onAddNewItemButtonClicked();
    void onDuplicateButtonClicked();
    void onDeleteButtonClicked();
    void onSearchButtonClicked();
    void onClipboardDataChanged();

private:
    Ui::mainWindow *ui;
    QSqlDatabase db;
    QSqlTableModel *model;
    QTableView *tableView;
    DetailsDialog *detailsDialog;
    QSystemTrayIcon *trayIcon;
    DateSortProxyModel *sortProxyModel;
    void addMockData(QSqlDatabase db);
};

#endif // MAINWINDOW_HPP
