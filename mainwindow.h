#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <player.h>
#include "paramsform.h"
#include "msparamsform.h"
#include "bgsubtractor.h"
#include "meanshifttracker.h"
#include "particlefiltertracker.h"

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
    void initMeanshiftTracker(QRect rect);
    void initPFTracker(QRect rect);
    void on_load_button_clicked();
    void on_play_button_clicked();


    void on_bgs_params_clicked();
    void on_ms_params_clicked();

private:
    Ui::MainWindow *ui;

    ParamsForm *bg_params_ui;
    MsParamsForm *ms_params_ui;
    Player* myPlayer;
    BgSubtractor* bgSubtractor;
    MeanShiftTracker* msTracker;
    ParticleFilterTracker* pfTracker;

    bool initialized;

    bool usingAlgCombination;
    cv::Mat mask;
    cv::Mat backproj;
    cv::Mat track;
};

#endif // MAINWINDOW_H
