#include "../headers/main_window.hpp"
#include "./ui_main_window.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::mainWindow) {
    showClipboardHint = true;
    settings = new QSettings("MyJobz", "MyJobz");
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
    detailsDialog = new DetailsDialog();
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
        if (!dbManager->openDatabase("database.db", ui->actionAutoBackup->isChecked())) {
            QMessageBox::critical(parent, "Error", "IO Error");
            return;
        }
        if (!dbManager->createJobsTableIfNotExists()) {
            QMessageBox::critical(parent, "Error", "Error creating table");
            return;
        }
        model = dbManager->getModel();
        dbManager->loadModel();
        sortProxyModel = new SortProxyModel(this);
        sortProxyModel->setSourceModel(model);
        setupTableView();
    }
    loadUserPreferences();
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
    connect(ui->firstPageButton, &QCheckBox::clicked, this, &MainWindow::onFirstPageButtonClicked);
    connect(ui->lastPageButton, &QCheckBox::clicked, this, &MainWindow::onLastPageButtonClicked);
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
    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::saveUserPreferences);
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
        aboutBox.setText("<div style='display: flex; align-items: center; justify-content: center; text-align: center;'>"
                         "<div style='margin-right: 10px;'><img src=':/application.png' height='96'></div>"
                         "<div style='display: flex; align-items: center; justify-content: center;'>"
                         "<b style='margin-right: 5px; font-size: 24px;'>MyJobz v0.0.2</b></div>"
                         "<div>Manage your job applications with <b>MyJobz</b><br>"
                         "Visit the <a href='https://github.com/xbandrade/my-jobz/'>GitHub repository</a> "
                         "for the documentation<br>and more information</div>"
                         "</div>");

        aboutBox.exec();
    });
}

void MainWindow::loadUserPreferences() {
    ui->actionAutoBackup->setChecked(settings->value("autoBackup", true).toBool());
    ui->actionApplied->setChecked(settings->value("defaultStatus", true).toBool());
    ui->actionBlankStatus->setChecked(!ui->actionApplied->isChecked());
    ui->actionToday->setChecked(settings->value("defaultDate", true).toBool());
    ui->actionBlankDate->setChecked(!ui->actionToday->isChecked());
    ui->trayCheckBox->setChecked(settings->value("minimizeToTray", true).toBool());
    ui->clipboardCheckBox->setChecked(settings->value("clipboardMonitor", false).toBool());
    ui->hideFinishedCheckBox->setChecked(settings->value("hideFinished", false).toBool());
    ui->action10Items->setChecked(true);
    int itemsPerPage = settings->value("itemsPerPage", 10).toInt();
    QActionGroup *itemsPerPageGroup = ui->action10Items->actionGroup();
    for (auto *action : itemsPerPageGroup->actions()) {
        if (action->data().toInt() == itemsPerPage) {
            action->setChecked(true);
            sortProxyModel->setItemsPerPage(itemsPerPage);
            onItemsPerPageChanged();
            break;
        }
    }
}

void MainWindow::saveUserPreferences() {
    settings->setValue("autoBackup", ui->actionAutoBackup->isChecked());
    settings->setValue("defaultStatus", ui->actionApplied->isChecked());
    settings->setValue("defaultDate", ui->actionToday->isChecked());
    settings->setValue("minimizeToTray", ui->trayCheckBox->isChecked());
    settings->setValue("clipboardMonitor", ui->clipboardCheckBox->isChecked());
    settings->setValue("hideFinished", ui->hideFinishedCheckBox->isChecked());
    int itemsPerPage = 10;
    QActionGroup *itemsPerPageGroup = ui->action10Items->actionGroup();
    for (auto *action : itemsPerPageGroup->actions()) {
        if (action->isChecked()) {
            itemsPerPage = action->data().toInt();
            break;
        }
    }
    settings->setValue("itemsPerPage", itemsPerPage);
}

void MainWindow::setupTableView() {
    tableView->setModel(sortProxyModel);
    static QStringList headers = {"ID", "Job Title", "Company", "Status", "Application Date", "URL/Email", "Details"};
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
    sortProxyModel->setCurrentPage(1);
    updatePagination();
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
        QModelIndex sourceIndex = sortProxyModel->mapToSource(index);
        bool isFinished = model->data(model->index(sourceIndex.row(), 7)).toBool();
        QMenu contextMenu(tr("Context Menu"), this);
        QAction *markAsFinishedAction = new QAction(
            isFinished ? tr("Unmark as Finished") : tr("Mark as Finished"), this);
        connect(markAsFinishedAction, &QAction::triggered, this, [this, sourceIndex, isFinished]() {
            int id = model->data(model->index(sourceIndex.row(), 0)).toInt();
            QString newStatus = isFinished ? "" : "Finished";
            if (newStatus.isEmpty()) {
                bool ok;
                QString status = QInputDialog::getText(
                    this, tr("New Status"), tr("Enter new status:"), QLineEdit::Normal, "", &ok);
                if (!ok || status.isEmpty()) {
                    return;
                }
                newStatus = status;
            }
            dbManager->updateStatus(id, newStatus);
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
        updatePagination();
    }
}

void MainWindow::updatePagination() {
    int currentPage = sortProxyModel->getCurrentPage();
    int totalPages = sortProxyModel->pageCount();
    QString labelText = QString("<span style='background-color: #8d959d;'>%1</span> of %2").arg(currentPage).arg(totalPages);
    ui->currPageLabel->setText(labelText);
    ui->prevPageButton->setEnabled(currentPage > 1);
    ui->firstPageButton->setEnabled(currentPage > 1);
    ui->nextPageButton->setEnabled(currentPage < totalPages);
    ui->lastPageButton->setEnabled(currentPage < totalPages);
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
    if (!dbManager->openDatabase(filePath, ui->actionAutoBackup->isChecked())) {
        QMessageBox::critical(this, "Error", "Failed to open database.");
        return;
    }
    dbManager->loadModel();
    setupTableView();
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

void MainWindow::onFirstPageButtonClicked() {
    sortProxyModel->setCurrentPage(1);
    updatePagination();
}

void MainWindow::onLastPageButtonClicked() {
    sortProxyModel->setCurrentPage(sortProxyModel->pageCount());
    updatePagination();
}

void MainWindow::onPrevPageButtonClicked() {
    int currentPage = sortProxyModel->getCurrentPage();
    if (currentPage <= 1) {
        return;
    }
    sortProxyModel->setCurrentPage(currentPage - 1);
    updatePagination();
}

void MainWindow::onNextPageButtonClicked() {
    int currentPage = sortProxyModel->getCurrentPage();
    if (currentPage >= sortProxyModel->pageCount()) {
        return;
    }
    sortProxyModel->setCurrentPage(currentPage + 1);
    updatePagination();
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
    sortProxyModel->setCurrentPage(std::min(sortProxyModel->getCurrentPage(), sortProxyModel->pageCount()));
    updatePagination();
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
    updatePagination();
    QMessageBox::information(this, "Success", "The appointment has been duplicated successfully.");
}

void MainWindow::onSearchButtonClicked() {
    QString searchString = ui->searchLineEdit->text();
    QRegularExpression regex(searchString, QRegularExpression::CaseInsensitiveOption);
    sortProxyModel->setFilterRegularExpression(regex);
    updatePagination();
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
    updatePagination();
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
