#ifndef DETAILSDIALOG_HPP
#define DETAILSDIALOG_HPP

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
    class DetailsDialog;
}
QT_END_NAMESPACE

class DetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit DetailsDialog(QWidget *parent = nullptr);
    QLabel *idLabel;
    QLineEdit *idLineEdit;
    QLineEdit *titleLineEdit;
    QLineEdit *companyLineEdit;
    QLineEdit *appDateLineEdit;
    QLineEdit *urlLineEdit;
    QLineEdit *statusLineEdit;
    QTextEdit *detailsTextEdit;
    QPushButton *detailsSubmitButton;
    ~DetailsDialog();

private:
    Ui::DetailsDialog *ui;
};

#endif // DETAILSDIALOG_HPP
