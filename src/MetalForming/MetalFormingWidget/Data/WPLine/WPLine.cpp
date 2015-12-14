#include "WPLine.h"
#include <QtCore/QDataStream>

namespace Data
{
    CWPLine::CWPLine( QLineF line /*= QLineF()*/,
        CWPLineType type /*= WPLine_Null*/ ):
    m_line(line), m_type(type)
    {

    }

    bool CWPLine::isNull() const
    {
        return (m_type == WPLine_Null) || (m_line.isNull());
    }

    void CWPLine::setNull()
    {
        m_type = WPLine_Null;
    }

    CWPLine::CWPLineType CWPLine::getType() const
    {
        return m_type;
    }

    void CWPLine::setType( CWPLineType type )
    {
        m_type = type;
    }

    QString CWPLine::getTypeString() const
    {
        return getTypeString(m_type);
    }

    QString CWPLine::getTypeString( CWPLineType type )
    {
        if (type == WPLine_Null)
            return QString(tr("Null"));
        QString sType;
        switch (type)
        {
        case WPLine_Main:
            sType = tr("Main");
            break;
        case WPLine_Aux:
            sType = tr("Auxiliary");
            break;
        case WPLine_GridCompact:
            sType = tr("Grid compation");
            break;
        }
        return sType;
    }

    const QLineF & CWPLine::getLine() const
    {
        return m_line;
    }

    void CWPLine::setLine( const QLineF &line )
    {
        m_line = line;
    }

    QPointF CWPLine::getCenterPoint() const
    {
        return m_line.pointAt(0.5f);
    }

    QString CWPLine::getStringShort() const
    {
        if (isNull())
            return QString(tr("Null"));

        QString sLineInfoText(tr("(%1, %2)-(%3, %4); %5"));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().y()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().y()));
        sLineInfoText = sLineInfoText.arg(getTypeString());
        return sLineInfoText;
    }

    QString CWPLine::getStringLong() const
    {
        if (isNull())
            return QString(tr("WPLine: Null"));
        QString sLineInfoText(
            tr("WPLine: (%1 mm, %2 mm)-(%3 mm, %4 mm); %5 type"));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p1().y()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().x()));
        sLineInfoText = sLineInfoText.arg(QString::number(m_line.p2().y()));
        sLineInfoText = sLineInfoText.arg(getTypeString());
        return sLineInfoText;
    }

    double CWPLine::getYAsisAngle() const
    {
        double fAngle = m_line.angle() + 90.0f;
        if (fAngle >= 360.0f) fAngle -= 360.0f;
        return fAngle;
    }

    double CWPLine::getAngleTo( const CWPLine &line ) const
    {
        return getAngleTo(line.m_line);
    }

    double CWPLine::getAngleTo( const QLineF &line ) const
    {
        return m_line.angleTo(line);
    }

    bool CWPLine::getIntersectPoint(const CWPLine &line,
        QPointF &ptIntersection )
    {
        return getIntersectPoint(line.m_line, ptIntersection);
    }

    bool CWPLine::getIntersectPoint( const QLineF &line,
        QPointF &ptIntersection )
    {
        QLineF::IntersectType intRes = m_line.intersect(line, &ptIntersection);
        if (intRes == QLineF::BoundedIntersection) return true;
        ptIntersection.setX(0.0f);
        ptIntersection.setY(0.0f);
        return false;
    }

    bool CWPLine::operator==( const CWPLine &other )
    {
        return ((m_line == other.m_line)&&(m_type == other.m_type));
    }
}

//////////////////////////////////////////////////////////////////////////
// Serialization/Deserealization fucntions
//////////////////////////////////////////////////////////////////////////

QDataStream & operator<<( QDataStream &out, const Data::CWPLine &wpLine )
{
    out << wpLine.getLine() << qint32(wpLine.getType());
    return out;
}

QDataStream & operator>>( QDataStream &in, Data::CWPLine &wpLine )
{
    QLineF line;
    qint32 iType;
    in >> line >> iType;
    wpLine.setLine(line);
    wpLine.setType(Data::CWPLine::CWPLineType(iType));
    return in;
}