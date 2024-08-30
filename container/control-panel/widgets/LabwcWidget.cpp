#include <QDir>
#include <iostream>
#include "LabwcWidget.h"
#include "ui_LabwcWidget.h"
#include "mainwindow.h"
#include <utils/XmlHelper.h>

static struct {
    QString name;
    QString value;
} Values[] = {{"90 deg cw",            "0 -1 1 1 0 0"},
              {"180 deg cw",           "-1 0 1 0 -1 1"},
              {"270 deg cw",           "0 1 0 -1 0 1"},
              {"reflect along y axis", "-1 0 1 1 0 0"}};

LabwcWidget::LabwcWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::LabwcWidget) {
    ui->setupUi(this);
    connect(ui->saveButton, &QPushButton::clicked, this, &LabwcWidget::onClickSave);
    for (const auto &item: Values) {
        ui->cbComboBox->addItem(item.name);
    }
}

LabwcWidget::~LabwcWidget() {
    delete ui;
}

void LabwcWidget::onClickSave() {
    QString path = QDir::homePath();
    path += "/.config/labwc/rc.xml";
    QString value = Values[ui->cbComboBox->currentIndex()].value;
    int i = MainWindow::getInstance()->addStatusStrA("Writing " + value + " to " + path);
    XmlHelper xml;
    xml.openFile(path);
    auto root = xml.getRoot();
    auto libinput = xml.getOrCreateElement(root, "libinput");
    auto device = xml.getOrCreateElement(libinput, "device");
    auto cm = xml.getOrCreateElement(device, "calibrationMatrix");
    xml.replaceElement(cm, value);
    xml.writeToFile(path);
    MainWindow::getInstance()->removeStatusStrA(i, "Finished");
}