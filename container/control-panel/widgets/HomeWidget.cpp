#include <iostream>
#include "HomeWidget.h"
#include "ui_HomeWidget.h"
#include <QProcess>
#include <unistd.h>
#include <pwd.h>

HomeWidget::HomeWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::HomeWidget) {
    ui->setupUi(this);
    connect(ui->userButton, &QPushButton::clicked, this, &HomeWidget::onClickStartRootMode);
    uint uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    ui->userLabel->setText("Current Effective User " + QString(pw->pw_name));
    if (uid == 0)
        ui->userButton->setEnabled(false);
}

HomeWidget::~HomeWidget() {
    delete ui;
}

void HomeWidget::onClickStartRootMode() {
    QStringList args;
    args.append(QApplication::applicationFilePath());
    args.append(QProcessEnvironment::systemEnvironment().toStringList());
    QProcess::startDetached("/bin/lxqt-sudo", args);
}