#include "CoordinatesMonitor.h"
#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtCore/QString>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
CCoordinatesMonitor::CCoordinatesMonitor(
    CBaseWorkingSet &baseWorkingSet,
    QWidget *pWorkAreaWidget,
    CCoordinatesMonitorWorkingSet &workingSet ) :
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget), m_workingSet(workingSet)
{

}
bool CCoordinatesMonitor::mouseMoveEventHandler( QWidget *pWidget,
                                                QMouseEvent * pEvent )
{
    QPointF ptRealCoord;
    QPoint ptSreenCoord = pEvent->pos();
    m_baseWorkingSet.m_grid.bindScreenPointToGrid(ptSreenCoord);
    m_baseWorkingSet.m_grid.transformScreenPointToRealPoint(ptSreenCoord,
        ptRealCoord);

    m_workingSet.m_labelXPos->setText(
        QString(tr("%1 mm")).arg(ptRealCoord.x()));
    m_workingSet.m_labelYPos->setText(
        QString(tr("%1 mm")).arg(ptRealCoord.y()));
    return true;
}

void CCoordinatesMonitor::init()
{
    m_workingSet.m_gbWS->setVisible(true);
    m_workingSet.m_labelXPos->setText(tr("---"));
    m_workingSet.m_labelYPos->setText(tr("---"));
}

void CCoordinatesMonitor::deinit()
{
    m_workingSet.m_gbWS->setVisible(false);
}