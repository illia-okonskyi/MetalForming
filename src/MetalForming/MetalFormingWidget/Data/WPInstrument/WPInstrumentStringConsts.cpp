#include "WPInstrumentStringConsts.h"

namespace Data
{
    CWPInstrumentStringConsts *CWPInstrumentStringConsts::g_pInstance = 0;

    CWPInstrumentStringConsts & CWPInstrumentStringConsts::instance()
    {
        if (!g_pInstance)
            g_pInstance = new CWPInstrumentStringConsts();
        return *g_pInstance;
    }

    CWPInstrumentStringConsts::CWPInstrumentStringConsts()
    {
        g_instrumentTypesNamesList[CWPInstrument::WPInstrument_Null] =
            tr("Null");
        g_instrumentTypesNamesList[CWPInstrument::WPInstrument_Default] =
            tr("Default");
        g_instrumentTypesNamesList[CWPInstrument::WPInstrument_DisturbedLoad] =
            tr("DisturbedLoad");
        g_instrumentTypesNamesList[CWPInstrument::
            WPInstrument_FrictionConsidering] = tr("FrictionConsidering");
        g_instrumentTypesNamesList[CWPInstrument::WPInstrument_StressStrain] =
            tr("StressStrain");

        g_propertiesNamesList[CWPInstrument::WPInstrProp_Type] =
            tr("Instrument type");
        g_propertiesNamesList[CWPInstrument::WPInstrProp_WPPointsMovement] =
            tr("Workpiece points movement");
        g_propertiesNamesList[CWPInstrument::WPInstrProp_InsrMovementX] =
            tr("Instrument movement X");
        g_propertiesNamesList[CWPInstrument::WPInstrProp_InsrMovementY] =
            tr("Instrument movement Y");
        g_propertiesNamesList[CWPInstrument::WPInstrProp_DisturbedLoad] =
            tr("Disturbed load");
        g_propertiesNamesList[CWPInstrument::WPInstrProp_Friction] =
            tr("Friction");

    }
}