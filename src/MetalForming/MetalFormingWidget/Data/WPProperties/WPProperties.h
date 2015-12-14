#pragma once
#include <QLine>
#include <QString>
#include <QVariant>
#include "../CommonDefs.h"
#include "../PageFormat/PageFormat.h"

namespace Data
{
    class CWPProperties
    {
    public:
        enum CWPPropsIndexes
        {
            WPProp_PageFormat,
            WPProp_ZonesFragmentationX,
            WPProp_ZonesFragmentationY,
            WPProp_CalcMatherialDuctivity,
            WPProp_YoungModulus,
            WPProp_PuassonCoefficient,
            WPProp_MetalFlowBoundary,
            WPProp_KCoefficient,
            WPProp_NCoefficient,
            WPProp_ACoefficient,
            WPProp_BCoefficient,
            WPPropsCount
        };

        CPageFormat m_pageFormat;

        int m_iZonesFragmenationXCount;
        int m_iZonesFragmenationYCount;

        bool m_bCalcMetherialDuctility;
        double m_fYoungModulus;
        double m_fPuassoneCoefficient;
        double m_fMetalFlowBoundary;
        double m_fKCoefficient;
        double m_fNCoefficient;
        double m_fACoefficient;
        double m_fBCoefficient;

        CWPProperties(CPageFormat m_pageFormat = PageFormat_A5H,
            int iZonesFragmenationXCount = 0,
            int iZonesFragmenationYCount = 0,
            bool bCalcMetherialDuctility = false,
            double fYoungModulus = 0.0f,
            double fPuassoneCoefficient = 0.0f,
            double fMetalFlowBoundary = 0.0f,
            double fKCoefficient = 0.0f,
            double fNCoefficient = 0.0f,
            double fACoefficient = 0.0f,
            double fBCoefficient = 0.0f);

        // returns true if successfully
        bool getWPPropertyValue(const CWPPropsIndexes &propIndex,
            QVariant &value) const;
        // returns true if successfully
        bool setWPPropertyValue(const CWPPropsIndexes &propIndex,
            const QVariant &value);

        void clearAllProperties();
    };
}

// Serialization/Deserealization fucntions
//
QDataStream & operator<< (QDataStream &out,
                          const Data::CWPProperties &wpProps);
QDataStream & operator>> (QDataStream &in,
                          Data::CWPProperties &wpProps);