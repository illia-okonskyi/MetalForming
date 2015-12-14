#include "PropertiesSetter.h"
#include "../Data/WPProperties/WPPropertiesStringConsts.h"
#include "../Data/PageFormat/PageFormatStringConsts.h"

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QPainter>
#include <QtGui/QPen>

#include <QtSolutions/qtpropertymanager.h>
#include <QtSolutions/qttreepropertybrowser>

CPropertiesSetter::CPropertiesSetter(CBaseWorkingSet &baseWorkingSet,
                                     QWidget *pWorkAreaWidget,
                                     CPropertiesSetterWorkingSet &workingSet):
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget), m_workingSet(workingSet)
{

}

void CPropertiesSetter::init()
{
    m_workingSet.m_propertiesManager->clear();

    // Generating properties list.
    //
    createPropertiesList();
    setStartupPropertiesValues();

    connect(m_workingSet.m_propertiesManager,
        SIGNAL(valueChanged (QtProperty *, const QVariant &)),
        this, SLOT(onPropertyValueChanged (QtProperty *, const QVariant &)));

    // Connect toolbar buttons signals
    //
    connect(m_workingSet.m_btnCompileData, SIGNAL(clicked()), this,
        SLOT(onCompileData()));

    m_workingSet.m_propertiesEditor->setVisible(true);
}

void CPropertiesSetter::deinit()
{
    // Disconnect all signals/slots.
    disconnect();

    m_workingSet.m_propertiesEditor->clear();
    m_workingSet.m_propertiesManager->clear();
    m_workingSet.m_propertiesEditor->setVisible(false);
}
bool CPropertiesSetter::mousePressEventHandler( QWidget *pWidget,
                                               QMouseEvent *pEvent )
{
    if (pEvent->buttons().testFlag(Qt::RightButton)) return true;;
    m_baseWorkingSet.m_grid.setZero(pEvent->pos());
    pWidget->update();
    return true;
}

bool CPropertiesSetter::mouseMoveEventHandler( QWidget *pWidget,
                                              QMouseEvent *pEvent )
{
    m_workingSet.m_ptZero = pEvent->pos();
    m_baseWorkingSet.m_grid.bindScreenPointToGrid(m_workingSet.m_ptZero);
    pWidget->update();
    return true;
}

bool CPropertiesSetter::paintEventHandler( QWidget *pWidget,
                                          QPaintEvent *pEvent )
{
    QPainter painter(pWidget);
    QPen oldPen(painter.pen());
    QPen zeroPen(Qt::red, 3, Qt::DashLine);
    painter.setPen(zeroPen);

    painter.drawLine(0, m_workingSet.m_ptZero.y(), pWidget->size().width(),
        m_workingSet.m_ptZero.y());
    painter.drawLine(m_workingSet.m_ptZero.x(), 0, m_workingSet.m_ptZero.x(),
        pWidget->size().height());

    painter.setPen(oldPen);
    return true;
}

bool CPropertiesSetter::keyPressEventHandler( QWidget *pWidget,
                                             QKeyEvent *pEvent )
{
    bool bHandled = false;

    // Prepare final data
    //
    if ((pEvent->key() == Qt::Key_F2))
    {
        onCompileData();
        bHandled = true;
    }

    return (!bHandled);
}

void CPropertiesSetter::onPropertyValueChanged( QtProperty *pProperty,
                                               const QVariant &value )
{
    QtVariantProperty *pVarProperty =
        dynamic_cast<QtVariantProperty *>(pProperty);
    if (!pVarProperty)
        return;

    CPropertiesMap::iterator I = m_propertiesMap.find(pVarProperty);
    m_workingSet.m_wpProperties.setWPPropertyValue(I.value(),
        I.key()->value());
}

void CPropertiesSetter::createPropertiesList()
{
    using namespace Data;
    QtVariantProperty *pProperty = 0;

    // 0) Add page format property
    //
    pProperty = m_workingSet.m_propertiesManager->addProperty(
        QtVariantPropertyManager::enumTypeId(),
        CWPPropertiesStringConsts::instance().
        g_WPPropertiesNamesList[CWPProperties::WPProp_PageFormat]);
    QStringList enumNames;
    for(int i = Data::PageFormat_A5H; i < PageFortmatsCount; ++i)
        enumNames <<
        CPageFormatStringConsts::instance().g_WPPageFormatsNamesList[i];
    
    pProperty->setAttribute(QLatin1String("enumNames"), enumNames);
    
    m_workingSet.m_propertiesEditor->addProperty(pProperty);
    m_propertiesMap.insert(pProperty, CWPProperties::WPProp_PageFormat);

    // 1) Add Zones fragmentation properties.
    //
    pProperty = m_workingSet.m_propertiesManager->addProperty(
        QVariant::Int, CWPPropertiesStringConsts::instance().
        g_WPPropertiesNamesList[CWPProperties::WPProp_ZonesFragmentationX]);
    m_workingSet.m_propertiesEditor->addProperty(pProperty);
    m_propertiesMap.insert(pProperty,
        CWPProperties::WPProp_ZonesFragmentationX);

    pProperty = m_workingSet.m_propertiesManager->addProperty(
        QVariant::Int, CWPPropertiesStringConsts::instance().
        g_WPPropertiesNamesList[CWPProperties::WPProp_ZonesFragmentationY]);
    m_workingSet.m_propertiesEditor->addProperty(pProperty);
    m_propertiesMap.insert(pProperty,
        CWPProperties::WPProp_ZonesFragmentationY);

    // 2) Add Calc matherial ductivity property.
    //
    pProperty = m_workingSet.m_propertiesManager->addProperty(
        QVariant::Bool, CWPPropertiesStringConsts::instance().
        g_WPPropertiesNamesList[CWPProperties::WPProp_CalcMatherialDuctivity]);
    m_workingSet.m_propertiesEditor->addProperty(pProperty);
    m_propertiesMap.insert(pProperty,
        CWPProperties::WPProp_CalcMatherialDuctivity);

    // 3-9) Add double properties (all remaining properties).
    //
    for (int i = CWPProperties::WPProp_YoungModulus;
        i < CWPProperties::WPPropsCount; ++i)
    {
        pProperty = m_workingSet.m_propertiesManager->addProperty(
            QVariant::Double, 
            CWPPropertiesStringConsts::instance().g_WPPropertiesNamesList[i]);
        pProperty->setAttribute(QLatin1String("decimals"), 6);
        m_workingSet.m_propertiesEditor->addProperty(pProperty);
        m_propertiesMap.insert(pProperty,
            CWPProperties::CWPPropsIndexes(i));
    }
}

void CPropertiesSetter::setStartupPropertiesValues()
{
    CPropertiesMap::iterator I = m_propertiesMap.begin();
    CPropertiesMap::iterator E = m_propertiesMap.end();
    for (; I != E; ++I)
    {
        QVariant propValue;
        m_workingSet.m_wpProperties.getWPPropertyValue(I.value(), propValue);
        I.key()->setValue(propValue);
    }
}

void CPropertiesSetter::onCompileData()
{
    if (QMessageBox::question(m_pWorkAreaWidget, tr("Are you sure?"),
        tr("Write workpiece properties?"), QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes) == QMessageBox::Yes)
        emit dataReady();
}
