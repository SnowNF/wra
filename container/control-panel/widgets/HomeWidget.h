#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>

namespace Ui {
    class HomeWidget;
}

class HomeWidget : public QWidget {
Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);

    ~HomeWidget();

private:
    Ui::HomeWidget *ui;

    static void onClickStartRootMode();
};

#endif // HOMEWIDGET_H
