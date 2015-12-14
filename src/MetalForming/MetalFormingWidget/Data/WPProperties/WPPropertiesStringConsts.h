#pragma once
#include "WPProperties.h"
#include <QtCore/QCoreApplication>

namespace Data
{
    class CWPPropertiesStringConsts
    {
        Q_DECLARE_TR_FUNCTIONS(CWPPropertiesStringConsts);

    public:
        QString g_WPPropertiesNamesList [CWPProperties::WPPropsCount];

        static CWPPropertiesStringConsts& instance();

    private:
        static CWPPropertiesStringConsts *g_pInstance;
        CWPPropertiesStringConsts();
    };
}