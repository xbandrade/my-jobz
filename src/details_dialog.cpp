#include "../headers/details_dialog.hpp"
#include "./ui_details_dialog.h"

DetailsDialog::DetailsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::DetailsDialog) {
    ui->setupUi(this);
    idLabel = ui->idLabel;
    idLineEdit = ui->idLineEdit;
    titleLineEdit = ui->titleLineEdit;
    companyLineEdit = ui->companyLineEdit;
    appDateLineEdit = ui->appDateLineEdit;
    urlLineEdit = ui->urlLineEdit;
    statusLineEdit = ui->statusLineEdit;
    detailsTextEdit = ui->detailsTextEdit;
    detailsSubmitButton = ui->detailsSubmitButton;
    connect(detailsSubmitButton, &QPushButton::clicked, this, &DetailsDialog::onSubmitButtonClicked);
    connect(this, &QDialog::finished, this, &DetailsDialog::onDialogFinished);
}

void DetailsDialog::onSubmitButtonClicked() {
    setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    show();
}

void DetailsDialog::onDialogFinished(int result) {
    if (result == QDialog::Accepted) {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        show();
    }
}

DetailsDialog::~DetailsDialog() {
    delete ui;
}
