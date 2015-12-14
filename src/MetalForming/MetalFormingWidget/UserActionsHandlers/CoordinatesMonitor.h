#pragma once
#include "MetalFormingStrategy.h"
#include "../WorkingSets/CoordinatesMonitorWorkingSet.h"

class CCoordinatesMonitor : public IMetalFormingStrategy
{
    Q_DECLARE_TR_FUNCTIONS(CCoordinatesMonitor)

public:
    CCoordinatesMonitor(CBaseWorkingSet &baseWorkingSet,
        QWidget *pWorkAreaWidget, CCoordinatesMonitorWorkingSet &workingSet);

    virtual void init();
    virtual void deinit();

    virtual bool mouseMoveEventHandler (QWidget *pWidget, QMouseEvent * pEvent);
protected:
    CCoordinatesMonitorWorkingSet &m_workingSet;
};