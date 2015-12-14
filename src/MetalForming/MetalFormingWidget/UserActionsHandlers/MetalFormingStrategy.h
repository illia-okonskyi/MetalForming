#pragma once

#include "../WorkingSets/BaseWorkingSet.h"
class QWidget;
class QMouseEvent;
class QPaintEvent;
class QWheelEvent;
class QKeyEvent;

#include <QtCore/QObject>

class IMetalFormingStrategy : public QObject
{
public:
    IMetalFormingStrategy(CBaseWorkingSet &baseWorkingSet,
        QWidget *pWorkAreaWidget);

    // Do nothing basically.
    virtual void init();
    virtual void deinit();

    // Do nothing basically returns true if it is need to call next handlers and
    // false otherwise.
    virtual bool mousePressEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool mouseReleaseEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool mouseMoveEventHandler(QWidget *pWidget, QMouseEvent * pEvent); 
    virtual bool mouseDoubleClickEventHandler(QWidget *pWidget,
        QMouseEvent *pEvent);
    virtual bool mouseClickEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool wheelEventHandler(QWidget *pWidget, QWheelEvent *pEvent);
    virtual bool keyPressEventHandler(QWidget *pWidget, QKeyEvent * pEvent);

    virtual bool paintEventHandler(QWidget *pWidget, QPaintEvent *pEvent);
    
protected:
    CBaseWorkingSet &m_baseWorkingSet;
    QWidget *m_pWorkAreaWidget;
};