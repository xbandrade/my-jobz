#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QDebug>
#include <QSqlQuery>
#include <stdexcept>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

class DatabaseManager {
public:
    DatabaseManager(QWidget* parent = nullptr);
    ~DatabaseManager();
    bool openDatabase(const QString& filePath);
    void loadModel();
    void closeDatabase();
    bool createJobsTableIfNotExists();
    void addMockData();
    void exportDatabase();
    QSqlQueryModel* getModel() const;

private:
    QSqlDatabase db;
    QSqlQueryModel* model;
    QWidget* parent;
};

#endif // DATABASE_MANAGER_H
