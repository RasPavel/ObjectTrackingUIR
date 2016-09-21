#ifndef MSPARAMSFORM_H
#define MSPARAMSFORM_H

#include <QWidget>
#include "camshifttracker.h"
#include "bgsubtractor.h"

namespace Ui {
class MsParamsForm;
}

class MsParamsForm : public QWidget
{
    Q_OBJECT

public:
    explicit MsParamsForm(QWidget *parent = 0);
    void setCsTracker(CamShiftTracker**);
    void setBgSubtractor(BgSubtractor**);
    ~MsParamsForm();
private:
    Ui::MsParamsForm *ui;
    CamShiftTracker **csTracker;
    BgSubtractor **bgSubtractor;
public slots:
    void updateFrames();
private slots:
    void on_hbins_spinbox_valueChanged(int);
    void on_sbins_spinbox_valueChanged(int);
    void on_vbins_spinbox_valueChanged(int);
    void on_alpha_slider_sliderMoved(int position);
    void on_thresh_spinbox_valueChanged(int);

};

#endif // MSPARAMSFORM_H
