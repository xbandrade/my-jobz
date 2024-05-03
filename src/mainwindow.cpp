#include "../headers/mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow) {
    showClipboardHint = true;
    trayIcon = new QSystemTrayIcon(this);
    QIcon icon(":/application.png");
    trayIcon->setIcon(icon);
    QMenu *menu = new QMenu(this);
    QAction *restoreAction = new QAction("Restore", this);
    menu->addAction(restoreAction);
    QAction *quitAction = new QAction("Quit", this);
    menu->addAction(quitAction);
    trayIcon->setContextMenu(menu);
    trayIcon->show();
    ui->setupUi(this);
    setWindowState(windowState() | Qt::WindowMaximized);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (layout) {
        layout->setAlignment(Qt::AlignCenter);
    }
    detailsDialog = new DetailsDialog(this);
    tableView = findChild<QTableView*>("tableView");
    if (tableView) {
        db = QSqlDatabase::addDatabase("QSQLITE");  // TODO: Separate into different files
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
        addMockData(db); // Test Only - Add mock entries if database is empty
        model = new QSqlTableModel(this, db);
        QStringList headers = {"ID", "Job Title", "Company", "Status", "Application Date", "URL/Email", "Details"};
        model->setTable("jobs");
        model->select();
        sortProxyModel = new DateSortProxyModel(this);
        sortProxyModel->setSourceModel(model);
        tableView->setModel(sortProxyModel);
        for (int n = 0; n < headers.length(); ++n) {
            model->setHeaderData(n, Qt::Horizontal, headers[n]);
        }
        tableView->horizontalHeader()->setStyleSheet(
            "QHeaderView::section { background-color: #495057;"
            "font-family: Nunito; font-size: 16px; color: #CED4DA; }"
        );
        tableView->resizeColumnToContents(0);
        tableView->verticalHeader()->setVisible(false);
        tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        tableView->setSortingEnabled(true);
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->model()->sort(0, Qt::DescendingOrder);
        // Signal/Slot connections
        connect(tableView, &QTableView::doubleClicked, this, &MainWindow::onTableCellDoubleClicked);
        connect(ui->trayButton, &QPushButton::clicked, this, &MainWindow::onTrayButtonClicked);
        connect(ui->addNewItemButton, &QPushButton::clicked, this, &MainWindow::onAddNewItemButtonClicked);
        connect(ui->duplicateButton, &QPushButton::clicked, this, &MainWindow::onDuplicateButtonClicked);
        connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
        connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
        connect(ui->searchLineEdit, &QLineEdit::returnPressed, ui->searchButton, &QPushButton::click);
        connect(ui->clipboardCheckBox, &QCheckBox::clicked, this, &MainWindow::onClipboardCheckBoxClicked);
        connect(detailsDialog->detailsSubmitButton, &QPushButton::clicked, this, &MainWindow::onDetailsSubmitButtonClicked);
        connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onSystemTrayIconActivated);
        connect(restoreAction, &QAction::triggered, this, &MainWindow::restoreWindow);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
        connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::onClipboardDataChanged);
        connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::onTrayMessageClicked);
    }
}

void MainWindow::addMockData(QSqlDatabase db) {
    QSqlQuery countQuery(db);
    if (!countQuery.exec("SELECT COUNT(*) FROM jobs")) {
        qDebug() << "Error checking table emptiness:" << countQuery.lastError().text();
        return;
    }
    countQuery.next();
    int rowCount = countQuery.value(0).toInt();
    QList<QString> queries;
    queries << "INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
               "VALUES ('Software Developer', 'BIM', 'Applied', '28/04/2024', 'https://bim.com.br', "
               "'Nam sit amet ligula lacus. Etiam vitae risus sit amet odio eleifend cursus sit amet ac eros."
               " Vivamus elementum aliquam lorem, eu faucibus nulla scelerisque et. Sed ante odio, maximus "
               "dignissim ullamcorper vitae, aliquet eu turpis. Phasellus auctor nunc vel velit feugiat convallis."
               " Morbi facilisis iaculis velit, et tristique velit lobortis vel. Sed ipsum nunc, euismod vitae nunc"
               " in, venenatis aliquam justo. Curabitur hendrerit, libero nec accumsan hendrerit, turpis metus "
               "imperdiet purus, eu pulvinar lorem lectus vitae ex. Quisque dapibus maximus congue.')"
            << "INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
               "VALUES ('Python Developer', 'BAM', 'Finished', '25/04/2024', 'https://bam.com.br', "
               "'Maecenas tempus interdum ante, quis tincidunt mi vestibulum sed.')"
            << "INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
               "VALUES ('Data Analyst', 'BEM', 'Technical Test', '13/04/2024', 'em@bem.br', "
               "'-')"
            << "INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
               "VALUES ('C++ Developer', 'BOM', 'HR Interview (30/04)', '27/04/2024', 'https://bom.com.br', "
               "'Lorem ipsum dolor sit amet, consectetur adipiscing elit.')";
    if (rowCount == 0) {
        QSqlQuery insertQuery(db);
        for (const auto& query : queries) {
            if (!insertQuery.exec(query)) {
                qDebug() << insertQuery.lastError().text();
                return;
            }
        }
        qDebug() << "Mock data added successfully!";
    }
}

