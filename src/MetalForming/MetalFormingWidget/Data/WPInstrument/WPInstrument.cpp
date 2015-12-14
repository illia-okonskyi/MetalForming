#include "WPInstrument.h"
#include "WPInstrumentStringConsts.h"
#include "../OutputFormatter/OutputFormatter.h"

#include <algorithm>
#include <QtCore/QTextstream>

namespace Data
{
    //////////////////////////////////////////////////////////////////////////
    struct CWPInstrumetsStringConsts
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
    CWPInstrument::CWPInstrument( QLineF line /*= QLineF()*/,
        CWPInstrumentType type /*= WPInstrument_Null*/ )
        : m_line(line), m_type(type), m_fWorkpicePointsMovement(0.0f),
        m_fInstrumentMovementX(0.0f), m_fInstrumentMovementY(0.0f),
        m_fDisturbedLoad(0.0f), m_fFriction(0.0f)
    {

    }

    bool CWPInstrument::isNull() const
    {
        return (m_type == WPInstrument_Null) || (m_line.isNull());
    }

    void CWPInstrument::setNull()
    {
        m_type = WPInstrument_Null;
    }

    CWPInstrument::CWPInstrumentType CWPInstrument::getType() const
    {
        return m_type;
    }

    void CWPInstrument::setType( CWPInstrumentType type )
    {
        m_type = type;
    }

    QString CWPInstrument::getTypeString() const
    {
        return getTypeString(m_type);
    }

    QString CWPInstrument::getTypeString( CWPInstrumentType type )
    {
        return CWPInstrumentStringConsts::instance().
            g_instrumentTypesNamesList[type];
    }

    const QLineF & CWPInstrument::getLine() const
    {
        return m_line;
    }

    void CWPInstrument::setLine( const QLineF &line )
    {
        m_line = line;
    }

    QPointF CWPInstrument::getCenterPoint() const
    {
        return m_line.pointAt(0.5f);
    }

