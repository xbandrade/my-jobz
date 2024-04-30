#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "detailsdialog.hpp"

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
    void onTableCellDoubleClicked(const QModelIndex &index);
    void restoreApp();
    void handleWindowStateChange(QEvent *event);
    void onTrayButtonClicked();
    void onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    QStandardItemModel *model;
    Ui::mainWindow *ui;
    QTableView *tableView;
    DetailsDialog *detailsDialog;
    QSystemTrayIcon *trayIcon;
};

#endif // MAINWINDOW_HPP