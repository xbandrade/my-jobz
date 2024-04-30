#include "../headers/detailsdialog.hpp"
#include "./ui_detailsdialog.h"

DetailsDialog::DetailsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DetailsDialog) {
    ui->setupUi(this);
    idLineEdit = ui->idLineEdit;
    titleLineEdit = ui->titleLineEdit;
    companyLineEdit = ui->companyLineEdit;
    appDateLineEdit = ui->appDateLineEdit;
    urlLineEdit = ui->urlLineEdit;
    statusLineEdit = ui->statusLineEdit;
    detailsTextEdit = ui->detailsTextEdit;
}

DetailsDialog::~DetailsDialog() {
    delete ui;
}
