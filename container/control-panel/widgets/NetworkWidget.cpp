#include "NetworkWidget.h"
#include "ui_NetworkWidget.h"
#include "Utils.h"
#include <mainwindow.h>
#include <QFile>
#include <iostream>
#include <thread>

NetworkWidget::NetworkWidget(QWidget *parent)
        : QWidget(parent), ui(new Ui::NetworkWidget) {
    ui->setupUi(this);
    connect(ui->socketButton, &QPushButton::clicked, this, &NetworkWidget::onCLickFixSocketPermission);
}

NetworkWidget::~NetworkWidget() {
    delete ui;
}

static bool msg(const QString &prog, const QStringList &arg, const QString &result) {
    bool ret;
    MainWindow::getInstance()->runOnUiThreadBlocked([&] {
        ret = Utils::unableToExecContinueMsgBox(prog, arg, result);
    });
    return ret;
}

static bool groupAdd(const QString &id, const QString &name) {
    QString result;
    QString prog = "/usr/sbin/groupadd";
    QStringList arg = {"-g", id, name};
    int r = Utils::exec(prog, arg, result);
    if (r != 0 && r != 9) {
        return msg(prog, arg, result);
    }
    return true;
}

static bool userMod(const QString &groupName, const QString &userName) {
    QString result;
    QString prog = "/usr/sbin/usermod";
    QStringList arg = {"-a", "-G", groupName, userName};
    int r = Utils::exec(prog, arg, result);
    if (r != 0 && r != 9) {
        return msg(prog, arg, result);
    }
    return true;
}

static bool readAllUsers(QStringList &users) {
    QString path = "/etc/passwd";
    QFile passwd(path);
    if (!passwd.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Failed to open file:" << path.toStdString();
        return false;
    }
    QString content = passwd.readAll();
    QStringList lines = content.split("\n", Qt::SkipEmptyParts);
    for (const auto &line: lines) {
        QStringList parts = line.split(':');
        if (!parts.empty()) {
            users.append(parts.at(0));
        }
    }
    return true;
}

void NetworkWidget::onCLickFixSocketPermission() {
    if (!Utils::checkRoot())
        return;
    int i = MainWindow::getInstance()->addStatusStrA("Performing");
    std::thread([i] {
        //https://android.googlesource.com/platform/system/core/+/master/libcutils/include/private/android_filesystem_config.h
        struct {
            QString id;
            QString name;
        } cmd[] = {
                {"3001", "aid_net_bt_admin"},
                {"3002", "aid_net_bt"},
                {"3003", "aid_inet"},
                {"3004", "aid_net_raw"},
                {"3005", "aid_net_admin"},
                {"3006", "aid_net_bw_stats"},
                {"3007", "aid_net_bw_acct"},
                {"3009", "aid_readproc"},
                {"3010", "aid_wakelock"},
                {"3011", "aid_uhid"},
                {"3012", "aid_readtracefs"},
                {"3013", "aid_virtualmachine"}
        };
        QStringList users;
        for (const auto &item: cmd) {
            if (!groupAdd(item.id, item.name))
                goto end;
        }
        readAllUsers(users);
        for (const auto &user: users) {
            for (const auto &group: cmd) {
                if (!userMod(group.name, user))
                    goto end;
            }
        }
        end:
        MainWindow::getInstance()->runOnUiThread([i] {
            MainWindow::getInstance()->removeStatusStrA(i, "Finished");
        });
    }).detach();
}