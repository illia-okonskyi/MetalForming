#pragma once
#include "PageFormat.h"
#include <QtCore/QCoreApplication>

namespace Data
{
    class CPageFormatStringConsts
    {
        Q_DECLARE_TR_FUNCTIONS(CPageFormatStringConsts);

    public:
        QString g_WPPageFormatsNamesList [PageFortmatsCount];

        static CPageFormatStringConsts& instance();

    private:
        static CPageFormatStringConsts *g_pInstance;
        CPageFormatStringConsts();
    };
}