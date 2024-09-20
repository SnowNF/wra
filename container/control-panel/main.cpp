#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    // set up env var for start from lxqt-sudo
    for (int i = 0; i < argc; ++i) {
        QString arg(argv[i]);
        if (arg.contains("=")) {
            putenv(argv[i]);
        }
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return QApplication::exec();
}
