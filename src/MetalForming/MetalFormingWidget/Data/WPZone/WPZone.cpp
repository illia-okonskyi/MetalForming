#include "WPZone.h"
#include "../OutputFormatter/OutputFormatter.h"

#include <algorithm>
#include <functional>
#include <cassert>

#include <QtCore/QLineF>
#include <QtGui/QPolygonF>
#include <QtCore/QTextStream>
#include <QtCore/QDataStream>

namespace Data
{
    struct CWalkOrderSortPredicate :
        public std::binary_function<QPointF, QPointF, bool>
    {
        CWalkOrderSortPredicate(const QPointF ptZero)
            :m_ptZero(ptZero)
        {
            // Real and screen coordinates has reverse Y asis direction
            m_ptZero.setY(-m_ptZero.y());
        }

        bool operator () (const QPointF &ptLeft, const QPointF &ptRight)
        {
            // Real and screen coordinates has reverse Y asis direction
            QPointF ptLeftScreen(ptLeft);
            QPointF ptRightScreen(ptRight);
            ptLeftScreen.setY(-ptLeftScreen.y());
            ptRightScreen.setY(-ptRightScreen.y());

            double fLeftAngle = QLineF(m_ptZero, ptLeftScreen).angle() +
                360.0f;
            double fRightAngle = QLineF(m_ptZero, ptRightScreen).angle() +
                360.0f;
            return fLeftAngle < fRightAngle;
        }
    private:
        QPointF m_ptZero;
    };

    bool isTriangle(const QPointF &ptA, const QPointF &ptB, const QPointF &ptC)
    {
        size_t nABLen = qRound(QLineF(ptA, ptB).length() * 1000.0f);
        size_t nACLen = qRound(QLineF(ptA, ptC).length() * 1000.0f);
        size_t nBCLen = qRound(QLineF(ptB, ptC).length() * 1000.0f);

        return ((nABLen < (nACLen + nBCLen)) && (nACLen < (nABLen + nBCLen)) &&
            (nBCLen < (nACLen + nABLen)));
    }

    bool isTriangle(const CPointsVector &points)
    {
        if ((points.empty()) || (points.size() > 3))  return false;
        return isTriangle(points[0], points[1], points[2]);
    }

    //////////////////////////////////////////////////////////////////////////

    CWPZone::CGridCompactionPoint::CGridCompactionPoint()
        : m_bAvaliable(false), m_pt(QPointF())

    {

    }

    CWPZone::CGridCompactionPoint::CGridCompactionPoint( const QPointF &pt )
        : m_bAvaliable(true), m_pt(pt)
    {

    }

    //////////////////////////////////////////////////////////////////////////

    CWPZone::CZoneLinesIntersection::CZoneLinesIntersection()
        : m_pt(QPointF()), m_nLineIndex(-1)
    {

    }

    CWPZone::CZoneLinesIntersection::CZoneLinesIntersection( const QPointF &pt,
        int nLineIndex ) : m_pt(pt), m_nLineIndex(nLineIndex)
    {

    }

    //////////////////////////////////////////////////////////////////////////
    CWPZone::CWPZone( const CPointsVector &points /*= CPointsVector()*/,
        CWPZoneType type /*= CWPZone::WPLine_Null*/ )
        : m_points(points), m_type(type), m_bInWalkOrder(false)
    {
        if (!m_points.empty())
            translatePointsToWalkOrder();

        // grid compaction points must be avaliable in every case
        m_gridCompactionPoints.insert(m_gridCompactionPoints.begin(),
            4, CGridCompactionPoint());
    }

    bool CWPZone::isNull() const
    {
        return ((m_points.empty()) || (m_type == WPZone_Null));
    }

    void CWPZone::setNull()
    {
        m_points.clear();
        m_type = WPZone_Null;
    }

    CWPZone::CWPZoneType CWPZone::getType() const
    {
        return m_type;
    }

    void CWPZone::setType( CWPZone::CWPZoneType type )
    {
        m_type = type;
    }

    QString CWPZone::getTypeString() const
    {
        return getTypeString(m_type);
    }

    QString CWPZone::getTypeString( CWPZoneType type )
    {
        if (type == WPZone_Null)
            return QString(tr("Null"));
        return QString((type == WPZone_One)? tr("One") :
            tr("Zero"));
    }

    int CWPZone::getZoneMapTypeInt() const
    {
        return getZoneMapTypeInt(m_type);
    }

