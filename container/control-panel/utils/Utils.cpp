//
// Created by snownf on 24-8-22.
//

#include <QProcess>
#include <QMessageBox>
#include <iostream>
#include "Utils.h"
#include <mainwindow.h>

QString Utils::cmdToString(const QString &program, const QStringList &arguments) {
    QString cmd = program;
    for (const auto &item: arguments) {
        cmd += " " + item;
    }
    return cmd;
}

int Utils::exec(const QString &program, const QStringList &arguments, QString &result) {
    QProcess p;
    std::cerr << "Utils::exec " << cmdToString(program, arguments).toStdString() << "\n";
    p.start(program, arguments);
    p.waitForStarted();
    p.waitForFinished();
    result = p.readAllStandardOutput();
    result += p.readAllStandardError();
    std::cerr << "Utils::exec " << program.toStdString() << ":\n" << result.toStdString();
    std::cerr << "Utils::exec\n";
    return p.exitCode();
}

void Utils::unableToExecMsgBox(const QString &program, const QStringList &arguments, const QString &result) {
    QMessageBox messageBox{MainWindow::getInstance()};
    messageBox.setText("Unable to exec [" + cmdToString(program, arguments) + "]\n\n" + result);
    messageBox.exec();
}

bool Utils::unableToExecContinueMsgBox(const QString &program, const QStringList &arguments, const QString &result) {
    QMessageBox messageBox{MainWindow::getInstance()};
    messageBox.setText("Unable to exec [" + cmdToString(program, arguments) + "]\n\n" + result + "\nContinue?");
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    int r = messageBox.exec();
    return r == QMessageBox::Yes;
}

bool Utils::checkRoot() {
    if (geteuid() == 0)
        return true;
    QMessageBox messageBox{MainWindow::getInstance()};
    messageBox.setText("Must be root");
    messageBox.exec();
    return false;
}

void Utils::execInTerminal(const QString &cmd) {
    QProcess::execute("/bin/qterminal", {"-e", "/bin/bash", "-c", cmd});
}