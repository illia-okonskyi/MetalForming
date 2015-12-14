#include "Tools.h"
#include <cmath>
namespace Tools
{
    bool isPointsFuzzyEqual( const QPointF &pt1, const QPointF &pt2,
        size_t nPrecision /*= 3*/ )
    {
        double fMultiplier = pow((double)10.0f, (double)nPrecision);
        qint64 val1 = qRound64(pt1.x() * fMultiplier);
        qint64 val2 = qRound64(pt2.x() * fMultiplier);

        if (val1 != val2) return false;

        val1 = qRound64(pt1.y() * fMultiplier);
        val2 = qRound64(pt2.y() * fMultiplier);

        return (val1 == val2);
    }
}