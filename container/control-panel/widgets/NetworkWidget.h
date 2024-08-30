#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>

namespace Ui {
class NetworkWidget;
}

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkWidget(QWidget *parent = nullptr);
    ~NetworkWidget();

private:
    Ui::NetworkWidget *ui;

    static void onCLickFixSocketPermission();
};

#endif // NETWORKWIDGET_H
