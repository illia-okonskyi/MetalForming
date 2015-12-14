#pragma once
#include "MetalFormingStrategy.h"
#include "../WorkingSets/PropertiesSetterWorkingSet.h"

#include <QtSolutions/qtvariantproperty>

class CPropertiesSetter : public IMetalFormingStrategy
{
public:
    Q_OBJECT;

public:
    CPropertiesSetter(CBaseWorkingSet &baseWorkingSet, QWidget *pWorkAreaWidget,
        CPropertiesSetterWorkingSet &workingSet);

    virtual void init();
    virtual void deinit();

    virtual bool mousePressEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool mouseMoveEventHandler(QWidget *pWidget, QMouseEvent * pEvent); 
    virtual bool keyPressEventHandler(QWidget *pWidget, QKeyEvent * pEvent);

    virtual bool paintEventHandler(QWidget *pWidget, QPaintEvent *pEvent);

protected:
    CPropertiesSetterWorkingSet &m_workingSet;
signals:
    void dataReady();

protected slots:
    void onPropertyValueChanged(QtProperty *pProperty, const QVariant &value );
    void onCompileData();

private:
    typedef QMap<QtVariantProperty *,
        Data::CWPProperties::CWPPropsIndexes> CPropertiesMap;

    CPropertiesMap m_propertiesMap;

    void createPropertiesList();
    void setStartupPropertiesValues();
};