#include "../headers/custom_textedit.hpp"

CustomTextEdit::CustomTextEdit(QWidget *parent) : QTextEdit(parent) {
}

void CustomTextEdit::insertFromMimeData(const QMimeData *source) {
    if (source->hasText()) {
        QTextCursor cursor = textCursor();
        QTextCharFormat originalFormat = cursor.charFormat();
        QTextCharFormat newFormat;
        newFormat.setFontFamilies(QStringList("Nunito"));
        newFormat.setFontPointSize(11);
        newFormat.setForeground(Qt::black);
        cursor.setCharFormat(newFormat);
        cursor.insertText(source->text());
        cursor.setCharFormat(originalFormat);
        setTextCursor(cursor);
        return;
    }
    QTextEdit::insertFromMimeData(source);
}
