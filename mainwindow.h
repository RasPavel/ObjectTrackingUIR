#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <player.h>
#include "bgsubtractor.h"
#include "meanshifttracker.h"

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
    void on_load_button_clicked();
    void on_play_button_clicked();

private:
    Ui::MainWindow *ui;
    Player* myPlayer;
    BgSubtractor* bgSubtractor;
    MeanShiftTracker* msTracker;

    bool usingAlgCombination;
    cv::Mat mask;
    cv::Mat backproj;
};

#endif // MAINWINDOW_H
