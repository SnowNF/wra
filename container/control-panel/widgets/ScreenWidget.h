#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <QWidget>
#include <QProcess>

namespace Ui {
    class ScreenWidget;
}

class ScreenWidget : public QWidget {
Q_OBJECT

public:
    explicit ScreenWidget(QWidget *parent = nullptr);

    ~ScreenWidget();

private:
    Ui::ScreenWidget *ui;
    qint64 kbdPid = 0;;

    void onClickKbd();

    void updateKbdText();
};

#endif // SCREENWIDGET_H
