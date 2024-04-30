#include "../headers/mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow) {
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon(":/application.png");
    trayIcon->setIcon(icon);
    QMenu *menu = new QMenu(this);
    QAction *restoreAction = new QAction("Restore", this);
    connect(restoreAction, &QAction::triggered, this, &MainWindow::restoreApp);
    menu->addAction(restoreAction);
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    menu->addAction(quitAction);
    trayIcon->setContextMenu(menu);
    trayIcon->hide();

    ui->setupUi(this);
    setWindowState(windowState() | Qt::WindowMaximized);

    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (layout) {
        layout->setAlignment(Qt::AlignCenter);
    }
    detailsDialog = new DetailsDialog(this);

    // MOCK TABLE DATA
    tableView = findChild<QTableView*>("tableView");
    if (tableView) {
        model = new QStandardItemModel(4, 3, this);
        model->setHorizontalHeaderLabels({"Job Title", "Company", "Status", "Application Date", "URL/Email", "Details"});
        model->setItem(0, 0, new QStandardItem("Software Developer"));
        model->setItem(0, 1, new QStandardItem("BIM"));
        model->setItem(0, 2, new QStandardItem("Applied"));
        model->setItem(0, 3, new QStandardItem("28/04/2024"));
        model->setItem(0, 4, new QStandardItem("https://bim.com.br"));
        model->setItem(0, 5, new QStandardItem("Nam sit amet ligula lacus. Etiam vitae risus sit amet odio eleifend cursus sit amet ac eros. Vivamus elementum aliquam lorem, eu faucibus nulla scelerisque et. Sed ante odio, maximus dignissim ullamcorper vitae, aliquet eu turpis. Phasellus auctor nunc vel velit feugiat convallis. Morbi facilisis iaculis velit, et tristique velit lobortis vel. Sed ipsum nunc, euismod vitae nunc in, venenatis aliquam justo. Curabitur hendrerit, libero nec accumsan hendrerit, turpis metus imperdiet purus, eu pulvinar lorem lectus vitae ex. Quisque dapibus maximus congue."));

        model->setItem(1, 0, new QStandardItem("Python Developer"));
        model->setItem(1, 1, new QStandardItem("BAM"));
        model->setItem(1, 2, new QStandardItem("Finished"));
        model->setItem(1, 3, new QStandardItem("25/04/2024"));
        model->setItem(1, 4, new QStandardItem("https://bam.com.br"));
        model->setItem(1, 5, new QStandardItem("Maecenas tempus interdum ante, quis tincidunt mi vestibulum sed."));

        model->setItem(2, 0, new QStandardItem("Data Analyst"));
        model->setItem(2, 1, new QStandardItem("BEM"));
        model->setItem(2, 2, new QStandardItem("Technical Tests"));
        model->setItem(2, 3, new QStandardItem("13/04/2024"));
        model->setItem(2, 4, new QStandardItem("bem@bem.br"));
        model->setItem(2, 5, new QStandardItem("-"));

        model->setItem(3, 0, new QStandardItem("C++ Developer"));
        model->setItem(3, 1, new QStandardItem("BOM"));
        model->setItem(3, 2, new QStandardItem("HR Interview (30/04)"));
        model->setItem(3, 3, new QStandardItem("27/04/2024"));
        model->setItem(3, 4, new QStandardItem("https://bom.com.br"));
        model->setItem(3, 5, new QStandardItem("Lorem ipsum dolor sit amet, consectetur adipiscing elit."));

        tableView->horizontalHeader()->setStyleSheet(
            "QHeaderView::section { "
            "background-color: rgb(115, 161, 199);"
            "font-family: Nunito;"
            "font-size: 16px;"
            "}"
        );
        tableView->setModel(model);
        tableView->verticalHeader()->setVisible(false);
        tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        tableView->setStyleSheet(
            "QTableView { background-color: rgb(187, 209, 227); }"
            "QTableView::item:selected { background-color: rgb(102, 153, 153); }"
         );
        tableView->setSortingEnabled(true);
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

        connect(tableView, &QTableView::doubleClicked, this, &MainWindow::onTableCellDoubleClicked);
        connect(ui->trayButton, &QPushButton::clicked, this, &MainWindow::onTrayButtonClicked);
        connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onSystemTrayIconActivated);
    }
}

void MainWindow::onTableCellDoubleClicked(const QModelIndex &index)
{
    QString title = model->item(index.row(), 0)->text();
    QString company = model->item(index.row(), 1)->text();
    QString status = model->item(index.row(), 2)->text();
    QString appDate = model->item(index.row(), 3)->text();
    QString url = model->item(index.row(), 4)->text();
    QString details = model->item(index.row(), 5)->text();
    detailsDialog->idLineEdit->setText(QString::number(index.row() + 1));
    detailsDialog->titleLineEdit->setText(title);
    detailsDialog->companyLineEdit->setText(company);
    detailsDialog->statusLineEdit->setText(status);
    detailsDialog->appDateLineEdit->setText(appDate);
    detailsDialog->urlLineEdit->setText(url);
    detailsDialog->detailsTextEdit->setText(details);
    detailsDialog->exec();
}

void MainWindow::onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        restoreApp();
    }
}

void MainWindow::restoreApp() {
    show();
    trayIcon->hide();
}

void MainWindow::handleWindowStateChange(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *stateChangeEvent = static_cast<QWindowStateChangeEvent*>(event);
        if (stateChangeEvent->oldState() & Qt::WindowMinimized) {
            hide();
            trayIcon->show();
        }
    }
}

void MainWindow::onTrayButtonClicked() {
    hide();
    trayIcon->show();
}

MainWindow::~MainWindow() {
    delete ui;
}
