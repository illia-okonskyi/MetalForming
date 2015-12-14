#pragma once
#include "../CommonDefs.h"
#include <QtCore/QString>
#include <QtCore/QLineF>
#include <QtCore/QCoreApplication>

namespace Data
{
    class CWPZone
    {
        Q_DECLARE_TR_FUNCTIONS(CWPZone);

    public:
        enum CWPZoneType
        {
            WPZone_Null = 0,
            WPZone_Zero = 1,
            WPZone_One = 2
        };

        struct CGridCompactionPoint
        {
            CGridCompactionPoint();
            explicit CGridCompactionPoint(const QPointF &pt);
            bool m_bAvaliable;
            QPointF m_pt;
        };
        typedef QVector<CGridCompactionPoint> CGridCompactionPoints;

        CWPZone(const CPointsVector &points = CPointsVector(),
            CWPZoneType type = CWPZone::WPZone_Null);

        bool isNull() const;
        void setNull();

        CWPZone::CWPZoneType getType() const;
        void setType(CWPZone::CWPZoneType type);

        QString getTypeString() const;
        static QString getTypeString(CWPZoneType type);

        int getZoneMapTypeInt() const;
        static int getZoneMapTypeInt(CWPZoneType type);

        CPointsVector getPoints() const;
        void setPoints(const CPointsVector &points);

        QPointF getCenterPoint() const;

        QString getStringLong() const;
        QString getStringShort() const;

        static bool isWPZone(const CPointsVector &points);
        bool translatePointsToWalkOrder();

        // returns true if successfully
        bool processGridCompactionLine(const QLineF &line);

        CPointsVector getGridCompactionPoints() const;

        // Output for the WPZone is ZONE_INDEX
        // ZONE_INDEX is the string, for more details see the documentation.
        // allWorkpiecePoints must be all points of the One-zones(including
        // grid compaction points) 
        bool getOutputString(const CPointsVector &allWorkpiecePoints,
            QString &sOutput);

        bool contains(const QPointF &point);
        bool operator ==(const CWPZone &other);

        // NOTE: This accessors are added for serialization/deserialization
        //       functions (declarind ser/deser fucntions as friend don't
        //       works, I don't know why). Do not use them in your code.
        bool getInnerInWalkOrder() const;
        void setInnerInWalkOrder(bool bInWalkOrder);

        // NOTE: This accessors are added for serialization/deserialization
        //       functions (declarind ser/deser fucntions as friend don't
        //       works, I don't know why). Do not use them in your code.
        CGridCompactionPoints getInnerGridCompactionPoints() const;
        void setInnerGridCompactionPoints(
            const CGridCompactionPoints &gridCompactionPoints);

    private:
        struct CZoneLinesIntersection
        {
            CZoneLinesIntersection();
            CZoneLinesIntersection(const QPointF &pt,
                int nLineIndex);

            int m_nLineIndex;
            QPointF m_pt;
        };
        typedef QVector<CZoneLinesIntersection> CZoneLinesIntersections;

        CPointsVector m_points;
        CWPZoneType m_type;
        bool m_bInWalkOrder;
        CGridCompactionPoints m_gridCompactionPoints;
    };
}

// Serialization/Deserealization fucntions
//
QDataStream & operator<< (QDataStream &out,
                          const Data::CWPZone::CGridCompactionPoint &gcp);
QDataStream & operator>> (QDataStream &in,
                          Data::CWPZone::CGridCompactionPoint &gcp);

QDataStream & operator<< (QDataStream &out,
                         const Data::CWPZone &wpZone);
QDataStream & operator>> (QDataStream &in,
                         Data::CWPZone &wpZone);