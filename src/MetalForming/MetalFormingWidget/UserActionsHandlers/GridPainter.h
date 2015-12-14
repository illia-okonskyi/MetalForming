#pragma once
#include "MetalFormingStrategy.h"

class CGridPainter : public IMetalFormingStrategy
{
public:
    CGridPainter(CBaseWorkingSet& baseWorkingSet, QWidget *pWorkAreaWidget);

    virtual bool paintEventHandler(QWidget *pWidget, QPaintEvent *pEvent);
};