void MainWindow::onTableCellDoubleClicked(const QModelIndex &index)
{
    QModelIndex sourceIndex = sortProxyModel->mapToSource(index);
    QModelIndex idIndex = model->index(sourceIndex.row(), 0);
    int id = model->data(idIndex).toInt();
    QSqlRecord record = model->record(sourceIndex.row());
    QString title = record.value("job_title").toString();
    QString company = record.value("company").toString();
    QString status = record.value("status").toString();
    QString appDate = record.value("application_date").toString();
    QString url = record.value("url_email").toString();
    QString details = record.value("details").toString();
    detailsDialog->idLineEdit->setText(QString::number(id));
    detailsDialog->titleLineEdit->setText(title);
    detailsDialog->companyLineEdit->setText(company);
    detailsDialog->statusLineEdit->setText(status);
    detailsDialog->appDateLineEdit->setText(appDate);
    detailsDialog->urlLineEdit->setText(url);
    detailsDialog->detailsTextEdit->setText(details);
    detailsDialog->idLineEdit->show();
    detailsDialog->idLabel->show();
    detailsDialog->titleLineEdit->setFocus();
    detailsDialog->exec();
}

void clearDialogFields(DetailsDialog *detailsDialog) {
    detailsDialog->idLineEdit->setText("");
    detailsDialog->titleLineEdit->setText("");
    detailsDialog->companyLineEdit->setText("");
    detailsDialog->statusLineEdit->setText("");
    detailsDialog->appDateLineEdit->setText("");
    detailsDialog->urlLineEdit->setText("");
    detailsDialog->detailsTextEdit->clear();
}


void MainWindow::onAddNewItemButtonClicked() {
    clearDialogFields(detailsDialog);
    detailsDialog->idLineEdit->hide();
    detailsDialog->idLabel->hide();
    detailsDialog->titleLineEdit->setFocus();
    detailsDialog->exec();
}

void MainWindow::onDeleteButtonClicked() {
    QModelIndex selectedIndex = tableView->selectionModel()->currentIndex();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select any cell of an item before deleting.");
        return;
    }
    QMessageBox::StandardButton confirmDelete = QMessageBox::question(
        this, "Confirmation", "Are you sure you want to delete this appointment?",
        QMessageBox::Yes | QMessageBox::No
    );
    if (confirmDelete != QMessageBox::Yes) {
        return;
    }
    QModelIndex sourceIndex = sortProxyModel->mapToSource(selectedIndex);
    int id = model->data(model->index(sourceIndex.row(), 0)).toInt();
    QSqlQuery query(db);
    query.prepare("DELETE FROM jobs WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        qDebug() << "Error:" << query.lastError().text();
        return;
    }
    model->select();
    QMessageBox::information(this, "Success", "The appointment has been deleted successfully.");
}

