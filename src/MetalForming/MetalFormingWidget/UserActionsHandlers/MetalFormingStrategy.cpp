#include "MetalFormingStrategy.h"

IMetalFormingStrategy::IMetalFormingStrategy(
    CBaseWorkingSet &baseWorkingSet, QWidget *pWorkAreaWidget) :
m_baseWorkingSet(baseWorkingSet), m_pWorkAreaWidget(pWorkAreaWidget)
{

}

void IMetalFormingStrategy::init()
{

}

void IMetalFormingStrategy::deinit()
{

}

bool IMetalFormingStrategy::mousePressEventHandler(QWidget *pWidget,
                                                   QMouseEvent *pEvent )
{
    return true;
}

bool IMetalFormingStrategy::mouseReleaseEventHandler(QWidget *pWidget,
                                                     QMouseEvent *pEvent )
{
    return true;
}

bool IMetalFormingStrategy::mouseMoveEventHandler(QWidget *pWidget,
                                                  QMouseEvent * pEvent )
{
    return true;
}

bool IMetalFormingStrategy::mouseDoubleClickEventHandler( QWidget *pWidget,
                                                         QMouseEvent *pEvent )
{
    return true;
}


bool IMetalFormingStrategy::mouseClickEventHandler( QWidget *pWidget,
                                                   QMouseEvent *pEvent )
{
    return true;
}

bool IMetalFormingStrategy::paintEventHandler( QWidget *pWidget,
                                              QPaintEvent *pEvent )
{
    return true;
}

bool IMetalFormingStrategy::wheelEventHandler( QWidget *pWidget,
                                              QWheelEvent *pEvent )
{
    return true;
}

bool IMetalFormingStrategy::keyPressEventHandler( QWidget *pWidget,
                                                 QKeyEvent * pEvent )
{
    return true;
}