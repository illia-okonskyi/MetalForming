#pragma once
#include "WPInstrument.h"
#include <QtGui/QApplication>

namespace Data
{
    class CWPInstrumentStringConsts
    {
        Q_DECLARE_TR_FUNCTIONS(CWPInstrumentStringConsts);

    public:
        QString
            g_instrumentTypesNamesList [CWPInstrument::WPInstrumentsTypesCount];
        QString g_propertiesNamesList [CWPInstrument::WPInstrPropsCount];

        static CWPInstrumentStringConsts& instance();

    private:
        static CWPInstrumentStringConsts *g_pInstance;
        CWPInstrumentStringConsts();
    };
}