#ifndef CUSTOM_TEXTEDIT_HPP
#define CUSTOM_TEXTEDIT_HPP

#include <QTextEdit>
#include <QMimeData>
#include <QTextCursor>
#include <QTextCharFormat>

class CustomTextEdit : public QTextEdit {
public:
    CustomTextEdit(QWidget *parent = nullptr);

protected:
    void insertFromMimeData(const QMimeData *source) override;
};

#endif // CUSTOM_TEXTEDIT_HPP
