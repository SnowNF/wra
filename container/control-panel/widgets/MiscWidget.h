#ifndef MISCWIDGET_H
#define MISCWIDGET_H

#include <QWidget>

namespace Ui {
class MiscWidget;
}

class MiscWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MiscWidget(QWidget *parent = nullptr);
    ~MiscWidget();

private:
    Ui::MiscWidget *ui;

    static void onClickStartSystemd();
};

#endif // MISCWIDGET_H
