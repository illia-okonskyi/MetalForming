#include "PageFormatStringConsts.h"

namespace Data
{
    CPageFormatStringConsts *CPageFormatStringConsts::g_pInstance = 0;

    CPageFormatStringConsts& CPageFormatStringConsts::instance()
    {
        if (!g_pInstance)
            g_pInstance = new CPageFormatStringConsts();
        return *g_pInstance;
    }

    CPageFormatStringConsts::CPageFormatStringConsts()
    {
        g_WPPageFormatsNamesList[PageFormat_A5H] =
            tr("A5H");
        g_WPPageFormatsNamesList[PageFormat_A5V] =
            tr("A5V");
        g_WPPageFormatsNamesList[PageFormat_A4H] =
            tr("A4H");
        g_WPPageFormatsNamesList[PageFormat_A4V] =
            tr("A4V");
        g_WPPageFormatsNamesList[PageFormat_A3H] =
            tr("A3H");
        g_WPPageFormatsNamesList[PageFormat_A3V] =
            tr("A3V");
    };
}