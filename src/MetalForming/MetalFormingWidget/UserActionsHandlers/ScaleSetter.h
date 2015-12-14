#pragma once
#include "MetalFormingStrategy.h"
#include "../WorkingSets/ScaleSetterWorkingSet.h"
#include "../Data/PageFormat/PageFormat.h"

class CScaleSetter : public IMetalFormingStrategy
{
public:
    Q_OBJECT;

public:
    CScaleSetter(CBaseWorkingSet& baseWorkingSet, QWidget *pWorkAreaWidget,
        CScaleSetterWorkingSet &workingSet);

    void increaseScale(int iTimes);
    void decreaseScale(int iTimes);

public slots:
    void resetScale();
    void increaseScale();
    void decreaseScale();

    void onProjectLoaded();

    void onPageFormatChanged(Data::CPageFormat pf);
public:
    virtual void init();
    virtual void deinit();
    virtual bool wheelEventHandler(QWidget *pWidget, QWheelEvent *pEvent);

private:
    Data::CPageFormat m_currentPF;
    int m_iCurrentScaleIndex;
    void updateScaleLabelText();
    void updateWorkAreaSizeAndZeroPoint();
    void updateScaleChangeButtons();

private:
    CScaleSetterWorkingSet &m_workingSet;
};
