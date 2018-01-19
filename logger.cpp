#include "logger.h"

QPlainTextEdit* Logger::m_logField = NULL;

void Logger::initialize(QPlainTextEdit* logField)
{
    m_logField = logField;
}

void Logger::log(QString text)
{
    if (m_logField == NULL)
        return;

    QTextCursor text_cursor = QTextCursor(m_logField->document());
    text_cursor.movePosition(QTextCursor::End);

    text_cursor.insertText(text+"\n");
}
