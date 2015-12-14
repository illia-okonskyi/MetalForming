#pragma once
#include <QPointF>
namespace Tools
{
    bool isPointsFuzzyEqual(const QPointF &pt1, const QPointF &pt2,
        size_t nPrecision = 3);
}