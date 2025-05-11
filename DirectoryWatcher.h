#ifndef DIRECTORYWATCHER_H
#define DIRECTORYWATCHER_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QString>
#include <QSet>
#include <QFileInfo>
#include <QTimer>

class DirectoryWatcher : public QObject {
    Q_OBJECT
public:
    QMap<QString, QFileInfo> collectAllFiles(const QString& root);
    DirectoryWatcher(QObject *parent = nullptr);
    void watch(const QString& path);

signals:
    void fileChanged(const QString& action, const QString& oldPath, const QString& newPath = "");

private slots:
    void onFileChanged(const QString& path);
    void onDirectoryChanged(const QString& path);

private:
    QFileSystemWatcher* watcher;
    bool initializing = false;
    QSet<QString> recentlyRenamed;
    QString currentDir;
    QMap<QString, QFileInfo> knownFiles;

    void updateWatchedFiles();
};

#endif // DIRECTORYWATCHER_H
