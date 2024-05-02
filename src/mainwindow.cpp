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
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("database.db");
        if (!db.open()) {
            QFile file("database.db");
            if (!file.open(QIODevice::ReadWrite)) {
                qDebug() << "Error: could not create database";
                return;
            }
            qDebug() << "Database created successfully.";
            file.close();
            db.close();
        }
        QSqlQuery query(db);
        if (!query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='jobs'")) {
            qDebug() << "Error checking table:" << query.lastError().text();
            return;
        }
        if (!query.next()) {
            qDebug() << "Table 'jobs' does not exist, creating...";
            if (!query.exec("CREATE TABLE jobs ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "job_title TEXT,"
                            "company TEXT,"
                            "status TEXT,"
                            "application_date TEXT,"
                            "url_email TEXT,"
                            "details TEXT"
                            ");")) {
                qDebug() << "Error creating table:" << query.lastError().text();
                return;
            }
            qDebug() << "Table 'jobs' created successfully!";
        }
        addMockData(db);
        QSqlTableModel *model = new QSqlTableModel(this, db);
        QStringList headers = {"ID", "Job Title", "Company", "Status", "Application Date", "URL/Email", "Details"};
        model->setTable("jobs");
        model->select();
        tableView->setModel(model);
        for (int n = 0; n < headers.length(); ++n) {
            model->setHeaderData(n, Qt::Horizontal, headers[n]);
        }
        tableView->horizontalHeader()->setStyleSheet(
            "QHeaderView::section { background-color: rgb(115, 161, 199);"
            "font-family: Nunito; font-size: 16px; }"
        );
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
        tableView->model()->sort(0, Qt::AscendingOrder);
        connect(tableView, &QTableView::doubleClicked, this, &MainWindow::onTableCellDoubleClicked);
        connect(ui->trayButton, &QPushButton::clicked, this, &MainWindow::onTrayButtonClicked);
        connect(detailsDialog->detailsSubmitButton, &QPushButton::clicked, this, &MainWindow::onDetailsSubmitButtonClicked);
        connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onSystemTrayIconActivated);
    }
}

bool databaseExists(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

void MainWindow::addMockData(QSqlDatabase db) {
    QSqlQuery countQuery(db);
    if (!countQuery.exec("SELECT COUNT(*) FROM jobs")) {
        qDebug() << "Error checking table emptiness:" << countQuery.lastError().text();
        return;
    }
    countQuery.next();
    int rowCount = countQuery.value(0).toInt();
    if (rowCount == 0) {
        qDebug() << "Adding mock data...";
        QSqlQuery insertQuery(db);
        if (!insertQuery.exec("INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
                              "VALUES ('Software Developer', 'BIM', 'Applied', '28/04/2024', 'https://bim.com.br', "
                              "'Nam sit amet ligula lacus. Etiam vitae risus sit amet odio eleifend cursus sit amet ac eros."
                              " Vivamus elementum aliquam lorem, eu faucibus nulla scelerisque et. Sed ante odio, maximus "
                              "dignissim ullamcorper vitae, aliquet eu turpis. Phasellus auctor nunc vel velit feugiat convallis."
                              " Morbi facilisis iaculis velit, et tristique velit lobortis vel. Sed ipsum nunc, euismod vitae nunc"
                              " in, venenatis aliquam justo. Curabitur hendrerit, libero nec accumsan hendrerit, turpis metus "
                              "imperdiet purus, eu pulvinar lorem lectus vitae ex. Quisque dapibus maximus congue.')")) {
            qDebug() << insertQuery.lastError().text();
            return;
        }
        if (!insertQuery.exec("INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
                              "VALUES ('Python Developer', 'BAM', 'Finished', '25/04/2024', 'https://bam.com.br', "
                              "'Maecenas tempus interdum ante, quis tincidunt mi vestibulum sed.')")) {
            qDebug() << insertQuery.lastError().text();
            return;
        }
        if (!insertQuery.exec("INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
                              "VALUES ('Data Analyst', 'BEM', 'Technical Test', '13/04/2024', 'em@bem.br', "
                              "'-')")) {
            qDebug() << insertQuery.lastError().text();
            return;
        }
        if (!insertQuery.exec("INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
                              "VALUES ('C++ Developer', 'BOM', 'HR Interview (30/04)', '27/04/2024', 'https://bom.com.br', "
                              "'Lorem ipsum dolor sit amet, consectetur adipiscing elit.')")) {
            qDebug() << insertQuery.lastError().text();
            return;
        }
        qDebug() << "Mock data added successfully!";
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
    detailsDialog->idLineEdit->setText(QString::number(index.row()));
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

    qDebug() << "onTrayButtonClicked!";
    hide();
    trayIcon->show();
}

void MainWindow::onDetailsSubmitButtonClicked() {
    bool ok;
    int id = detailsDialog->idLineEdit->text().toInt(&ok);
    if (!ok) {
        QMessageBox::critical(detailsDialog, "Error", "Invalid ID");
        detailsDialog->close();
        return;
    }
    QString title = detailsDialog->titleLineEdit->text();
    QString company = detailsDialog->companyLineEdit->text();
    QString status = detailsDialog->statusLineEdit->text();
    QString appDate = detailsDialog->appDateLineEdit->text();
    QString url = detailsDialog->urlLineEdit->text();
    QString details = detailsDialog->detailsTextEdit->toPlainText();
    model->setItem(id, 0, new QStandardItem(title));
    model->setItem(id, 1, new QStandardItem(company));
    model->setItem(id, 2, new QStandardItem(status));
    model->setItem(id, 3, new QStandardItem(appDate));
    model->setItem(id, 4, new QStandardItem(url));
    model->setItem(id, 5, new QStandardItem(details));
    if (detailsDialog) {
        detailsDialog->close();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
