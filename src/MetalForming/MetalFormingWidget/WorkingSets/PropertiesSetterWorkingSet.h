#pragma once
#include <QtCore/QPoint>
#include "../Data/Data.h"

class QtVariantPropertyManager;
class QtTreePropertyBrowser;
class QPushButton;

class CPropertiesSetterWorkingSet
{
public:
    // Working data
    //
    QPoint m_ptZero;
    Data::CWPProperties m_wpProperties;

    // GUI objects
    //
    QtVariantPropertyManager *m_propertiesManager;
    QtTreePropertyBrowser *m_propertiesEditor;

    QPushButton *m_btnCompileData;
};