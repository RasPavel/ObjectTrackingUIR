#include "msparamsform.h"
#include "ui_msparamsform.h"

MsParamsForm::MsParamsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MsParamsForm)
{
    ui->setupUi(this);
}

void MsParamsForm::updateFrames()
{
    if (!*csTracker) {
        return;
    }
    Mat backproj = (*csTracker)->getBackProjection();
    Mat heatmap = (*csTracker)->getHeatmap();
    Mat bgmask = *((*bgSubtractor)->getMask());

    QImage backproj_img = QImage((uchar*) backproj.data, backproj.cols, backproj.rows, backproj.step, QImage::Format_Grayscale8);
    QPixmap backproj_pix = QPixmap::fromImage(backproj_img);

    QImage bgmask_img = QImage((uchar*) bgmask.data, bgmask.cols, bgmask.rows, bgmask.step, QImage::Format_Grayscale8);
    QPixmap bgmask_pix = QPixmap::fromImage(bgmask_img);

    QImage heatmap_img = QImage((uchar*) heatmap.data, heatmap.cols, heatmap.rows, heatmap.step, QImage::Format_Grayscale8);
    QPixmap heatmap_pix = QPixmap::fromImage(heatmap_img);

    ui->label_backproj->setPixmap(backproj_pix.scaled(ui->label_backproj->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->label_bgmask->setPixmap(bgmask_pix.scaled(ui->label_bgmask->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->label_heatmap->setPixmap(heatmap_pix.scaled(ui->label_heatmap->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

MsParamsForm::~MsParamsForm()
{
    delete ui;
}

void MsParamsForm::setCsTracker(CamShiftTracker** mt)
{
    csTracker = mt;
}

void MsParamsForm::setBgSubtractor(BgSubtractor** bgs)
{
    bgSubtractor = bgs;
}

void MsParamsForm::on_hbins_spinbox_valueChanged(int h)
{
    (*csTracker)->setHbins(h);
}

void MsParamsForm::on_sbins_spinbox_valueChanged(int s)
{
    (*csTracker)->setSbins(s);
}

void MsParamsForm::on_vbins_spinbox_valueChanged(int v)
{
    (*csTracker)->setVbins(v);
}

void MsParamsForm::on_alpha_slider_sliderMoved(int position)
{
    (*csTracker)->setAlpha((double) position / 100.0);
}

void MsParamsForm::on_thresh_spinbox_valueChanged(int thresh)
{
    (*csTracker)->setThreshold(thresh);
}
