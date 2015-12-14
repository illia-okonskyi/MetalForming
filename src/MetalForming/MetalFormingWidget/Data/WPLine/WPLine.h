#pragma once
#include <QtCore/QLine>
#include <QtCore/QString>
#include <QtCore/QCoreApplication>

namespace Data
{
    class CWPLine
    {
        Q_DECLARE_TR_FUNCTIONS(CWPLine);

    public:
        enum CWPLineType
        {
            WPLine_Null = 0,
            WPLine_Main = 1,
            WPLine_Aux = 2,
            WPLine_GridCompact = 3
        };
        CWPLine(QLineF line = QLineF(), CWPLineType type = WPLine_Null);

        bool isNull() const;
        void setNull();

        CWPLineType getType() const;
        void setType(CWPLineType type);

        QString getTypeString() const;
        static QString getTypeString(CWPLineType type);

        const QLineF &getLine() const;
        void setLine(const QLineF &line);

        QPointF getCenterPoint() const;

        QString getStringLong() const;
        QString getStringShort() const;

        // Returns [0.0f; 360.0f] degreeses in clock-wise direction
        double getYAsisAngle() const;
        double getAngleTo(const CWPLine &line) const;
        double getAngleTo(const QLineF &line) const;

        // Returns true if line intersects in their bounds, otherwise returns
        // false and ptIntersection = {0.0f, 0.0f}.
        bool getIntersectPoint(const CWPLine &line, QPointF &ptIntersection);
        bool getIntersectPoint(const QLineF &line,  QPointF &ptIntersection);

        bool operator ==(const CWPLine &other);
    private:
        QLineF m_line;
        CWPLineType m_type;
    };
}

// Serialization/Deserealization fucntions
//
QDataStream & operator<< (QDataStream &out,
                          const Data::CWPLine &wpLine);
QDataStream & operator>> (QDataStream &in,
                          Data::CWPLine &wpLine);