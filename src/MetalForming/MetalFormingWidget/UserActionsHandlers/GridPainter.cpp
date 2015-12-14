#include "GridPainter.h"
#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtGui/QPen>

CGridPainter::CGridPainter( CBaseWorkingSet& baseWorkingSet,
                           QWidget *pWorkAreaWidget):
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget)
{

}

bool CGridPainter::paintEventHandler(QWidget *pWidget, QPaintEvent *pEvent )
{
    QPainter painter(pWidget);
    int iXStep = m_baseWorkingSet.m_grid.getXStep();
    int iYStep = m_baseWorkingSet.m_grid.getYStep();
    QPoint ptZero = m_baseWorkingSet.m_grid.getZero();

    int iWhdWidth = pWidget->size().width();
    int iWhdHeight = pWidget->size().height();
    QPen oldPen(painter.pen());

    QPen gridPen(Qt::gray, 1, Qt::DashLine);
    QPen zeroPen(Qt::black, 2);

    // 1) Drawning default grid lines
    //
    painter.setPen(gridPen);    
    for (int i = 0; i <= iWhdWidth; i += iXStep)
        painter.drawLine(i, 0, i, iWhdHeight);

    for (int i = 0; i <= iWhdHeight; i += iYStep)
        painter.drawLine(0, i, iWhdWidth, i);

    // 2) Drawning zero grid lines
    //
    painter.setPen(zeroPen);
    painter.drawLine(0, ptZero.y(), iWhdWidth, ptZero.y());
    painter.drawLine(ptZero.x(), 0, ptZero.x(), iWhdHeight);

    painter.setPen(oldPen);

    return true;
}