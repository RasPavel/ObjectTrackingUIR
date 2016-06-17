#include "clickablelabel.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>

ClickableLabel::ClickableLabel(QWidget *parent) : QLabel(parent)
{
    selectionStarted = false;
}

void ClickableLabel::mousePressEvent(QMouseEvent *e)
{
    selectionStarted = true;
    selectionRect.setTopLeft(e->pos());
    selectionRect.setBottomRight(e->pos());
}

void ClickableLabel::mouseMoveEvent(QMouseEvent *e)
{
    if (selectionStarted) {
        selectionRect.setBottomRight(e->pos());
        repaint();
    }
}

void ClickableLabel::mouseReleaseEvent(QMouseEvent *e)
{
    selectionStarted = false;
    emit selected(selectionRect);
}

void ClickableLabel::paintEvent(QPaintEvent *e)
{
    QLabel::paintEvent(e);
    QPainter painter(this);
    painter.setPen(QPen(QBrush(QColor(0,0,0,180)),1,Qt::DashLine));
    painter.setBrush(QBrush(QColor(255,255,255,120)));

    painter.drawRect(selectionRect);
}
