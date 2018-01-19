#ifndef LOGGER_H
#define LOGGER_H

#include <QPlainTextEdit>


class Logger
{
public:
    static void initialize(QPlainTextEdit* logField);
    static void log(QString text);
private:
    static QPlainTextEdit* m_logField;
};

#endif // LOGGER_H
