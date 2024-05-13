#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <stdexcept>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRecord>

class DatabaseManager {
public:
    DatabaseManager(QWidget* parent = nullptr);
    ~DatabaseManager();
    bool openDatabase(const QString& filePath, bool autoBackup);
    void loadModel();
    void closeDatabase();
    bool createJobsTableIfNotExists();
    void exportDatabase();
    void createDatabaseBackup(const QString& backupFilePath);
    void updateStatus(int id, const QString& status);
    void exportToCSV();
    QSqlQueryModel* getModel() const;

private:
    QSqlDatabase db;
    QSqlQueryModel* model;
    QWidget* parent;
};

#endif // DATABASE_MANAGER_HPP
