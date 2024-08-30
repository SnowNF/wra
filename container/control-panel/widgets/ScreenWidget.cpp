#include <thread>
#include <iostream>
#include "ScreenWidget.h"
#include "ui_ScreenWidget.h"
#include "mainwindow.h"

ScreenWidget::ScreenWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::ScreenWidget) {
    ui->setupUi(this);
    connect(ui->kbdButton, &QPushButton::clicked, this, &ScreenWidget::onClickKbd);
}

ScreenWidget::~ScreenWidget() {
    delete ui;
}

void ScreenWidget::updateKbdText() {
    ui->kbdButton->setText(kbdPid == 0 ? "Execute" : "Kill");
}

void ScreenWidget::onClickKbd() {
    int i = MainWindow::getInstance()->addStatusStrA("Performing");
    if (kbdPid == 0) {
        std::cerr << "Executing wvkbd-mobintl\n";
        QProcess::startDetached("/usr/bin/wvkbd-mobintl", {}, QString(), &kbdPid);
        updateKbdText();
    } else {
        std::cerr << "Killing wvkbd-mobintl pid: " << kbdPid << "\n";
        QProcess::startDetached("/usr/bin/kill", {QString::number(kbdPid)});
        kbdPid = 0;
        updateKbdText();
    }
    MainWindow::getInstance()->removeStatusStrA(i, "Finished");
}