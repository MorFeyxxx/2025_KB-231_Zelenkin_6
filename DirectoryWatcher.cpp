#include "Directorywatcher.h"
#include <QDir>
#include <QFileInfoList>

DirectoryWatcher::DirectoryWatcher(QObject *parent)
    : QObject(parent), watcher(new QFileSystemWatcher(this)) {
    connect(watcher, &QFileSystemWatcher::fileChanged,
            this, &DirectoryWatcher::onFileChanged);
    connect(watcher, &QFileSystemWatcher::directoryChanged,
            this, &DirectoryWatcher::onDirectoryChanged);
}

void DirectoryWatcher::watch(const QString& path) {
    currentDir = path;
    initializing = true;
    watcher->addPath(path);
    updateWatchedFiles();
    initializing = false;
}

QMap<QString, QFileInfo> DirectoryWatcher::collectAllFiles(const QString& root) {
    QMap<QString, QFileInfo> result;
    QDir dir(root);
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    for (const QFileInfo& info : entries) {
        result.insert(info.absoluteFilePath(), info);
        if (info.isDir()) {
            QMap<QString, QFileInfo> sub = collectAllFiles(info.absoluteFilePath());
            for (auto it = sub.constBegin(); it != sub.constEnd(); ++it) {
                result.insert(it.key(), it.value());
            }
        }
    }

    return result;
}

void DirectoryWatcher::updateWatchedFiles() {

    QMap<QString, QFileInfo> newFiles = collectAllFiles(currentDir);

    QList<QString> removed;
    QList<QString> added;

    for (const QString& oldPath : knownFiles.keys()) {
        if (!newFiles.contains(oldPath)) {
            removed.append(oldPath);
        }
    }

    for (const QString& newPath : newFiles.keys()) {
        if (!knownFiles.contains(newPath)) {
            added.append(newPath);
        }
    }

    QSet<QString> handledAdded, handledRemoved;
    for (const QString& oldPath : removed) {
        const QFileInfo& oldInfo = knownFiles[oldPath];

        for (const QString& newPath : added) {
            const QFileInfo& newInfo = newFiles[newPath];

            int timeDiff = std::abs(oldInfo.lastModified().secsTo(newInfo.lastModified()));

            if (timeDiff <= 1) {
                watcher->removePath(oldPath);
                watcher->addPath(newPath);

                if (oldInfo.size() == newInfo.size()) {
                    emit fileChanged("renamed", newPath, oldPath);
                    recentlyRenamed.insert(newPath);
                } else {
                    emit fileChanged("renamed and modified", newPath, oldPath);
                }

                handledAdded.insert(newPath);
                handledRemoved.insert(oldPath);
                break;
            }
        }
    }

    for (const QString& path : removed) {
        if (!handledRemoved.contains(path)) {
            watcher->removePath(path);
            if (!initializing) {
                emit fileChanged("deleted", path);
            }
        }
    }

    for (const QString& path : added) {
        if (!handledAdded.contains(path)) {
            watcher->addPath(path);
            if (!initializing) {
                emit fileChanged("created", path);
            }
        }
    }

    for (const QString& path : newFiles.keys()) {
        if (knownFiles.contains(path)) {
            const QFileInfo& oldInfo = knownFiles[path];
            const QFileInfo& newInfo = newFiles[path];

            if ((oldInfo.lastModified() != newInfo.lastModified() ||
                 oldInfo.size() != newInfo.size()) &&
                !oldInfo.isDir()) {
                if (!recentlyRenamed.contains(path)) {
                    emit fileChanged("modified", path);
                } else {
                    recentlyRenamed.remove(path);
                }
            }

        }
    }

    knownFiles = newFiles;
}


void DirectoryWatcher::onFileChanged(const QString& path) {
    Q_UNUSED(path);
    updateWatchedFiles();
}

void DirectoryWatcher::onDirectoryChanged(const QString& path) {
    Q_UNUSED(path);
    updateWatchedFiles();
}
