#ifndef PARAMSFORM_H
#define PARAMSFORM_H

#include <QWidget>
#include "bgsubtractor.h"

namespace Ui {
class ParamsForm;
}

class ParamsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ParamsForm(QWidget *parent = 0);
    void setBgSubtractor(BgSubtractor*);
    ~ParamsForm();

public slots:
    void updateFrames(cv::Mat frame);

private slots:
    void on_history_spinbox_valueChanged(int arg1);

    void on_varthreshold_spinbox_valueChanged(int arg1);

    void on_openelementsize_spinbox_valueChanged(int arg1);

    void on_closeelementsize_spinbox_valueChanged(int arg1);

private:
    Ui::ParamsForm *ui;
    BgSubtractor* bgSubtractor;
};

#endif // PARAMSFORM_H
