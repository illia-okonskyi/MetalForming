#include "ScaleSetter.h"
#include "ScaleSetterConst.h"

#include <QtCore/QString>
#include <QtGui/QLabel>
#include <QtGui/QWheelEvent>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>

CScaleSetter::CScaleSetter( CBaseWorkingSet& baseWorkingSet,
                           QWidget *pWorkAreaWidget,
                           CScaleSetterWorkingSet &workingSet):
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget),
m_workingSet(workingSet), m_iCurrentScaleIndex(ScaleIndex_Default),
m_currentPF(Data::PageFormat_A5H)
{

}

void CScaleSetter::init()
{
    // Connect toolbar buttons signals
    //
    connect(m_workingSet.m_btnScaleIncrease, SIGNAL(clicked()), this,
        SLOT(increaseScale()));
    connect(m_workingSet.m_btnScaleDecrease, SIGNAL(clicked()), this,
        SLOT(decreaseScale()));
    connect(m_workingSet.m_btnScaleReset, SIGNAL(clicked()), this,
        SLOT(resetScale()));
}

void CScaleSetter::deinit()
{
    disconnect();
}

bool CScaleSetter::wheelEventHandler( QWidget *pWidget, QWheelEvent *pEvent )
{
    if (!(pEvent->modifiers() & Qt::ControlModifier))
        return true;

    int iDegrees = pEvent->delta() / 8;
    int iSteps = iDegrees / 15;

    if (pEvent->orientation() == Qt::Vertical)
    {
        if (iSteps > 0)
            increaseScale(iSteps);
        else
            decreaseScale(-iSteps);
    }
    return false;
}

void CScaleSetter::decreaseScale(int iTimes)
{
    m_iCurrentScaleIndex += iTimes;
    if (m_iCurrentScaleIndex > ScaleIndex_Max)
        m_iCurrentScaleIndex = ScaleIndex_Max;

    m_baseWorkingSet.m_grid.setXScale(g_fScales[m_iCurrentScaleIndex]);
    m_baseWorkingSet.m_grid.setYScale(g_fScales[m_iCurrentScaleIndex]);

    updateScaleChangeButtons();
    updateScaleLabelText();
    updateWorkAreaSizeAndZeroPoint();

    m_pWorkAreaWidget->setFocus();
    m_pWorkAreaWidget->update();
}

void CScaleSetter::increaseScale(int iTimes)
{
    m_iCurrentScaleIndex -= iTimes;
    if (m_iCurrentScaleIndex < ScaleIndex_Min)
        m_iCurrentScaleIndex = ScaleIndex_Min;

    m_baseWorkingSet.m_grid.setXScale(g_fScales[m_iCurrentScaleIndex]);
    m_baseWorkingSet.m_grid.setYScale(g_fScales[m_iCurrentScaleIndex]);

    updateScaleChangeButtons();
    updateScaleLabelText();
    updateWorkAreaSizeAndZeroPoint();

    m_pWorkAreaWidget->setFocus();
    m_pWorkAreaWidget->update();
}

void CScaleSetter::decreaseScale()
{
    return decreaseScale(1);
}

void CScaleSetter::increaseScale()
{
    return increaseScale(1);
}

void CScaleSetter::resetScale()
{
    m_iCurrentScaleIndex = ScaleIndex_Default;
    m_baseWorkingSet.m_grid.setXScale(g_fScales[m_iCurrentScaleIndex]);
    m_baseWorkingSet.m_grid.setYScale(g_fScales[m_iCurrentScaleIndex]);

    updateScaleChangeButtons();
    updateScaleLabelText();
    updateWorkAreaSizeAndZeroPoint();

    m_pWorkAreaWidget->setFocus();
    m_pWorkAreaWidget->update();
}

void CScaleSetter::updateScaleLabelText()
{
    m_workingSet.m_labelScale->setText(
        QString(tr("X(%1 mm), Y(%2 mm)")).
        arg(m_baseWorkingSet.m_grid.getXScale()).
        arg(m_baseWorkingSet.m_grid.getYScale()));
}

void CScaleSetter::onProjectLoaded()
{
    resetScale();
}

void CScaleSetter::onPageFormatChanged( Data::CPageFormat pf )
{
    m_currentPF = pf;
    updateWorkAreaSizeAndZeroPoint();
}

void CScaleSetter::updateWorkAreaSizeAndZeroPoint()
{
    QSize pageSize;
    double fWidth = 0;
    double fHeight = 0;

    // Settings page size in milimeters
    //
    switch (m_currentPF)
    {
    case Data::PageFormat_A5H:
        fWidth = 210.0f;
        fHeight = 148.0f;
        break;
    case Data::PageFormat_A5V:
        fWidth = 148.0f;
        fHeight = 210.0f;
        break;
    case Data::PageFormat_A4H:
        fWidth = 297.0f;
        fHeight = 210.0f;
        break;
    case Data::PageFormat_A4V:
        fWidth = 210.0f;
        fHeight = 297.0f;
        break;
    case Data::PageFormat_A3H:
        fWidth = 420.0f;
        fHeight = 297.0f;
        break;
    case Data::PageFormat_A3V:
        fWidth = 297.0f;
        fHeight = 420.0f;
        break;
    }

    // Apply grid step to the new size
    //
    fWidth *= m_baseWorkingSet.m_grid.getXStep();
    fHeight *= m_baseWorkingSet.m_grid.getYStep();

    // Apply grid scale to the new size
    //
    fWidth /= m_baseWorkingSet.m_grid.getXScale();
    fHeight /= m_baseWorkingSet.m_grid.getYScale();

    // Calc new zero point coords
    //
    double fNewPtZeroX = m_baseWorkingSet.m_grid.getZero().x();
    double fNewPtZeroY = m_baseWorkingSet.m_grid.getZero().y();
    QSize szOldWorkAreaSize = m_pWorkAreaWidget->minimumSize();
    fNewPtZeroX *= fWidth / double(szOldWorkAreaSize.width());
    fNewPtZeroY *= fHeight / double(szOldWorkAreaSize.height());
    QPoint ptNewZeroPoint = QPointF(fNewPtZeroX, fNewPtZeroY).toPoint();
    m_baseWorkingSet.m_grid.setZero(ptNewZeroPoint);

    // Apply new size to the WokrAreaWidget
    //
    m_pWorkAreaWidget->setMinimumSize(QSize(fWidth, fHeight));
    m_pWorkAreaWidget->setMaximumSize(QSize(fWidth, fHeight));

    // Move scroll area view to zero point
    //
    m_workingSet.m_scrollArea->ensureVisible(ptNewZeroPoint.x(),
        ptNewZeroPoint.y(), m_workingSet.m_scrollArea->size().width() / 2,
        m_workingSet.m_scrollArea->size().height() / 2);
}

void CScaleSetter::updateScaleChangeButtons()
{
    m_workingSet.m_btnScaleDecrease->setEnabled(
        m_iCurrentScaleIndex != ScaleIndex_Max);
    m_workingSet.m_btnScaleIncrease->setEnabled(
        m_iCurrentScaleIndex != ScaleIndex_Min);
    m_workingSet.m_btnScaleReset->setEnabled(
        m_iCurrentScaleIndex != ScaleIndex_Default);
}
