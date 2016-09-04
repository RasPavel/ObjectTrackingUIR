#ifndef MSPARAMSFORM_H
#define MSPARAMSFORM_H

#include <QWidget>
#include "meanshifttracker.h"
#include "bgsubtractor.h"

namespace Ui {
class MsParamsForm;
}

class MsParamsForm : public QWidget
{
    Q_OBJECT

public:
    explicit MsParamsForm(QWidget *parent = 0);
    void setMsTracker(MeanShiftTracker**);
    void setBgSubtractor(BgSubtractor**);
    ~MsParamsForm();

public slots:
    void updateFrames();

private slots:
    void on_hbins_spinbox_valueChanged(int);
    void on_sbins_spinbox_valueChanged(int);
    void on_vbins_spinbox_valueChanged(int);
    void on_alpha_slider_sliderMoved(int position);

private:
    Ui::MsParamsForm *ui;
    MeanShiftTracker** msTracker;
    BgSubtractor** bgSubtractor;
};

#endif // MSPARAMSFORM_H
