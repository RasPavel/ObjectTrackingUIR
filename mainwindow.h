#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <player.h>
#include "paramsform.h"
#include "msparamsform.h"
#include "bgsubtractor.h"
#include "camshifttracker.h"
#include "particlefiltertracker.h"

#include <QElapsedTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void processFrame(cv::Mat frame);
    void updatePlayerUI(QImage img);
    void initCamshiftTracker(QRect rect);
    void initPFTracker(QRect rect);
    void on_load_button_clicked();
    void on_play_button_clicked();


    void on_bgs_params_clicked();
    void on_ms_params_clicked();

    void on_webcam_button_clicked();

    void on_pf_checkbox_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    ParamsForm *bg_params_ui;
    MsParamsForm *cs_params_ui;
    Player* myPlayer;
    BgSubtractor* bgSubtractor;
    CamShiftTracker* csTracker;
    ParticleFilterTracker* pfTracker;

    bool initialized;
    bool usingAlgCombination;
    bool pf_tracker_enabled;

    cv::Mat mask;
    cv::Mat backproj;
    cv::Mat track;

    QElapsedTimer timer;
};

#endif // MAINWINDOW_H
