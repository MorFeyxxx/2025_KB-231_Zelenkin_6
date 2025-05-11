#include "Logger.h"

Logger::Logger(const QString& logPath) : logFile(logPath) {}

void Logger::log(const QString& message, const QString& filePath) {
    QFile file(logFile);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << message << "\n";
        file.close();
    }
}

