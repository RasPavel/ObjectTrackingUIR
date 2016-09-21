#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QObject>
#include <QWidget>
#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(QWidget *parent = 0);
//    ClickableLabel(QWidget *parent = 0) : QLabel(parent) {};
private:
    bool selectionStarted;
    QRect selectionRect;
signals:
    void selected(QRect rect);
protected:
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
};

#endif // CLICKABLELABEL_H
