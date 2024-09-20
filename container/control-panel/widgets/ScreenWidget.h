#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <QWidget>
#include <QProcess>
#include "listeners/ReadEventListener.h"

namespace Ui {
    class ScreenWidget;
}

class ScreenWidget : public QWidget {
Q_OBJECT

public:
    explicit ScreenWidget(QWidget *parent = nullptr);

    ~ScreenWidget();

private:
    static constexpr char EVENT_TRIGGER_CFG[] = "event-trigger";
    Ui::ScreenWidget *ui;
    qint64 kbdPid = 0;
    QStringList eventPath;

    ReadEventListener::Trigger trigger = []() {
        QProcess::startDetached("wlopm", {"--toggle", "*"});
    };

    ReadEventListener listener{trigger};

    void onClickKbd();

    void updateKbdText();

    void initEventPath();

    void onEventPathSelected(int index);
};

#endif // SCREENWIDGET_H
