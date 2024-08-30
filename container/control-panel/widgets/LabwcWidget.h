#ifndef LABWCWIDGET_H
#define LABWCWIDGET_H

#include <QWidget>

namespace Ui {
    class LabwcWidget;
}

class LabwcWidget : public QWidget {
Q_OBJECT

public:
    explicit LabwcWidget(QWidget *parent = nullptr);

    ~LabwcWidget();

private:
    Ui::LabwcWidget *ui;

    void onClickSave();
};

#endif // LABWCWIDGET_H
