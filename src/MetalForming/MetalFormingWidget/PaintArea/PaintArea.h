#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>
#include "ui_PaintArea.h"
#include <QVector>


#include "../UserActionsHandlers/MetalFormingStrategy.h"

#include <QtCore/QQueue>

class CPaintArea : public QWidget
{
    Q_OBJECT

public:
    CPaintArea(QWidget *parent = 0);
    ~CPaintArea();

    void pushStrategy(IMetalFormingStrategy *pStrategy);
    IMetalFormingStrategy *popStrategy();

private:
    Ui::CPaintAreaClass ui;
protected:
    virtual void mousePressEvent(QMouseEvent *pEvent);
    virtual void mouseReleaseEvent(QMouseEvent *pEvent);
    virtual void mouseMoveEvent(QMouseEvent * pEvent); 
    virtual void mouseDoubleClickEvent (QMouseEvent *pEvent);
    virtual void paintEvent(QPaintEvent *pEvent);
    virtual void wheelEvent ( QWheelEvent * pEvent );
    virtual void keyPressEvent ( QKeyEvent * pEvent );

private:
    typedef QQueue <IMetalFormingStrategy *> CStrategiesQueue;
    CStrategiesQueue m_strategies;
    QPoint m_ptPress;
};

#endif // PAINTAREA_H
