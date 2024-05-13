#include "../headers/database_manager.hpp"

DatabaseManager::DatabaseManager(QWidget *parent) {
    this->parent = parent;
    model = new QSqlQueryModel();
}

void DatabaseManager::createDatabaseBackup(const QString& backupFilePath) {
    if (db.isOpen()) {
        db.close();
    }
    QFile::remove(backupFilePath);
    QFile::copy(db.databaseName(), backupFilePath);
    if (!db.open()) {
        QMessageBox::critical(parent, "Error", "Failed to reopen database after creating backup");
        return;
    }
}

bool DatabaseManager::openDatabase(const QString& filePath, bool autoBackup) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);
    bool ok = db.open();
    if (ok && autoBackup) {
        createDatabaseBackup(filePath + ".bak");
    }
    return ok;
}

void DatabaseManager::loadModel() {
    model->setQuery("SELECT * FROM jobs");
    if (model->lastError().isValid()) {
        QMessageBox::critical(parent, "Error", "Error: " + model->lastError().text());
    }
}

void DatabaseManager::closeDatabase() {
    db.close();
    QSqlDatabase::removeDatabase("QSQLITE");
}

bool DatabaseManager::createJobsTableIfNotExists() {
    QSqlQuery query(db);
    return query.exec("CREATE TABLE IF NOT EXISTS jobs ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "job_title TEXT,"
                      "company TEXT,"
                      "status TEXT,"
                      "application_date TEXT,"
                      "url_email TEXT,"
                      "details TEXT,"
                      "is_finished BOOLEAN DEFAULT 0"
                      ");");
}

void DatabaseManager::updateStatus(int id, const QString& status) {
    {
        QSqlQuery query(db);
        query.prepare("UPDATE jobs SET status = :status, is_finished = :is_finished WHERE id = :id");
        query.bindValue(":status", status);
        query.bindValue(":id", id);
        static std::vector<QString> finishedStatus = {"Finished", "Done", "Complete", "Rejected", "Approved"};
        bool isFinished = std::find(finishedStatus.begin(), finishedStatus.end(), status) != finishedStatus.end();
        query.bindValue(":is_finished", isFinished);
        if (!query.exec()) {
            QMessageBox::critical(parent, "Error", "Failed to update status");
            return;
        }
    }
    loadModel();
    QMessageBox::information(parent, "Success", "Status updated successfully");
}

void DatabaseManager::exportDatabase() {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Export Database", QDir::homePath() + "/database.db", "SQLite Database (*.db)");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (file.exists()) {
        if (!file.remove()) {
            QMessageBox::critical(parent, "Error", "Error: Failed to remove existing file.");
            return;
        }
    }
    QSqlDatabase exportDb = QSqlDatabase::addDatabase("QSQLITE", "export");
    exportDb.setDatabaseName(fileName);
    if (!exportDb.open()) {
        QMessageBox::critical(parent, "Error", "Error: " + exportDb.lastError().text());
        return;
    }
    {
        QSqlQuery createTableQuery(exportDb);
        if (!createTableQuery.exec("CREATE TABLE IF NOT EXISTS jobs ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "job_title TEXT,"
                                   "company TEXT,"
                                   "status TEXT,"
                                   "application_date TEXT,"
                                   "url_email TEXT,"
                                   "details TEXT,"
                                   "is_finished BOOLEAN DEFAULT 0"
                                   ");")) {
            QMessageBox::critical(parent, "Error", "Error: " + createTableQuery.lastError().text());
            exportDb.close();
            return;
        }
        QSqlQuery insertQuery(exportDb);
        for (int row = 0; row < model->rowCount(); ++row) {
            QSqlRecord record = model->record(row);
            insertQuery.prepare("INSERT INTO jobs (job_title, company, status, application_date, url_email, details, is_finished) "
                                "VALUES (:job_title, :company, :status, :application_date, :url_email, :details, :is_finished)");
            insertQuery.bindValue(":job_title", record.value("job_title"));
            insertQuery.bindValue(":company", record.value("company"));
            insertQuery.bindValue(":status", record.value("status"));
            insertQuery.bindValue(":application_date", record.value("application_date"));
            insertQuery.bindValue(":url_email", record.value("url_email"));
            insertQuery.bindValue(":details", record.value("details"));
            insertQuery.bindValue(":is_finished", record.value("is_finished"));
            if (!insertQuery.exec()) {
                QMessageBox::critical(parent, "Error", "Error: " + insertQuery.lastError().text());
                exportDb.close();
                return;
            }
        }
    }
    QMessageBox::information(parent, "Success", "Exported DB successfully to " + fileName);
    exportDb.close();
}

void DatabaseManager::exportToCSV() {
    if (!model) {
        QMessageBox::critical(parent, "Error", "Error: No model to export.");
        return;
    }
    QString outputPath = QFileDialog::getSaveFileName(parent, "Save File", "output.csv", "CSV Files (*.csv)");
    if (outputPath.isEmpty()) {
        return;
    }
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(parent, "Error", "IO Error: " + file.errorString());
        return;
    }
    QTextStream out(&file);
    int colCount = model->columnCount();
    int rowCount = model->rowCount();
    for (int col = 0; col < colCount; ++col) {
        out << "\"" << model->headerData(col, Qt::Horizontal).toString() << "\"";
        if (col < colCount - 1)
            out << ",";
    }
    out << "\n";
    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < colCount; ++col) {
            QString value = model->data(model->index(row, col)).toString();
            if (value.contains(',')) {
                value = "\"" + value + "\"";
            }
            out << value;
            if (col < colCount - 1)
                out << ",";
        }
        out << "\n";
    }
    QMessageBox::information(parent, "Success", "Successfully exported as CSV to " + outputPath);
    file.close();
}

QSqlQueryModel* DatabaseManager::getModel() const {
    return model;
}

DatabaseManager::~DatabaseManager() {
    delete model;
    closeDatabase();
}
