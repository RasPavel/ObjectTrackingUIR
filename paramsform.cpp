#include "paramsform.h"
#include "ui_paramsform.h"

ParamsForm::ParamsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParamsForm)
{
    qRegisterMetaType< cv::Mat >("cv::Mat");
    ui->setupUi(this);
}

void ParamsForm::updateFrames(cv::Mat frame)
{
    Mat mask = *(bgSubtractor->getMask());
    Mat morph_open = bgSubtractor->mask_open;
    Mat morph_close = bgSubtractor->mask_close;

    QImage mask_img = QImage((uchar*) mask.data, mask.cols, mask.rows, mask.step, QImage::Format_Grayscale8);
    QPixmap mask_pix = QPixmap::fromImage(mask_img);

    QImage morph1_img = QImage((uchar*) morph_open.data, morph_open.cols, morph_open.rows, morph_open.step, QImage::Format_Grayscale8);
    QPixmap morph1_pix = QPixmap::fromImage(morph1_img);

    QImage morph2_img = QImage((uchar*) morph_close.data, morph_close.cols, morph_close.rows, morph_close.step, QImage::Format_Grayscale8);
    QPixmap morph2_pix = QPixmap::fromImage(morph2_img);

    ui->label1->setPixmap(mask_pix.scaled(ui->label1->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->label2->setPixmap(morph1_pix.scaled(ui->label2->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
    ui->label3->setPixmap(morph2_pix.scaled(ui->label3->size(), Qt::IgnoreAspectRatio, Qt::FastTransformation));
}


ParamsForm::~ParamsForm()
{
    delete ui;
}

void ParamsForm::setBgSubtractor(BgSubtractor* b)
{
    bgSubtractor = b;
}

void ParamsForm::on_history_spinbox_valueChanged(int h)
{
    bgSubtractor->setHistory(h);
}

void ParamsForm::on_varthreshold_spinbox_valueChanged(int vt)
{
    bgSubtractor->setVarThreshold(vt);
}


void ParamsForm::on_openelementsize_spinbox_valueChanged(int s)
{
    bgSubtractor->setOpenElementSize(s);
}



void ParamsForm::on_closeelementsize_spinbox_valueChanged(int s)
{
    bgSubtractor->setCloseElementSize(s);
}
