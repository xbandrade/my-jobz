#include "../headers/main_window.hpp"
#include "./ui_main_window.h"

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
    QActionGroup *itemsPerPageGroup = new QActionGroup(this);
    itemsPerPageGroup->setExclusive(true);
    itemsPerPageGroup->addAction(ui->action10Items);
    itemsPerPageGroup->addAction(ui->action15Items);
    itemsPerPageGroup->addAction(ui->action20Items);
    itemsPerPageGroup->addAction(ui->action40Items);
    ui->action10Items->setData(10);
    ui->action15Items->setData(15);
    ui->action20Items->setData(20);
    ui->action40Items->setData(40);
    QActionGroup *setInitialDate = new QActionGroup(this);
    setInitialDate->setExclusive(true);
    setInitialDate->addAction(ui->actionToday);
    setInitialDate->addAction(ui->actionBlankDate);
    QActionGroup *setInitialStatus = new QActionGroup(this);
    setInitialStatus->setExclusive(true);
    setInitialStatus->addAction(ui->actionApplied);
    setInitialStatus->addAction(ui->actionBlankStatus);
    tableView = findChild<QTableView*>("tableView");
    if (tableView) {
        dbManager = new DatabaseManager(this);
        if (!dbManager->openDatabase("database.db")) {
            QMessageBox::critical(parent, "Error", "IO Error");
            return;
        }
        if (!dbManager->createJobsTableIfNotExists()) {
            QMessageBox::critical(parent, "Error", "Error creating table");
            return;
        }
        model = dbManager->getModel();
        model->setQuery("SELECT * FROM jobs");
        sortProxyModel = new SortProxyModel(this);
        sortProxyModel->setSourceModel(model);
        tableView->setModel(sortProxyModel);
        QStringList headers = {"ID", "Job Title", "Company", "Status", "Application Date", "URL/Email", "Details"};
        for (int col = 0; col < headers.length(); ++col) {
            model->setHeaderData(col, Qt::Horizontal, headers[col]);
        }
        tableView->horizontalHeader()->setStyleSheet(
            "QHeaderView::section { background-color: #495057; padding-top: 2px; padding-bottom: 2px; "
            "font-family: Nunito; font-size: 16px; color: #CED4DA; }"
        );
        tableView->setColumnHidden(7, true);
        tableView->resizeColumnToContents(0);
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        tableView->verticalHeader()->setVisible(false);
        tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        tableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        tableView->setSortingEnabled(true);
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->model()->sort(0, Qt::DescendingOrder);
        ui->prevPageButton->setEnabled(false);
        if (sortProxyModel->pageCount() <= 1) {
            ui->nextPageButton->setEnabled(false);
        }
    }
    // Signal/Slot connections
    connect(tableView, &QTableView::doubleClicked, this, &MainWindow::onTableCellDoubleClicked);
    connect(tableView, &QTableView::customContextMenuRequested, this, &MainWindow::onCustomContextMenuRequested);
    connect(ui->addNewItemButton, &QPushButton::clicked, this, &MainWindow::onAddNewItemButtonClicked);
    connect(ui->duplicateButton, &QPushButton::clicked, this, &MainWindow::onDuplicateButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    connect(ui->searchLineEdit, &QLineEdit::returnPressed, ui->searchButton, &QPushButton::click);
    connect(ui->clipboardCheckBox, &QCheckBox::clicked, this, &MainWindow::onClipboardCheckBoxClicked);
    connect(ui->prevPageButton, &QCheckBox::clicked, this, &MainWindow::onPrevPageButtonClicked);
    connect(ui->nextPageButton, &QCheckBox::clicked, this, &MainWindow::onNextPageButtonClicked);
    connect(detailsDialog->detailsSubmitButton, &QPushButton::clicked, this, &MainWindow::onDetailsSubmitButtonClicked);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayIconActivated);
    connect(restoreAction, &QAction::triggered, this, &MainWindow::restoreWindow);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::onClipboardDataChanged);
    connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::onTrayMessageClicked);
    connect(ui->actionExportToDB, &QAction::triggered, this, &MainWindow::onExportToDBTriggered);
    connect(ui->actionImportFromDB, &QAction::triggered, this, &MainWindow::onImportFromDBTriggered);
    connect(ui->actionExportToCSV, &QAction::triggered, this, &MainWindow::onExportToCSVTriggered);
    connect(ui->action10Items, &QAction::triggered, this, &MainWindow::onItemsPerPageChanged);
    connect(ui->action15Items, &QAction::triggered, this, &MainWindow::onItemsPerPageChanged);
    connect(ui->action20Items, &QAction::triggered, this, &MainWindow::onItemsPerPageChanged);
    connect(ui->action40Items, &QAction::triggered, this, &MainWindow::onItemsPerPageChanged);
    connect(ui->hideFinishedCheckBox, &QCheckBox::stateChanged, this, [=](int state){
        sortProxyModel->setHideFinished(state == Qt::Checked);
    });
    connect(ui->actionGithub, &QAction::triggered, this, [](){
        QDesktopServices::openUrl(QUrl("https://github.com/xbandrade/my-jobz"));
    });
    connect(ui->actionAbout, &QAction::triggered, this, [](){
        QMessageBox aboutBox;
        aboutBox.setWindowTitle("About MyJobz");
        aboutBox.setTextFormat(Qt::RichText);
        aboutBox.setText("<center>Manage your job applications with MyJobz<br>"
                         "Visit the <a href=\"https://www.example.com\">github repository</a>"
                         "for the documentation<br>and more information<br>v0.0.1</center>");
        aboutBox.exec();
    });
}

