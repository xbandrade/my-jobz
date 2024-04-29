#include "../headers/mainwindow.hpp"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowState(windowState() | Qt::WindowMaximized);
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (layout) {
        layout->setAlignment(Qt::AlignCenter);
    }

    // MOCK TABLE DATA
    tableView = findChild<QTableView*>("tableView");
    if (tableView) {
        QStandardItemModel *model = new QStandardItemModel(4, 3, this);
        model->setHorizontalHeaderLabels({"Job Title", "Company", "Status", "Application Date", "URL/Email"});
        model->setItem(0, 0, new QStandardItem("Software Developer"));
        model->setItem(0, 1, new QStandardItem("BIM"));
        model->setItem(0, 2, new QStandardItem("Applied"));
        model->setItem(0, 3, new QStandardItem("28/04/2024"));
        model->setItem(0, 4, new QStandardItem("https://bim.com.br"));

        model->setItem(1, 0, new QStandardItem("Python Developer"));
        model->setItem(1, 1, new QStandardItem("BAM"));
        model->setItem(1, 2, new QStandardItem("Finished"));
        model->setItem(1, 3, new QStandardItem("25/04/2024"));
        model->setItem(1, 4, new QStandardItem("https://bam.com.br"));


        model->setItem(2, 0, new QStandardItem("Data Analyst"));
        model->setItem(2, 1, new QStandardItem("BEM"));
        model->setItem(2, 2, new QStandardItem("Technical Tests"));
        model->setItem(2, 3, new QStandardItem("13/04/2024"));
        model->setItem(2, 4, new QStandardItem("bem@bem.br"));


        model->setItem(3, 0, new QStandardItem("C++ Developer"));
        model->setItem(3, 1, new QStandardItem("BOM"));
        model->setItem(3, 2, new QStandardItem("HR Interview (30/04)"));
        model->setItem(3, 3, new QStandardItem("27/04/2024"));
        model->setItem(3, 4, new QStandardItem("https://bom.com.br"));

        tableView->horizontalHeader()->setStyleSheet(
            "QHeaderView::section { "
            "background-color: #749190;"
            "font-family: Nunito;"
            "font-size: 16px;"
            "}"
        );
        tableView->verticalHeader()->setVisible(false);
        tableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tableView->setModel(model);
    }

}

MainWindow::~MainWindow() {
    delete ui;
}
