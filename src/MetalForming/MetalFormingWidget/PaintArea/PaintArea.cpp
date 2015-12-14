#include "PaintArea.h"
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <cassert>

CPaintArea::CPaintArea(QWidget *parent)
: QWidget(parent)
{
    ui.setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
}

CPaintArea::~CPaintArea()
{

}
void CPaintArea::pushStrategy( IMetalFormingStrategy *pStrategy )
{   
    pStrategy->init();
    m_strategies.push_back(pStrategy);
}

IMetalFormingStrategy * CPaintArea::popStrategy()
{
    if (m_strategies.empty()) return 0;

    IMetalFormingStrategy *pStrategy = m_strategies.back();
    m_strategies.pop_back();
    pStrategy->deinit();
    return pStrategy;
}

void CPaintArea::mousePressEvent( QMouseEvent *pEvent )
{
    m_ptPress = pEvent->pos();
    CStrategiesQueue::iterator it = m_strategies.begin();
    CStrategiesQueue::iterator itLast = m_strategies.end();
    bool bNeedToCallNextHandler = true;
    for (; it != itLast; ++it)
    {
        bNeedToCallNextHandler = (*it)->mousePressEventHandler(this, pEvent);
        if (!bNeedToCallNextHandler) return;
    }

    if (bNeedToCallNextHandler)
        QWidget::mousePressEvent(pEvent);
}

void CPaintArea::mouseReleaseEvent( QMouseEvent * pEvent )
{  
    CStrategiesQueue::iterator it = m_strategies.begin();
    CStrategiesQueue::iterator itLast = m_strategies.end();
    bool bNeedToCallNextHandler = true;
    for (; it != itLast; ++it)
    {
        bNeedToCallNextHandler = (*it)->mouseReleaseEventHandler(this, pEvent);
        if (!bNeedToCallNextHandler) return;
    }
    if (bNeedToCallNextHandler)
        QWidget::mouseReleaseEvent(pEvent);

    if (m_ptPress == pEvent->pos())
    {
        for (it = m_strategies.begin(); it != itLast; ++it)
        {
            bNeedToCallNextHandler = (*it)->mouseClickEventHandler(this, pEvent);
            if (!bNeedToCallNextHandler) return;
        }
    }
}

void CPaintArea::mouseMoveEvent( QMouseEvent * pEvent )
{
    m_ptPress.setX(-1);
    m_ptPress.setY(-1);
    CStrategiesQueue::iterator it = m_strategies.begin();
    CStrategiesQueue::iterator itLast = m_strategies.end();
    bool bNeedToCallNextHandler = true;
    for (; it != itLast; ++it)
    {
        bNeedToCallNextHandler = (*it)->mouseMoveEventHandler(this, pEvent);
        if (!bNeedToCallNextHandler) return;
    }

    if (bNeedToCallNextHandler)
        QWidget::mouseMoveEvent(pEvent);
}

void CPaintArea::wheelEvent( QWheelEvent * pEvent )
{
    CStrategiesQueue::iterator it = m_strategies.begin();
    CStrategiesQueue::iterator itLast = m_strategies.end();
    bool bNeedToCallNextHandler = true;
    for (; it != itLast; ++it)
    {
        bNeedToCallNextHandler = (*it)->wheelEventHandler(this, pEvent);
        if (!bNeedToCallNextHandler) return;
    }

    if (bNeedToCallNextHandler)
        QWidget::wheelEvent(pEvent);
}
void CPaintArea::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    CStrategiesQueue::iterator it = m_strategies.begin();
    CStrategiesQueue::iterator itLast = m_strategies.end();
    bool bNeedToCallNextHandler = true;
    for (; it != itLast; ++it)
    {
        bNeedToCallNextHandler = (*it)->mouseDoubleClickEventHandler(this, pEvent);
        if (!bNeedToCallNextHandler) return;
    }

    if (bNeedToCallNextHandler)
        QWidget::mouseDoubleClickEvent(pEvent);
}

void CPaintArea::paintEvent( QPaintEvent *pEvent )
{
    CStrategiesQueue::iterator it = m_strategies.begin();
    CStrategiesQueue::iterator itLast = m_strategies.end();
    bool bNeedToCallNextHandler = true;
    for (; it != itLast; ++it)
    {
        bNeedToCallNextHandler = (*it)->paintEventHandler(this, pEvent);
        if (!bNeedToCallNextHandler) return;
    }

    if (bNeedToCallNextHandler)
        QWidget::paintEvent(pEvent);
}

void CPaintArea::keyPressEvent( QKeyEvent * pEvent )
{
    CStrategiesQueue::iterator it = m_strategies.begin();
    CStrategiesQueue::iterator itLast = m_strategies.end();
    bool bNeedToCallNextHandler = true;
    for (; it != itLast; ++it)
    {
        bNeedToCallNextHandler = (*it)->keyPressEventHandler(this, pEvent);
        if (!bNeedToCallNextHandler) return;
    }

    if ((bNeedToCallNextHandler))
        QWidget::keyPressEvent(pEvent);
}