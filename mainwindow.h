#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#include "SmartPtr.h"
#include "Logger.h"
#include "DirectoryWatcher.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_chooseDirButton_clicked();
    void on_startWatchingButton_clicked();
    void on_stopWatchingButton_clicked();
    void on_chooseLogFileButton_clicked();

private:
    Ui::MainWindow *ui;

    SmartRtr<Logger>* logger;
    DirectoryWatcher* watcher;
    bool watching = false;
    QString currentDir;
};
#endif // MAINWINDOW_H
