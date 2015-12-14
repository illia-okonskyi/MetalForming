#include "WPProperties.h"

#include <QtCore/QDataStream>
#include <QtGui/QApplication>

namespace Data
{
    CWPProperties::CWPProperties(CPageFormat pageFormat /*= PageFormat_A5H*/,
        int iZonesFragmenationXCount /*= 0*/,
        int iZonesFragmenationYCount /*= 0*/,
        bool bCalcMetherialDuctility /*= false*/,
        double fYoungModulus /*= 0.0f*/,
        double fPuassoneCoefficient /*= 0.0f*/,
        double fMetalFlowBoundary /*= 0.0f*/,
        double fKCoefficient /*= 0.0f*/,
        double fNCoefficient /*= 0.0f*/,
        double fACoefficient /*= 0.0f*/,
        double fBCoefficient /*= 0.0f*/ )
        :m_pageFormat(pageFormat),
        m_iZonesFragmenationXCount(iZonesFragmenationXCount),
        m_iZonesFragmenationYCount(iZonesFragmenationYCount),
        m_bCalcMetherialDuctility(bCalcMetherialDuctility),
        m_fYoungModulus(fYoungModulus),
        m_fPuassoneCoefficient(fPuassoneCoefficient),
        m_fMetalFlowBoundary(fMetalFlowBoundary),
        m_fKCoefficient(fKCoefficient),
        m_fNCoefficient(fNCoefficient),
        m_fACoefficient(fACoefficient),
        m_fBCoefficient(fBCoefficient)
    {

    }

    bool CWPProperties::getWPPropertyValue( const CWPPropsIndexes &propIndex,
        QVariant &value ) const
    {
        switch (propIndex)
        {
        case WPProp_PageFormat:
            value = m_pageFormat;
            break;
        case WPProp_ZonesFragmentationX:
            value = m_iZonesFragmenationXCount;
            break;
        case WPProp_ZonesFragmentationY:
            value = m_iZonesFragmenationYCount;
            break;
        case WPProp_CalcMatherialDuctivity:
            value = m_bCalcMetherialDuctility;
            break;
        case WPProp_YoungModulus:
            value = m_fYoungModulus;
            break;
        case WPProp_PuassonCoefficient:
            value = m_fPuassoneCoefficient;
            break;
        case WPProp_MetalFlowBoundary:
            value = m_fMetalFlowBoundary;
            break;
        case WPProp_KCoefficient:
            value = m_fKCoefficient;
            break;
        case WPProp_NCoefficient:
            value = m_fNCoefficient;
            break;
        case WPProp_ACoefficient:
            value = m_fACoefficient;
            break;
        case WPProp_BCoefficient:
            value = m_fBCoefficient;
            break;
        default:
            return false;
        }
        return true;
    }

    bool CWPProperties::setWPPropertyValue( const CWPPropsIndexes &propIndex,
        const QVariant &value )
    {
        switch (propIndex)
        {
        case WPProp_PageFormat:
            m_pageFormat = CPageFormat(value.toInt());
            break;
        case WPProp_ZonesFragmentationX:
            m_iZonesFragmenationXCount = value.toUInt();
            break;
        case WPProp_ZonesFragmentationY:
            m_iZonesFragmenationYCount = value.toUInt();
            break;
        case WPProp_CalcMatherialDuctivity:
            m_bCalcMetherialDuctility = value.toBool();
            break;
        case WPProp_YoungModulus:
            m_fYoungModulus = value.toDouble();
            break;
        case WPProp_PuassonCoefficient:
            m_fPuassoneCoefficient = value.toDouble();
            break;
        case WPProp_MetalFlowBoundary:
            m_fMetalFlowBoundary = value.toDouble();
            break;
        case WPProp_KCoefficient:
            m_fKCoefficient = value.toDouble();
            break;
        case WPProp_NCoefficient:
            m_fNCoefficient = value.toDouble();
            break;
        case WPProp_ACoefficient:
            m_fACoefficient = value.toDouble();
            break;
        case WPProp_BCoefficient:
            m_fBCoefficient = value.toDouble();
            break;
        default:
            return false;
        }
        return true;
    }