    QString CWPInstrument::getStringShort() const
    {
        if (isNull())
            return CWPInstrumentStringConsts::instance().
            g_instrumentTypesNamesList[WPInstrument_Null];

        QString sLineInfoText(tr("(%1, %2)-(%3, %4); %5"));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().y()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().y()));
        sLineInfoText = sLineInfoText.arg(getTypeString());
        return sLineInfoText;
    }

    QString CWPInstrument::getStringLong() const
    {
        if (isNull())
            return QString(tr("WPInstrument: Null"));
        QString sLineInfoText(
            tr("WPInstrument: (%1 mm, %2 mm)-(%3 mm, %4 mm); %5 type"));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().y()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().y()));
        sLineInfoText = sLineInfoText.arg(getTypeString());
        return sLineInfoText;
    }

    double CWPInstrument::getYAsisAngle() const
    {
        double fAngle = m_line.angle() + 90.0f;
        if (fAngle >= 360.0f) fAngle -= 360.0f;
        return fAngle;
    }

    bool CWPInstrument::operator==( const CWPInstrument &other )
    {
        // NOTE: now we compare instruments only by line.
        return (m_line == other.m_line);
    }

    //////////////////////////////////////////////////////////////////////////

    double CWPInstrument::getWorkpicePointsMovement() const
    {
        return m_fWorkpicePointsMovement;
    }

    void CWPInstrument::setWorkpicePointsMovement(
        double fWorkpicePointsMovement )
    {
        m_fWorkpicePointsMovement = fWorkpicePointsMovement;
    }

    double CWPInstrument::getInstrumentMovementX() const
    {
        return m_fInstrumentMovementX;
    }

    void CWPInstrument::setInstrumentMovementX( double fInstrumentMovementX )
    {
        m_fInstrumentMovementX = fInstrumentMovementX;
    }

    double CWPInstrument::getInstrumentMovementY() const
    {
        return m_fInstrumentMovementY;
    }

    void CWPInstrument::setInstrumentMovementY( double fInstrumentMovementY )
    {
        m_fInstrumentMovementY = fInstrumentMovementY;
    }

    double CWPInstrument::getDisturbedLoad() const
    {
        return m_fDisturbedLoad;
    }

    void CWPInstrument::setDisturbedLoad( double fDisturbedLoad )
    {
        m_fDisturbedLoad = fDisturbedLoad;
    }

    double CWPInstrument::getFriction() const
    {
        return m_fFriction;
    }

    void CWPInstrument::setFriction( double fFriction )
    {
        m_fFriction = fFriction;
    }

    bool CWPInstrument::getInstrumentPropertyValue(
        const CWPInstrumentProperties &propType, QVariant &value ) const
    {
        switch (propType)
        {
        case WPInstrProp_Type:
            value = getType() - WPInstrument_Default;
            break;
        case WPInstrProp_WPPointsMovement:
            value = getWorkpicePointsMovement();
            break;
        case WPInstrProp_InsrMovementX:
            value = getInstrumentMovementX();
            break;
        case WPInstrProp_InsrMovementY:
            value = getInstrumentMovementY();
            break;
        case WPInstrProp_DisturbedLoad:
            value = getDisturbedLoad();
            break;
        case WPInstrProp_Friction:
            value = getFriction();
            break;
        default:
            value.clear();
            return false;
        }
        return true;
    }

    bool CWPInstrument::setInstrumentPropertyValue(
        const CWPInstrumentProperties &propType, const QVariant &value )
    {
        switch (propType)
        {
        case WPInstrProp_Type:
            setType(CWPInstrumentType(value.toInt() + WPInstrument_Default));
            break;
        case WPInstrProp_WPPointsMovement:
            setWorkpicePointsMovement(value.toDouble());
            break;
        case WPInstrProp_InsrMovementX:
            setInstrumentMovementX(value.toDouble());
            break;
        case WPInstrProp_InsrMovementY:
            setInstrumentMovementY(value.toDouble());
            break;
        case WPInstrProp_DisturbedLoad:
            setDisturbedLoad(value.toDouble());
            break;
        case WPInstrProp_Friction:
            setFriction(value.toDouble());
            break;
        default:
            return false;
        }
        return true;
    }

    bool CWPInstrument::isPropertyAvaliableForCurrentType(
        const CWPInstrumentProperties &propType ) const
    {
        if (m_type == WPInstrument_Null)
            return false;

        if (propType == WPInstrProp_Type)
            return true;

        switch(propType)
        {
        case WPInstrProp_WPPointsMovement:
            return (m_type == WPInstrument_Default);
            break;

        case WPInstrProp_InsrMovementX:
        case WPInstrProp_InsrMovementY:
            return (m_type != WPInstrument_DisturbedLoad);
            break;

        case WPInstrProp_DisturbedLoad:
            return (m_type == WPInstrument_DisturbedLoad);
            break;

        case WPInstrProp_Friction:
            return (m_type == WPInstrument_FrictionConsidering);
            break;

        default:
            return false;
        }
    }

    void CWPInstrument::swapLinePoints()
    {
        QPointF pt2 = m_line.p1();
        m_line.setP1(m_line.p1());
        m_line.setP2(pt2);
    }

    void CWPInstrument::translatePointsIntoOutputOrder()
    {
        if (isNull())
            return;

        QPointF pt1 = m_line.p1();
        QPointF pt2 = m_line.p2();

        // 1) Line is horisontal.
        //
        if (pt1.y() == pt2.y())
        {
            if (pt2.x() < pt1.x())
                swapLinePoints();
            return ;
        }

        // 2) Line is vertical.
        //
        if (pt1.x() == pt2.x())
        {
            if (pt2.y() < pt1.y())
                swapLinePoints();
            return ;
        }

        // 3) Line is canonical surface.
        //
        QPointF ptZero(0.0f,0.0f);

        // Real and screen coordinates has reverse Y asis direction
        //
        pt1.setY(-pt1.y());
        pt2.setY(-pt2.y());

        double fP1Angle = QLineF(ptZero, pt1).angle() + 360.0f;
        double fP2Angle = QLineF(ptZero, pt2).angle() + 360.0f;
        if (fP2Angle < fP1Angle)
            swapLinePoints();
    }

    bool CWPInstrument::getOutputString( const CDoublesVector &movementsHV,
        const CDoublesVector &movementsC,
        const CDoublesVector &disturbedLoads,
        const CDoublesVector &frictions, QString &sOutput )
    {
        if (isNull())
            return false;

        translatePointsIntoOutputOrder();

        // 0) Fill the indexes numbers:
        //    (Index = std::distance(std::find()) + 1).
        //
        CDoublesVector::difference_type nWPPointsMovementHVIndex =
            std::distance(movementsHV.begin(),
            std::find(movementsHV.begin(),
            movementsHV.end(), m_fWorkpicePointsMovement)) + 1;
        CDoublesVector::difference_type nWPPointsMovementCIndex =
            std::distance(movementsC.begin(),
            std::find(movementsC.begin(),
            movementsC.end(), m_fWorkpicePointsMovement)) + 1;
        CDoublesVector::difference_type nInstrumentMovementsXIndex =
            std::distance(movementsHV.begin(),
            std::find(movementsHV.begin(),
            movementsHV.end(), m_fInstrumentMovementX)) + 1;
        CDoublesVector::difference_type nInstrumentMovementsYIndex =
            std::distance(movementsHV.begin(),
            std::find(movementsHV.begin(),
            movementsHV.end(), m_fInstrumentMovementY)) + 1;
        CDoublesVector::difference_type nDisturbedLoadIndex =
            std::distance(disturbedLoads.begin(),
            std::find(disturbedLoads.begin(),
            disturbedLoads.end(), m_fDisturbedLoad)) + 1;
        CDoublesVector::difference_type nFrictionIndex =
            std::distance(frictions.begin(),
            std::find(frictions.begin(),
            frictions.end(), m_fFriction)) + 1;

        // 1) Check if line is horisontal or vertical
        //
        bool bLineIsHorisontal = (m_line.p1().y() == m_line.p2().y());
        bool bLineIsVertical = (m_line.p1().x() == m_line.p2().x());
        
        QTextStream ss(&sOutput);

        // 2) Write coordinates.
        //
        ss << COutputFormatter::formatValue(m_line.p1().x(), OutputFormat_F6_2);
        ss << COutputFormatter::formatValue(m_line.p1().y(), OutputFormat_F6_2);
        ss << COutputFormatter::formatValue(m_line.p2().x(), OutputFormat_F6_2);
        ss << COutputFormatter::formatValue(m_line.p2().y(), OutputFormat_F6_2);

        // 3) Write indexes.
        // 3.1) Write instrument specific indexes.
        // 
        switch(m_type)
        {
        case WPInstrument_Default:
            if (bLineIsHorisontal)
            {
                ss << COutputFormatter::formatValue(nWPPointsMovementHVIndex,
                OutputFormat_I2);
                ss << COutputFormatter::formatValue(1, OutputFormat_I2);
            }
            else if (bLineIsVertical)
            {
                ss << COutputFormatter::formatValue(nWPPointsMovementHVIndex,
                    OutputFormat_I2);
                ss << COutputFormatter::formatValue(2, OutputFormat_I2);
            }
            else
            {
                ss << COutputFormatter::formatValue(nWPPointsMovementCIndex,
                OutputFormat_I2);
            }
            break;
        case WPInstrument_DisturbedLoad:
            ss << COutputFormatter::formatValue(nDisturbedLoadIndex,
                OutputFormat_I2);
            break;
        case WPInstrument_FrictionConsidering:
            ss << COutputFormatter::formatValue(nFrictionIndex,
                OutputFormat_I2);
            break;
        case WPInstrument_StressStrain:
            // No cpecific indexes.
            break;

        default:
            return false;
            break;
        }
        
        // 3.1) Write instruments common indexes, except DisturbedLoad
        //      instrument type indexes.
        // 
        switch(m_type)
        {
        case WPInstrument_Default:
        case WPInstrument_FrictionConsidering:
        case WPInstrument_StressStrain:
            ss << COutputFormatter::formatValue(nInstrumentMovementsXIndex,
                OutputFormat_I2);
            ss << COutputFormatter::formatValue(nInstrumentMovementsYIndex,
                OutputFormat_I2);
            break;
        }
        ss.flush();
        return true;
    }

    bool CWPInstrument::isCanonical() const
    {
        return (!(isHorizontal() || isVertical()));
    }

    bool CWPInstrument::isHorizontal() const
    {
        return (m_line.p1().y() == m_line.p2().y());
    }

    bool CWPInstrument::isVertical() const
    {
        return (m_line.p1().x() == m_line.p2().x());
    }
}