    int CWPZone::getZoneMapTypeInt( CWPZoneType type )
    {
        return (type == WPZone_One)? 1 : 0;
    }

    Data::CPointsVector CWPZone::getPoints() const
    {
        return m_points;
    }

    void CWPZone::setPoints( const CPointsVector &points )
    {
        m_points = points;
    }

    QPointF CWPZone::getCenterPoint() const
    {
        assert(m_bInWalkOrder);

        // Center point is the intersection of the diagonals
        QLineF d1(m_points[0], m_points[2]);
        QLineF d2(m_points[1], m_points[3]);
        QPointF ptCenter;
        d1.intersect(d2, &ptCenter);
        return ptCenter;
    }

    QString CWPZone::getStringLong() const
    {
        if (isNull())
            return QString(tr("WPZone: Null"));

        QPointF ptCentral = getCenterPoint();
        QString sLineInfoText(
            tr("WPZone: Central point at (%1 mm, %2 mm); %3 type"));
        sLineInfoText = sLineInfoText.arg(QString::number(ptCentral.x()));
        sLineInfoText = sLineInfoText.arg(QString::number(ptCentral.y()));
        sLineInfoText = sLineInfoText.arg(getTypeString());
        return sLineInfoText;
    }

    QString CWPZone::getStringShort() const
    {
        if (isNull())
            return QString(tr("Null"));

        QPointF ptCentral = getCenterPoint();
        QString sLineInfoText(tr("CP at (%1 mm, %2 mm); %3"));
        sLineInfoText = sLineInfoText.arg(QString::number(ptCentral.x()));
        sLineInfoText = sLineInfoText.arg(QString::number(ptCentral.y()));
        sLineInfoText = sLineInfoText.arg(getTypeString());
        return sLineInfoText;
    }

    bool CWPZone::contains( const QPointF &point )
    {
        QPolygonF poly(m_points);
        return poly.containsPoint(point, Qt::OddEvenFill);
    }

    bool CWPZone::operator==( const CWPZone &other )
    {
        return ((m_points == other.m_points) &&(m_type == other.m_type));
    }

    bool CWPZone::isWPZone( const CPointsVector &points )
    {
        if ((points.empty()) || (points.size() > 4)) return false;

        for (size_t i = 0; i < 4; ++i)
        {
            CPointsVector triangle;
            for (size_t j = 0; j < 4; ++j)
                if (i != j) triangle.push_back(points[j]);

            if (!isTriangle(triangle)) return false;
        }
        return true;
    }

    bool CWPZone::translatePointsToWalkOrder()
    {
        if (!isWPZone(m_points)) return false;

        // searching bottom-left point with REAL coordinate
        QPointF ptZero = m_points[0];
        foreach(QPointF pt, m_points)
        {
            if ((pt.y() < ptZero.y()) ||
                ((pt.y() == ptZero.y()) && (pt.x() < ptZero.x()))) ptZero = pt;
        }

        CWalkOrderSortPredicate pred(ptZero);
        m_points.erase(std::find(m_points.begin(), m_points.end(),
            ptZero));
        std::sort(m_points.begin(), m_points.end(), pred);
        m_points.push_front(ptZero);

        m_bInWalkOrder = true;
        return true;
    }

    bool CWPZone::processGridCompactionLine( const QLineF &line )
    {
        if (isNull()) return false;
        if (!m_bInWalkOrder) return false;

        CZoneLinesIntersections intersections;
        CPointsVector::iterator itPt1 = m_points.begin();
        CPointsVector::iterator itLast = m_points.end() - 1;

        // Check last workpiece line
        //
        QPointF ptIntersection;
        QLineF wpLine(*itPt1, *itLast);
        if (wpLine.intersect(line, &ptIntersection) == 
            QLineF::BoundedIntersection)
            intersections.push_back(CZoneLinesIntersection(ptIntersection, 3));

        // Check all lines except the last one
        //
        for (size_t nLineIndex = 0; itPt1 != itLast; ++itPt1, ++nLineIndex)
        {
            CPointsVector::iterator itPt2 = itPt1 + 1;
            wpLine.setP1(*itPt1);
            wpLine.setP2(*itPt2);

            if (wpLine.intersect(line, &ptIntersection) == 
                QLineF::BoundedIntersection)
                intersections.push_back(
                CZoneLinesIntersection(ptIntersection, nLineIndex));
        }

        // If count of intersections not equal 2 - the line isn't compacting 
        // workpiece grid.
        if (intersections.size() != 2) return true;

        // If intersection points lie on near lines - the line isn't compacting 
        // workpiece grid.
        //
        if (abs(intersections[0].m_nLineIndex - intersections[1].m_nLineIndex)
            != 2) return true;

        foreach(CZoneLinesIntersection intersection, intersections)
        {
            m_gridCompactionPoints[intersection.m_nLineIndex].m_bAvaliable = true;
            m_gridCompactionPoints[intersection.m_nLineIndex].m_pt =
                intersection.m_pt;
        }

        return true;
    }