void MainWindow::onTableCellDoubleClicked(const QModelIndex &index) {
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

void MainWindow::onCustomContextMenuRequested(const QPoint &pos) {
    QModelIndex index = tableView->indexAt(pos);
    if (index.isValid()) {
        QMenu contextMenu(tr("Context Menu"), this);
        QAction *markAsFinishedAction = new QAction(tr("Mark as Finished"), this);
        connect(markAsFinishedAction, &QAction::triggered, this, [this, index]() {
            QModelIndex sourceIndex = sortProxyModel->mapToSource(index);
            int id = model->data(model->index(sourceIndex.row(), 0)).toInt();
            dbManager->updateStatus(id, "Finished");
        });
        contextMenu.addAction(markAsFinishedAction);
        QAction *goToUrlEmailAction = new QAction(tr("Go To URL/Email"), this);
        goToUrlEmailAction->setEnabled(false);
        connect(goToUrlEmailAction, &QAction::triggered, this, [this, index]() {
            QModelIndex sourceIndex = sortProxyModel->mapToSource(index);
            QString data = model->data(model->index(sourceIndex.row(), 5)).toString();
            static QString mailSearchUrl = "https://mail.google.com/mail/u/0/#search/";
            QDesktopServices::openUrl(QUrl(
                ((data.startsWith("http://") || data.startsWith("https://")) ? "" : mailSearchUrl) + data));
        });
        contextMenu.addAction(goToUrlEmailAction);
        QModelIndex sourceIndex = sortProxyModel->mapToSource(index);
        QString data = model->data(model->index(sourceIndex.row(), 5)).toString();
        if (data.startsWith("http://") || data.startsWith("https://") || data.contains("@")) {
            goToUrlEmailAction->setEnabled(true);
        }
        contextMenu.exec(tableView->viewport()->mapToGlobal(pos));
    }
}

void MainWindow::onItemsPerPageChanged() {
    QAction *action = qobject_cast<QAction*>(sender());
    if (action) {
        int itemsPerPage = action->data().toInt();
        sortProxyModel->setItemsPerPage(itemsPerPage);
        sortProxyModel->setCurrentPage(1);
        ui->pageLabel->setText(QString("1"));
        ui->prevPageButton->setEnabled(false);
        ui->nextPageButton->setEnabled(sortProxyModel->pageCount() > 1);
    }
}

void MainWindow::onExportToCSVTriggered() {
    dbManager->exportToCSV();
}

void MainWindow::clearDialogFields() {
    detailsDialog->idLineEdit->setText("");
    detailsDialog->titleLineEdit->setText("");
    detailsDialog->companyLineEdit->setText("");
    detailsDialog->statusLineEdit->setText("");
    detailsDialog->appDateLineEdit->setText("");
    detailsDialog->urlLineEdit->setText("");
    detailsDialog->detailsTextEdit->clear();
}

void MainWindow::onExportToDBTriggered() {
    if (!dbManager) {
        QMessageBox::critical(this, "Error", "DB manager is not initialized.");
        return;
    }
    dbManager->exportDatabase();
}

void MainWindow::onImportFromDBTriggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open Database File"), QDir::homePath(), tr("SQLite Database (*.db)"));
    if (filePath.isEmpty()) {
        return;
    }
    tableView->setModel(nullptr);
    dbManager->closeDatabase();
    if (!dbManager->openDatabase(filePath)) {
        QMessageBox::critical(this, "Error", "Failed to open database.");
        return;
    }
    tableView->setModel(sortProxyModel);
    QMessageBox::information(this, "Success", "Database loaded successfully.");
}

void MainWindow::onAddNewItemButtonClicked() {
    clearDialogFields();
    detailsDialog->idLineEdit->hide();
    detailsDialog->idLabel->hide();
    QDate today = QDate::currentDate();
    if (ui->actionToday->isChecked()) {
        detailsDialog->appDateLineEdit->setText(today.toString("dd/MM/yyyy"));
    }
    if (ui->actionApplied->isChecked()) {
        detailsDialog->statusLineEdit->setText("Applied");
    }
    detailsDialog->titleLineEdit->setFocus();
    detailsDialog->exec();
}

