#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Git Пародия");

    watcher = new DirectoryWatcher(this);

    connect(watcher, &DirectoryWatcher::fileChanged, this, [=](const QString& action, const QString& Newpath, const QString& OldPath) {
        QString relativePath = QDir(currentDir).relativeFilePath(Newpath);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

        QString message;
        if (action == "renamed" || action == "renamed and modified") {
            QString oldRelative = QDir(currentDir).relativeFilePath(OldPath);
            message = timestamp + " — " + action + ": " + oldRelative + " -> " + relativePath;
        } else {
            message = timestamp + " — " + action + ": " + relativePath;
        }

        ui->plainTextEdit->appendPlainText(message);

        if (logger) {
            (*logger)->log(message, relativePath);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    delete logger;
}

void MainWindow::on_chooseDirButton_clicked() {
    QString dir = QFileDialog::getExistingDirectory(this, "Выбрать директорию");
    if (!dir.isEmpty()) {
        currentDir = dir;
        ui->statusbar->showMessage("Выбрана директория: " + currentDir);
    }
}

void MainWindow::on_startWatchingButton_clicked() {
    if (!currentDir.isEmpty() && !watching) {
        watcher->watch(currentDir);
        watching = true;
        ui->plainTextEdit->appendPlainText("Слежение запущено...");
    }
}

void MainWindow::on_stopWatchingButton_clicked() {
    if (watching) {
        watching = false;
        ui->plainTextEdit->appendPlainText("Слежение остановлено.");
    }
}

void MainWindow::on_chooseLogFileButton_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Выбрать лог-файл");
    if (!filePath.isEmpty()) {
        delete logger;
        logger = new SmartRtr<Logger>(new Logger(filePath));
        ui->statusbar->showMessage("Лог-файл: " + filePath);
    }
}
