#include <QLabel>
#include <thread>
#include <QSemaphore>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <unistd.h>
#include <semaphore>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->list, &QListWidget::currentRowChanged, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    connect(this, &MainWindow::runOnUiThread, this, &MainWindow::doRunOnUiThread);
    instance = this;
    if (geteuid() == 0) {
        setWindowTitle(windowTitle() + " [root]");
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

UI_THREAD
void MainWindow::doRunOnUiThread(const std::function<void()> &function) {
    function();
}


UI_THREAD
int MainWindow::addStatusStr(const QString &string) {
    for (int i = 0;; ++i) {
        QLabel *res = statusLabels.value(i, nullptr);
        if (res == nullptr) {
            res = new QLabel(">" + string);
            fprintf(stderr, "StatusBar: add %d %s\n", i, string.toUtf8().data());
            ui->statusbar->addWidget(res);
            statusLabels[i] = res;
            return i;
        }
    }
}

UI_THREAD
void MainWindow::replaceStatusStr(int index, const QString &string) {
    QLabel *res = statusLabels.value(index, nullptr);
    if (res == nullptr)
        assert(0);
    fprintf(stderr, "StatusBar: replace %d %s\n", index, string.toUtf8().data());
    res->setText(string);
}

ANY_THREAD
void MainWindow::replaceStatusStrA(int index, const QString &string) {
    runOnUiThread([this, index, string] {
        replaceStatusStr(index, string);
    });
}

UI_THREAD
void MainWindow::removeStatusStr(int index, const QString &finishStr, int timeout) {
    QLabel *res = statusLabels.value(index, nullptr);
    if (res == nullptr)
        assert(0);
    res->setText(">" + finishStr);
    fprintf(stderr, "StatusBar: remove %d %s\n", index, finishStr.toUtf8().data());
    std::thread([this, res, index, timeout] {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        runOnUiThread([this, res, index] {
            ui->statusbar->removeWidget(res);
            delete res;
            statusLabels.remove(index);
        });
    }).detach();
}

ANY_THREAD
int MainWindow::addStatusStrA(const QString &string) {
    QSemaphore semaphore;
    int *r = new int;
    runOnUiThread([this, string, &semaphore, r] {
        *r = addStatusStr(string);
        semaphore.release();
    });
    semaphore.acquire();
    int rr = *r;
    delete r;
    return rr;
}

ANY_THREAD
void MainWindow::removeStatusStrA(int index, const QString &finishStr, int timeout) {
    runOnUiThread([this, index, finishStr, timeout] {
        removeStatusStr(index, finishStr, timeout);
    });
}

MainWindow *MainWindow::getInstance() {
    return instance;
}

void MainWindow::runOnUiThreadBlocked(const std::function<void()> &function) {
    std::counting_semaphore<1> semaphore{0};
    runOnUiThread([&] {
        function();
        semaphore.release();
    });
    semaphore.acquire();
}