void MainWindow::onPrevPageButtonClicked() {
    int currentPage = sortProxyModel->getCurrentPage();
    if (currentPage <= 1) {
        return;
    }
    sortProxyModel->setCurrentPage(currentPage - 1);
    int newPage = sortProxyModel->getCurrentPage();
    if (newPage < currentPage) {
        ui->pageLabel->setText(QString::number(newPage));
        ui->nextPageButton->setEnabled(true);
        if (newPage == 1) {
            ui->prevPageButton->setEnabled(false);
        }
    }
}

void MainWindow::onNextPageButtonClicked() {
    int currentPage = sortProxyModel->getCurrentPage();
    if (currentPage >= sortProxyModel->pageCount()) {
        return;
    }
    sortProxyModel->setCurrentPage(currentPage + 1);
    int newPage = sortProxyModel->getCurrentPage();
    if (newPage > currentPage) {
        ui->pageLabel->setText(QString::number(newPage));
        ui->prevPageButton->setEnabled(true);
        if (newPage == sortProxyModel->pageCount()) {
            ui->nextPageButton->setEnabled(false);
        }
    }
}

void MainWindow::onDeleteButtonClicked() {
    QModelIndex selectedIndex = tableView->selectionModel()->currentIndex();
    QString outputStr;
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
    {
        QSqlQuery query(db);
        query.prepare("DELETE FROM jobs WHERE id = :id");
        query.bindValue(":id", id);
        if (!query.exec()) {
            outputStr += "Error: " + query.lastError().text();
            QMessageBox::critical(this, "Error", outputStr);
            return;
        }
    }
    model->setQuery("SELECT * FROM jobs");
    ui->nextPageButton->setEnabled(sortProxyModel->getCurrentPage() < sortProxyModel->pageCount());
    QMessageBox::information(this, "Success", "The appointment has been deleted successfully.");
}

void MainWindow::onDuplicateButtonClicked() {
    QModelIndex selectedIndex = tableView->selectionModel()->currentIndex();
    QString outputStr;
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "Warning", "Please select any cell of an item before duplicating.");
        return;
    }
    QModelIndex sourceIndex = sortProxyModel->mapToSource(selectedIndex);
    QSqlRecord record = model->record(sourceIndex.row());
    {
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
            outputStr += "Error: " + query.lastError().text();
            QMessageBox::critical(this, "Error", outputStr);
            return;
        }
    }
    model->setQuery("SELECT * FROM jobs");
    ui->nextPageButton->setEnabled(sortProxyModel->getCurrentPage() < sortProxyModel->pageCount());
    QMessageBox::information(this, "Success", "The appointment has been duplicated successfully.");
}

void MainWindow::onSearchButtonClicked() {
    QString searchString = ui->searchLineEdit->text();
    QRegularExpression regex(searchString, QRegularExpression::CaseInsensitiveOption);
    sortProxyModel->setFilterRegularExpression(regex);
}

void MainWindow::onDetailsSubmitButtonClicked() {
    QString idText = detailsDialog->idLineEdit->text();
    int id = idText.isEmpty() ? -1 : idText.toInt();
    bool isNewEntry = id < 0;
    {
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
        query.bindValue(":details", detailsDialog->detailsTextEdit->toPlainText().trimmed());
        if (!isNewEntry) {
            query.bindValue(":id", id);
        }
        if (!query.exec()) {
            QMessageBox::critical(detailsDialog, "Error", "Failed to update appointment");
            detailsDialog->close();
            return;
        }
    }
    model->setQuery("SELECT * FROM jobs");
    QMessageBox::information(detailsDialog, "Success",
        QString("Appointment ") + (isNewEntry ? "added" : "updated") + " successfully");
    ui->nextPageButton->setEnabled(sortProxyModel->getCurrentPage() < sortProxyModel->pageCount());
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

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason) {
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
    if (isMinimized() && ui->trayCheckBox->isChecked()) {
        restoreWindow();
    }
    QString clipboardText = QApplication::clipboard()->text();
    clearDialogFields();
    detailsDialog->urlLineEdit->setText(clipboardText);
    QDate today = QDate::currentDate();
    detailsDialog->idLineEdit->hide();
    detailsDialog->idLabel->hide();
    if (ui->actionToday->isChecked()) {
        detailsDialog->appDateLineEdit->setText(today.toString("dd/MM/yyyy"));
    }
    if (ui->actionApplied->isChecked()) {
        detailsDialog->statusLineEdit->setText("Applied");
    }
    detailsDialog->titleLineEdit->setFocus();
    detailsDialog->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    detailsDialog->showNormal();
    detailsDialog->raise();
    detailsDialog->activateWindow();
}

void MainWindow::restoreWindow() {
    bool wasMaximized = isMaximized();
    showNormal();
    if (wasMaximized) {
        showMaximized();
    }
    raise();
    activateWindow();
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized() && ui->trayCheckBox->isChecked()) {
            hide();
        }
    }
    QMainWindow::changeEvent(event);
}

MainWindow::~MainWindow() {
    delete ui;
}
