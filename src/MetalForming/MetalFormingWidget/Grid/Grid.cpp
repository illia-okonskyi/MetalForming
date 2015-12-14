#include "Grid.h"
#include "GridConsts.h"
#include <cassert>

#include <QtCore/QDataStream>

CGrid::CGrid() :
m_iXStep(GridConsts::Defaults::xStep), m_iYStep(GridConsts::Defaults::yStep),
m_ptZero(GridConsts::Defaults::xZero, GridConsts::Defaults::yZero),
m_fXScale(GridConsts::Defaults::xScale), m_fYScale(GridConsts::Defaults::yScale)
{
    bindScreenPointToGrid(m_ptZero);
}

CGrid::CGrid( int iXStep, int iYStep, const QPoint& ptZero, double fXScale,
             double fYScale ) :
m_iXStep(iXStep), m_iYStep(iYStep), m_ptZero(ptZero), m_fXScale(fXScale),
m_fYScale(fYScale)

{
    bindScreenPointToGrid(m_ptZero);
}

double CGrid::getXScale() const
{
    return m_fXScale;
}

void CGrid::setXScale( double fXScale )
{
    m_fXScale = fXScale;
}

double CGrid::getYScale() const
{
    return m_fYScale;
}

void CGrid::setYScale( double fYScale )
{
    m_fYScale = fYScale;
}

const QPoint CGrid::getZero() const
{
    return m_ptZero;
}

void CGrid::setZero( const QPoint &ptZero )
{
    m_ptZero = ptZero;
    bindScreenPointToGrid(m_ptZero);
}

int CGrid::getXStep() const
{
    return m_iXStep;
}

void CGrid::setXStep( int iXStep )
{
    m_iXStep = iXStep;
}

int CGrid::getYStep() const
{
    return m_iYStep;
}

void CGrid::setYStep( int iYStep )
{
    m_iYStep = iYStep;
}

void CGrid::bindScreenPointToGrid( QPoint &point, GridBindingType xBindingType
                            /*= gridBindingType_Nearest*/,
                            GridBindingType yBindingType
                            /*= gridBindingType_Nearest*/ ) const
{
    int modX = point.rx() % m_iXStep;
    int modY = point.ry() % m_iYStep;
    int dx = 0;
    int dy = 0;

    switch(xBindingType)
    {
    case gridBindingType_Lower:
        dx = -modX;
        break;

    case gridBindingType_Upper:
        dx = m_iXStep - modX;
        break;

    case gridBindingType_Nearest:
        dx = (modX < (m_iXStep / 2))? -modX : m_iXStep - modX;
        break;

    default:
        // wrong binding type!
        assert(false);
        break;
    }

    switch(yBindingType)
    {
    case gridBindingType_Lower:
        dy = -modY;
        break;

    case gridBindingType_Upper:
        dy = m_iYStep - modY;
        break;

    case gridBindingType_Nearest:
        dy = (modY < (m_iYStep / 2)) ? -modY : m_iYStep - modY;
        break;

    default:
        // wrong binding type!
        assert(false);
        break;
    }

    point.rx() += dx;
    point.ry() += dy;
}

void CGrid::transformScreenPointToRealPoint( const QPoint &ptScreen,
                                            QPointF &ptReal )
{
    double fX = ptScreen.x() - m_ptZero.x();
    double fY = m_ptZero.y() - ptScreen.y();
    fX /= m_iXStep;
    fY /= m_iYStep;

    fX *= m_fXScale;
    fY *= m_fYScale;
    
    ptReal.setX(fX);
    ptReal.setY(fY);
}

void CGrid::transformRealPointToScreenPoint( const QPointF ptReal,
                                            QPoint &ptScreen )
{
    double fX = ptReal.x();
    double fY = ptReal.y();
    
    fX /= m_fXScale;
    fY /= m_fYScale;

    fX *= m_iXStep;
    fY *= m_iYStep;

    QPointF tmp(fX, fY);
    ptScreen = tmp.toPoint();
    ptScreen.rx() += m_ptZero.x();
    ptScreen.ry() = m_ptZero.y() - ptScreen.y();
}

//////////////////////////////////////////////////////////////////////////
// Serialization/Deserealization fucntions
//////////////////////////////////////////////////////////////////////////

QDataStream & operator<<( QDataStream &out, const CGrid &grid )
{
    out << grid.getZero() << grid.getXScale() << grid.getYScale() <<
        qint32(grid.getXStep()) << qint32(grid.getYStep());
    return out;
}

QDataStream & operator>>( QDataStream &in, CGrid &grid )
{
    QPoint ptZero;
    double fXScale;
    double fYScale;
    qint32 iXStep;
    qint32 iYStep;

    in >> ptZero >> fXScale >> fYScale >> iXStep >> iYStep;

    grid.setZero(ptZero);
    grid.setXScale(fXScale);
    grid.setYScale(fYScale);
    grid.setXStep(iXStep);
    grid.setYStep(iYStep);

    return in;
}
