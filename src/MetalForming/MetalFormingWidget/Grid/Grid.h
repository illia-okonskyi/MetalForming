#pragma once
#include <QtCore\QPoint>

class CGrid
{
public:
    enum GridBindingType
    {
        gridBindingType_Lower = 0,
        gridBindingType_Upper = 1,
        gridBindingType_Nearest = 2,
    };
    
    // This constructor uses defaults from file GridConsts.h
    CGrid();

    CGrid(int iXStep, int iYStep, const QPoint& ptZero, double fXScale,
        double fYScale);

    int getXStep() const;
    void setXStep(int iXStep);

    int getYStep() const;
    void setYStep(int iYStep);

    const QPoint getZero() const;
    void setZero(const QPoint &ptZero);

    double getXScale() const;
    void setXScale(double fXScale);

    double getYScale() const;
    void setYScale(double fYScale);

    void bindScreenPointToGrid(QPoint &point,
        GridBindingType xBindingType = gridBindingType_Nearest,
        GridBindingType yBindingType = gridBindingType_Nearest) const;

    void transformScreenPointToRealPoint(const QPoint &ptScreen, QPointF &ptReal);
    void transformRealPointToScreenPoint(const QPointF ptReal, QPoint &ptScreen);

private:
    int m_iXStep;
    int m_iYStep;

    double m_fXScale;
    double m_fYScale;
    
    QPoint m_ptZero;
};

// Serialization/Deserealization fucntions
//
QDataStream & operator<< (QDataStream &out,
                          const CGrid &grid);
QDataStream & operator>> (QDataStream &in,
                          CGrid &grid);