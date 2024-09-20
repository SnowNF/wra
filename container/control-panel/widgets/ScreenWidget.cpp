#include <iostream>
#include <QDir>
#include <QDirIterator>
#include "ScreenWidget.h"
#include "ui_ScreenWidget.h"
#include "mainwindow.h"
#include "config.h"
#include <QThread>

void ScreenWidget::initEventPath() {
    QDir dir("/dev/input/");

    if (!dir.exists()) {
        std::cerr << "Directory does not exist:" << dir.path().toStdString();
    }

    QDirIterator it(dir.path(), QDir::System | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        eventPath.append(filePath);
    }
}

ScreenWidget::ScreenWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::ScreenWidget) {
    ui->setupUi(this);
    connect(ui->kbdButton, &QPushButton::clicked, this, &ScreenWidget::onClickKbd);
    connect(ui->wlopmComboBox, &QComboBox::currentIndexChanged, this, &ScreenWidget::onEventPathSelected);

    QString none = "None";

    eventPath.append(none);
    initEventPath();

    QString event;
    {
        QSettings settings{QSETTINGS_ORGANIZATION, QSETTINGS_APPLICATION};
        event = settings.value(EVENT_TRIGGER_CFG, none).toString();
    }

    listener = ReadEventListener{trigger};
    listener.setPath(event.toStdString());

    for (int i = 0; i < eventPath.size(); ++i) {
        auto &item = eventPath[i];
        ui->wlopmComboBox->addItem(item);
        if (item == event) {
            ui->wlopmComboBox->setCurrentIndex(i);
        }
    }
}

void ScreenWidget::onEventPathSelected(int index) {
    QSettings settings{QSETTINGS_ORGANIZATION, QSETTINGS_APPLICATION};
    QString path = eventPath.at(index);
    settings.setValue(EVENT_TRIGGER_CFG, path);

    if (index == 0)
        return;
    listener.setPath(path.toStdString());
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
        QProcess::startDetached("busybox", {"killall", "wvkbd-mobintl"});
        kbdPid = 0;
        updateKbdText();
    }
    MainWindow::getInstance()->removeStatusStrA(i, "Finished");
}