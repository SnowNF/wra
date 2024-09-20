#include "MiscWidget.h"
#include "ui_MiscWidget.h"
#include "mainwindow.h"
#include <QProcess>
#include <Utils.h>

MiscWidget::MiscWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::MiscWidget) {
    ui->setupUi(this);
    connect(ui->systemdButton, &QPushButton::clicked, this, &MiscWidget::onClickStartSystemd);
}

MiscWidget::~MiscWidget() {
    delete ui;
}

void MiscWidget::onClickStartSystemd() {
    if (!Utils::checkRoot())
        return;
    int i = MainWindow::getInstance()->addStatusStrA("Writing");
    QString out;
    QString prog = "busybox";
    QStringList args = {"touch", "/wra/.systemd"};
    if (Utils::exec(prog, args, out) != 0) {
        Utils::unableToExecMsgBox(prog, args, out);
    }
    MainWindow::getInstance()->removeStatusStrA(i, "Finished");
}