    CPointsVector CWPZone::getGridCompactionPoints() const
    {
        CPointsVector result;

        foreach(CGridCompactionPoint pt, m_gridCompactionPoints)
        {
            if (pt.m_bAvaliable)
                result.push_back(pt.m_pt);
        } 
        return result;
    }

    bool CWPZone::getOutputString( const CPointsVector &allWorkpiecePoints,
        QString &sOutput )
    {
        if (isNull()) return false;
        if (!m_bInWalkOrder) return false;

        // assume that all ok.

        CPointsVector::const_iterator itBegin = allWorkpiecePoints.begin();
        CPointsVector::const_iterator itEnd = allWorkpiecePoints.end();
        QTextStream ss(&sOutput);
        for (size_t i = 0; i < 4; ++i)
        {
            // In output points' indexes start from 1 not from 0.
            //
            CPointsVector::difference_type nPointIndex = std::distance(itBegin,
                std::find(itBegin, itEnd, m_points[i])) + 1;

            CPointsVector::difference_type nGridCompactionPointIndex = 0;
            if (m_gridCompactionPoints[i].m_bAvaliable)
            {
                // In output points' indexes start from 1 not from 0.
                //
                nGridCompactionPointIndex = std::distance(itBegin,
                    std::find(itBegin, itEnd,
                    m_gridCompactionPoints[i].m_pt)) + 1;
            }

            ss << COutputFormatter::formatValue(nPointIndex, OutputFormat_I5);
            ss << COutputFormatter::formatValue(nGridCompactionPointIndex,
                OutputFormat_I5);
        }
        ss.flush();
        return true;
    }

    // Additional accessors
    //
    bool CWPZone::getInnerInWalkOrder() const
    {
        return m_bInWalkOrder;
    }

    void CWPZone::setInnerInWalkOrder( bool bInWalkOrder )
    {
        m_bInWalkOrder = bInWalkOrder;
    }

    CWPZone::CGridCompactionPoints CWPZone::getInnerGridCompactionPoints() const
    {
        return m_gridCompactionPoints;
    }

    void CWPZone::setInnerGridCompactionPoints(
        const CWPZone::CGridCompactionPoints &gridCompactionPoints )
    {
        m_gridCompactionPoints = gridCompactionPoints;
    }
}

//////////////////////////////////////////////////////////////////////////
// Serialization/Deserealization fucntions
//////////////////////////////////////////////////////////////////////////

QDataStream & operator<<( QDataStream &out,
                         const Data::CWPZone::CGridCompactionPoint &gcp )
{
    out << gcp.m_pt << gcp.m_bAvaliable;
    return out;
}

QDataStream & operator>>( QDataStream &in,
                         Data::CWPZone::CGridCompactionPoint &gcp )
{
    in >> gcp.m_pt >> gcp.m_bAvaliable;
    return in;
}

QDataStream & operator<<( QDataStream &out, const Data::CWPZone &wpZone )
{
    out << wpZone.getPoints() << qint32(wpZone.getType()) <<
        wpZone.getInnerInWalkOrder() << wpZone.getInnerGridCompactionPoints();
    return out;
}

QDataStream & operator>>( QDataStream &in, Data::CWPZone &wpZone )
{
    Data::CPointsVector points;
    qint32 iType;
    bool bInWalkOrder;
    Data::CWPZone::CGridCompactionPoints gridCompactionPoints;

    in >> points >> iType >> bInWalkOrder >> gridCompactionPoints;

    wpZone.setPoints(points);
    wpZone.setType(Data::CWPZone::CWPZoneType(iType));
    wpZone.setInnerInWalkOrder(bInWalkOrder);
    wpZone.setInnerGridCompactionPoints(gridCompactionPoints);

    return in;
}

