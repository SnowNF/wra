#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

#define ANY_THREAD
#define UI_THREAD

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void runOnUiThreadBlocked(const std::function<void()> &function);

signals:

    void runOnUiThread(const std::function<void()> &function);

private:
    Ui::MainWindow *ui;
    QMap<int, QLabel *> statusLabels;

    static void doRunOnUiThread(const std::function<void()> &function);

public:
    void replaceStatusStr(int index, const QString &string);

    int addStatusStr(const QString &string);

    int addStatusStrA(const QString &string);

    void removeStatusStrA(int index, const QString &finishStr, int timeout = 2);

    void replaceStatusStrA(int index, const QString &string);

    void removeStatusStr(int index, const QString &qString, int timeout = 2);

    static MainWindow *getInstance();
};

static MainWindow *instance;

#endif // MAINWINDOW_H