void MainWindow::onDuplicateButtonClicked() {
    QModelIndex selectedIndex = tableView->selectionModel()->currentIndex();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select any cell of an item before duplicating.");
        return;
    }
    QModelIndex sourceIndex = sortProxyModel->mapToSource(selectedIndex);
    QSqlRecord record = model->record(sourceIndex.row());
    QSqlQuery query(db);
    query.prepare("INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
                  "VALUES (:job_title, :company, :status, :application_date, :url_email, :details)");
    query.bindValue(":job_title", record.value("job_title"));
    query.bindValue(":company", record.value("company"));
    query.bindValue(":status", record.value("status"));
    query.bindValue(":application_date", record.value("application_date"));
    query.bindValue(":url_email", record.value("url_email"));
    query.bindValue(":details", record.value("details"));
    if (!query.exec()) {
        qDebug() << "Error: " << query.lastError().text();
        return;
    }
    model->select();
    QMessageBox::information(this, "Success", "The appointment has been duplicated successfully.");
}

void MainWindow::onSearchButtonClicked() {
    QString searchText = ui->searchLineEdit->text();
    if (searchText.isEmpty()) {
        model->setFilter("");
        return;
    }
    QString filter = QString("job_title LIKE '%%1%' OR company LIKE '%%1%' "
                             "OR status LIKE '%%1%' OR application_date LIKE '%%1%' "
                             "OR url_email LIKE '%%1%' OR details LIKE '%%1%'").arg(searchText);
    model->setFilter(filter);
}

void MainWindow::onDetailsSubmitButtonClicked() {
    QString idText = detailsDialog->idLineEdit->text();
    int id = idText.isEmpty() ? -1 : idText.toInt();
    bool isNewEntry = id < 0;
    QSqlQuery query(db);
    query.prepare(isNewEntry ?
                  "INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
                  "VALUES (:title, :company, :status, :appDate, :url, :details)" :
                  "UPDATE jobs SET job_title = :title, company = :company, status = :status, "
                  "application_date = :appDate, url_email = :url, details = :details "
                  "WHERE id = :id");
    query.bindValue(":title", detailsDialog->titleLineEdit->text());
    query.bindValue(":company", detailsDialog->companyLineEdit->text());
    query.bindValue(":status", detailsDialog->statusLineEdit->text());
    query.bindValue(":appDate", detailsDialog->appDateLineEdit->text());
    query.bindValue(":url", detailsDialog->urlLineEdit->text());
    query.bindValue(":details", detailsDialog->detailsTextEdit->toPlainText());
    if (!isNewEntry) {
        query.bindValue(":id", id);
    }
    if (!query.exec()) {
        QMessageBox::critical(detailsDialog, "Error", "Failed to update appointment");
        detailsDialog->close();
        return;
    }
    model->select();
    QMessageBox::information(detailsDialog, "Success",
        QString("Appointment ") + (isNewEntry ? "added" : "updated") + " successfully");
    detailsDialog->close();
}

void MainWindow::onClipboardCheckBoxClicked() {
    if (showClipboardHint && ui->clipboardCheckBox->isChecked()) {
        showClipboardHint = false;
        QMessageBox::information(detailsDialog, "Info",
                                 "This option allows the application to listen for changes on your clipboard\n"
                                 "When a URL or email is detected, click on the notification to add a new appointment");
    }
}

void MainWindow::onSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::DoubleClick) {
        restoreWindow();
    }
}

void MainWindow::onClipboardDataChanged() {
    QString clipboardText = QApplication::clipboard()->text();
    if (!ui->clipboardCheckBox->isChecked() || clipboardText == previousClipboard) {
        return;
    }
    previousClipboard = clipboardText;
    static QRegularExpression emailRegex(R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b)");
    static QRegularExpression urlRegex(R"(https?://\S+)");
    if (emailRegex.match(clipboardText).hasMatch() || urlRegex.match(clipboardText).hasMatch()) {
        trayIcon->showMessage("Clipboard Content", "URL or Email detected!\nClick here to add a new appointment",
            QSystemTrayIcon::Information, 3000);
    }
}

void MainWindow::onTrayMessageClicked() {
    restoreWindow();
    activateWindow();
    raise();
    QString clipboardText = QApplication::clipboard()->text();
    clearDialogFields(detailsDialog);
    detailsDialog->urlLineEdit->setText(clipboardText);
    // ... If domain is known, fill in other fields
    detailsDialog->idLineEdit->hide();
    detailsDialog->idLabel->hide();
    detailsDialog->titleLineEdit->setFocus();
    detailsDialog->exec();
}

void MainWindow::restoreWindow() {
    show();
}

void MainWindow::onTrayButtonClicked() {
    hide();
}

MainWindow::~MainWindow() {
    delete ui;
}
