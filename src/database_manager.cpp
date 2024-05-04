#include "../headers/database_manager.hpp"

DatabaseManager::DatabaseManager(QWidget *parent) {
    this->parent = parent;
    model = new QSqlQueryModel();
}

bool DatabaseManager::openDatabase(const QString& filePath) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filePath);
    return db.open();
}

void DatabaseManager::loadModel() {
    model->setQuery("SELECT * FROM jobs");
    if (model->lastError().isValid()) {
        qDebug() << "Error loading model:" << model->lastError().text();
    }
}

void DatabaseManager::closeDatabase() {
    db.close();
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
                      "details TEXT"
                      ");");
}

void DatabaseManager::addMockData() {
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

void DatabaseManager::exportDatabase() {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Export Database", QDir::homePath() + "/database.db", "SQLite Database (*.db)");
    if (fileName.isEmpty()) {
        return;
    }
    QSqlDatabase exportDb = QSqlDatabase::addDatabase("QSQLITE", "export");
    exportDb.setDatabaseName(fileName);
    QString outputStr;
    if (!exportDb.open()) {
        outputStr += "Error: " + exportDb.lastError().text();
        QMessageBox::critical(parent, "Error", outputStr);
        return;
    }
    QSqlQuery createTableQuery(exportDb);
    if (!createTableQuery.exec("CREATE TABLE IF NOT EXISTS jobs ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "job_title TEXT,"
                               "company TEXT,"
                               "status TEXT,"
                               "application_date TEXT,"
                               "url_email TEXT,"
                               "details TEXT"
                               ");")) {
        outputStr += "Error: " + createTableQuery.lastError().text();
        QMessageBox::critical(parent, "Error", outputStr);
        exportDb.close();
        return;
    }
    QSqlQuery insertQuery(exportDb);
    for (int row = 0; row < model->rowCount(); ++row) {
        QSqlRecord record = model->record(row);
        insertQuery.prepare("INSERT INTO jobs (job_title, company, status, application_date, url_email, details) "
                            "VALUES (:job_title, :company, :status, :application_date, :url_email, :details)");
        insertQuery.bindValue(":job_title", record.value("job_title"));
        insertQuery.bindValue(":company", record.value("company"));
        insertQuery.bindValue(":status", record.value("status"));
        insertQuery.bindValue(":application_date", record.value("application_date"));
        insertQuery.bindValue(":url_email", record.value("url_email"));
        insertQuery.bindValue(":details", record.value("details"));
        if (!insertQuery.exec()) {
            outputStr += "Error: " + insertQuery.lastError().text();
            QMessageBox::critical(parent, "Error", outputStr);
            exportDb.close();
            return;
        }
    }
    outputStr += "Exported DB successfully to " + fileName;
    QMessageBox::information(parent, "Success", outputStr);
    exportDb.close();
}


QSqlQueryModel* DatabaseManager::getModel() const {
    return model;
}

DatabaseManager::~DatabaseManager() {
    delete model;
    closeDatabase();
}
