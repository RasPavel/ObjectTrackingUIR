#-------------------------------------------------
#
# Project created by QtCreator 2016-04-26T19:32:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ObjectTrackingUIR
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
#LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui  -lopencv_videoio -lopencv_imgproc
LIBS += -L/usr/local/lib -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired -lopencv_ccalib -lopencv_cvv -lopencv_dnn -lopencv_dpm -lopencv_fuzzy -lopencv_hdf -lopencv_line_descriptor -lopencv_optflow -lopencv_plot -lopencv_reg -lopencv_saliency -lopencv_stereo -lopencv_structured_light -lopencv_rgbd -lopencv_viz -lopencv_surface_matching -lopencv_tracking -lopencv_datasets -lopencv_text -lopencv_face -lopencv_xfeatures2d -lopencv_shape -lopencv_video -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_xobjdetect -lopencv_objdetect -lopencv_ml -lopencv_xphoto -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo -lopencv_imgproc -lopencv_core -lQt5Test -lQt5Concurrent -lQt5OpenGL -lpng -ltiff -ljasper -ljpeg -lImath -lIlmImf -lIex -lHalf -lIlmThread -ldc1394 -lavcodec-ffmpeg -lavformat-ffmpeg -lavutil-ffmpeg -lswscale-ffmpeg -lQt5Core -lQt5Gui -lQt5Widgets -lpthread -lz -ldl -lm -lvtkCommon -lvtkFiltering -lvtkImaging -lvtkGraphics -lvtkGenericFiltering -lvtkIO -lvtkRendering -lvtkVolumeRendering -lvtkHybrid -lvtkWidgets -lvtkParallel -lvtkInfovis -lvtkGeovis -lvtkViews -lvtkCharts -lrt -lGLU -lGL -ltbb -latomic


SOURCES += main.cpp\
        mainwindow.cpp \
    player.cpp

HEADERS  += mainwindow.h \
    player.h

FORMS    += mainwindow.ui
