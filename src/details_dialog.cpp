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
    installEventFilter(this);
}

bool DetailsDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == this && event->type() == QEvent::MouseButtonPress) {
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        show();
    }
    return QDialog::eventFilter(obj, event);
}

DetailsDialog::~DetailsDialog() {
    delete ui;
}