//////////////////////////////////////////////////////////////////////////
// Serialization/Deserealization fucntions
//////////////////////////////////////////////////////////////////////////

QDataStream & operator<<( QDataStream &out,
                         const Data::CWPInstrument &wpInstrument )
{
    out << wpInstrument.getLine() << qint32(wpInstrument.getType()) <<
        wpInstrument.getWorkpicePointsMovement() <<
        wpInstrument.getInstrumentMovementX() <<
        wpInstrument.getInstrumentMovementY() <<
        wpInstrument.getDisturbedLoad() <<
        wpInstrument.getFriction();
    return out;
}

QDataStream & operator>>( QDataStream &in, Data::CWPInstrument &wpInstrument )
{
    QLineF line;
    qint32 iType;
    double fWorkpicePointsMovement;
    double fInstrumentMovementX;
    double fInstrumentMovementY;
    double fDisturbedLoad;
    double fFriction;

    in >> line >> iType >> fWorkpicePointsMovement >> fInstrumentMovementX >>
        fInstrumentMovementY >> fDisturbedLoad >> fFriction;

    wpInstrument.setLine(line);
    wpInstrument.setType(Data::CWPInstrument::CWPInstrumentType(iType));
    wpInstrument.setWorkpicePointsMovement(fWorkpicePointsMovement);
    wpInstrument.setInstrumentMovementX(fInstrumentMovementX);
    wpInstrument.setInstrumentMovementY(fInstrumentMovementY);
    wpInstrument.setDisturbedLoad(fDisturbedLoad);
    wpInstrument.setFriction(fFriction);
    return in;
}
