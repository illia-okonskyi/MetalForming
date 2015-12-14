#include "WPPropertiesStringConsts.h"

namespace Data
{
    CWPPropertiesStringConsts *CWPPropertiesStringConsts::g_pInstance = 0;

    CWPPropertiesStringConsts& CWPPropertiesStringConsts::instance()
    {
        if (!g_pInstance)
            g_pInstance = new CWPPropertiesStringConsts();
        return *g_pInstance;
    }

    CWPPropertiesStringConsts::CWPPropertiesStringConsts()
    {
        g_WPPropertiesNamesList[CWPProperties::WPProp_PageFormat] =
            tr("Page format");
        g_WPPropertiesNamesList[CWPProperties::WPProp_ZonesFragmentationX] =
            tr("Zones Fragmentation by X");
        g_WPPropertiesNamesList[CWPProperties::WPProp_ZonesFragmentationY] =
            tr("Zones Fragmentation by Y");
        g_WPPropertiesNamesList[CWPProperties::WPProp_CalcMatherialDuctivity] =
            tr("Calculate Matherial Ductivity");
        g_WPPropertiesNamesList[CWPProperties::WPProp_YoungModulus] =
            tr("YoungModulus");
        g_WPPropertiesNamesList[CWPProperties::WPProp_PuassonCoefficient] =
            tr("PuassonCoefficient");
        g_WPPropertiesNamesList[CWPProperties::WPProp_MetalFlowBoundary] =
            tr("MetalFlowBoundary");
        g_WPPropertiesNamesList[CWPProperties::WPProp_KCoefficient] =
            tr("KCoefficient");
        g_WPPropertiesNamesList[CWPProperties::WPProp_NCoefficient] =
            tr("NCoefficient");
        g_WPPropertiesNamesList[CWPProperties::WPProp_ACoefficient] =
            tr("ACoefficient");
        g_WPPropertiesNamesList[CWPProperties::WPProp_BCoefficient] =
            tr("BCoefficient");
    };
}