    void CWPProperties::clearAllProperties()
    {
        m_pageFormat = PageFormat_A5H;
        m_iZonesFragmenationXCount = 0;
        m_iZonesFragmenationYCount = 0;
        m_bCalcMetherialDuctility = false;
        m_fYoungModulus = 0.0f;
        m_fPuassoneCoefficient = 0.0f;
        m_fMetalFlowBoundary = 0.0f;
        m_fKCoefficient = 0.0f;
        m_fNCoefficient = 0.0f;
        m_fACoefficient = 0.0f;
        m_fBCoefficient = 0.0f;
    }
}

//////////////////////////////////////////////////////////////////////////
// Serialization/Deserealization fucntions
//////////////////////////////////////////////////////////////////////////

QDataStream & operator<<( QDataStream &out, const Data::CWPProperties &wpProps )
{
    int iPageFormat;
    int iZonesFragmenationXCount;
    int iZonesFragmenationYCount;

    bool bCalcMetherialDuctility;
    double fYoungModulus;
    double fPuassoneCoefficient;
    double fMetalFlowBoundary;
    double fKCoefficient;
    double fNCoefficient;
    double fACoefficient;
    double fBCoefficient;

    QVariant val;

    // 1) Get all values.
    //
    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_PageFormat,
        val);
    iPageFormat = val.toInt();
    
    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_ZonesFragmentationX,
        val);
    iZonesFragmenationXCount = val.toInt();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_ZonesFragmentationY,
        val);
    iZonesFragmenationYCount = val.toInt();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_CalcMatherialDuctivity,
        val);
    bCalcMetherialDuctility = val.toBool();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_YoungModulus,
        val);
    fYoungModulus = val.toDouble();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_PuassonCoefficient,
        val);
    fPuassoneCoefficient = val.toDouble();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_MetalFlowBoundary,
        val);
    fMetalFlowBoundary = val.toDouble();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_KCoefficient,
        val);
    fKCoefficient = val.toDouble();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_NCoefficient,
        val);
    fNCoefficient = val.toDouble();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_ACoefficient,
        val);
    fACoefficient = val.toDouble();

    wpProps.getWPPropertyValue(Data::CWPProperties::WPProp_BCoefficient,
        val);
    fBCoefficient = val.toDouble();

    // 2) Write all values into stream.
    //
    out << iPageFormat << iZonesFragmenationXCount <<
        iZonesFragmenationYCount << bCalcMetherialDuctility << fYoungModulus <<
        fPuassoneCoefficient << fMetalFlowBoundary << fKCoefficient <<
        fNCoefficient << fACoefficient << fBCoefficient;

    return out;
}

QDataStream & operator>>( QDataStream &in, Data::CWPProperties &wpProps )
{
    int iPageFormat;
    int iZonesFragmenationXCount;
    int iZonesFragmenationYCount;

    bool bCalcMetherialDuctility;
    double fYoungModulus;
    double fPuassoneCoefficient;
    double fMetalFlowBoundary;
    double fKCoefficient;
    double fNCoefficient;
    double fACoefficient;
    double fBCoefficient;

    // 1) Read all values from stream.
    //
    in >> iPageFormat >> iZonesFragmenationXCount >> iZonesFragmenationYCount >>
        bCalcMetherialDuctility >> fYoungModulus >> fPuassoneCoefficient >>
        fMetalFlowBoundary >> fKCoefficient >> fNCoefficient >>
        fACoefficient >> fBCoefficient;

    // 2) Set all values in object
    //
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_PageFormat,
        QVariant(iPageFormat));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_ZonesFragmentationX,
        QVariant(iZonesFragmenationXCount));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_ZonesFragmentationY,
        QVariant(iZonesFragmenationYCount));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_CalcMatherialDuctivity,
        QVariant(bCalcMetherialDuctility));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_YoungModulus,
        QVariant(fYoungModulus));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_PuassonCoefficient,
        QVariant(fPuassoneCoefficient));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_MetalFlowBoundary,
        QVariant(fMetalFlowBoundary));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_KCoefficient,
        QVariant(fKCoefficient));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_NCoefficient,
        QVariant(fNCoefficient));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_ACoefficient,
        QVariant(fACoefficient));
    wpProps.setWPPropertyValue(Data::CWPProperties::WPProp_BCoefficient,
        QVariant(fBCoefficient));

    return in;
}
