#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

class Logger {
public:
    Logger(const QString& logPath);
    void log(const QString& message, const QString& filePath);
private:
    QString logFile;
};

#endif // LOGGER_H
