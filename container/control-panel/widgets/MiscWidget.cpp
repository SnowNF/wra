#include "MiscWidget.h"
#include "ui_MiscWidget.h"
#include "mainwindow.h"

MiscWidget::MiscWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::MiscWidget) {
    ui->setupUi(this);
    connect(ui->systemdButton, &QPushButton::clicked, this, &MiscWidget::onClickStartSystemd);
}

MiscWidget::~MiscWidget() {
    delete ui;
}

void MiscWidget::onClickStartSystemd() {
    int i = MainWindow::getInstance()->addStatusStrA("Writing");

    MainWindow::getInstance()->removeStatusStrA(i, "Finished");
}
