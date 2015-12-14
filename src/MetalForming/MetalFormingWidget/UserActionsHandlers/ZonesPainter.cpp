#include "ZonesPainter.h"
#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QKeyEvent>

#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <Windows.h>
#include <algorithm>
#include <functional>
#include <cassert>

#include "../OutputFormatter/OutputFormatter.h"

double g_fWPPointMarkerWidth = 10.0f;
double g_fWPPointMarkerHeight = 10.0f;

struct CZonesComparator:
    public std::binary_function<Data::CWPZone, Data::CWPZone, bool>
{
    bool operator()(const Data::CWPZone &znLeft, const Data::CWPZone &znRight)
    {
        QPointF ptCPLeft(znLeft.getCenterPoint());
        QPointF ptCPRight(znRight.getCenterPoint());

        int iLeftY = qRound(ptCPLeft.y() * 1000.0f);
        int iRightY = qRound(ptCPRight.y() * 1000.0f);

        if (iLeftY > iRightY) return true;

        int iLeftX = qRound(ptCPLeft.x() * 1000.0f);
        int iRightX = qRound(ptCPRight.x() * 1000.0f);

        return ((iLeftY == iRightY) && (iLeftX < iRightX));
    }
};

//////////////////////////////////////////////////////////////////////////

struct CPointsFLessComparator:
    public std::binary_function<QPointF, QPointF, bool>
{
    bool operator()(const QPointF &ptLeft, const QPointF &ptRight)
    {
        if (ptLeft.x() == ptRight.x())
            return (ptLeft.y() < ptRight.y());

        return (ptLeft.x() < ptRight.x());
    }
};

//////////////////////////////////////////////////////////////////////////

struct COneZonesIdentifier:
    public std::unary_function<Data::CWPZone, bool>
{
    bool operator()(const Data::CWPZone &zone)
    {
        return (zone.getType() != Data::CWPZone::WPZone_One);
    }
};

//////////////////////////////////////////////////////////////////////////

CZonesPainter::CZonesPainter(CBaseWorkingSet &baseWorkingSet,
                             QWidget *pWorkAreaWidget,
                             CZonesPainterWorkingSet &workingSet):
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget),
m_workingSet(workingSet)
{

}

void CZonesPainter::init()
{
    m_selectedPoints.clear();
    m_currentZoneType = Data::CWPZone::WPZone_One;

    m_workingSet.m_gbInfo->setVisible(true);
    m_workingSet.m_labelZoneType->setText(Data::CWPZone::getTypeString(
        m_currentZoneType));
    m_workingSet.m_gbZoneMapInfo->setVisible(true);
    updateSelLineInfo();

    // Connect toolbar buttons signals
    //
    connect(m_workingSet.m_btnCompileData, SIGNAL(clicked()), this,
        SLOT(onCompileData()));
    connect(m_workingSet.m_btnClearWorkspace, SIGNAL(clicked()), this,
        SLOT(onClearWorkspace()));
    connect(m_workingSet.m_btnChangeType, SIGNAL(clicked()), this,
        SLOT(onChangeType()));
    connect(m_workingSet.m_btnRestoreLastDeleted, SIGNAL(clicked()), this,
        SLOT(onRestoreLastDeleted()));
    connect(m_workingSet.m_btnDeleteSelected, SIGNAL(clicked()), this,
        SLOT(onDeleteSelected()));
    connect(m_workingSet.m_btnDeleteLastCreated, SIGNAL(clicked()), this,
        SLOT(onDeleteLastCreated()));
    connect(m_workingSet.m_btnClearDeletionHistory, SIGNAL(clicked()), this,
        SLOT(onClearDeletionHistory()));
}

void CZonesPainter::deinit()
{
    disconnect();

    m_workingSet.m_gbInfo->setVisible(false);
    m_workingSet.m_gbZoneMapInfo->setVisible(false);
}

bool CZonesPainter::compile()
{
    return genereateOutputString(false);
}

bool CZonesPainter::mousePressEventHandler(QWidget *pWidget, QMouseEvent *pEvent )
{
    return true;
}

bool CZonesPainter::mouseReleaseEventHandler(QWidget *pWidget, QMouseEvent *pEvent )
{
    return true;
}

bool CZonesPainter::mouseMoveEventHandler(QWidget *pWidget, QMouseEvent * pEvent )
{
    return true;
}

bool CZonesPainter::paintEventHandler( QWidget *pWidget, QPaintEvent *pEvent )
{
    QPainter painter(pWidget);
    drawFinalLines(painter);
    drawFinalZones(painter);
    drawSelectedZone(painter);
    drawZonesPoints(painter);
    drawSelectedPoints(painter);

    // Grid compaction lines must be at foreground.
    drawGridCompactionLines(painter);
    return true;
}

bool CZonesPainter::mouseClickEventHandler( QWidget *pWidget, QMouseEvent *pEvent )
{
    // First find the selection as point and if not founded search selection
    // as zone.

    bool bPointFounded = false;
    foreach(QPointF pt, m_workingSet.m_workpiecePoints)
    {
        QRectF rcPointMarker;
        getWPPointMarker(pt, rcPointMarker);
        if (rcPointMarker.contains(QPointF(pEvent->pos())))
        {
            bPointFounded = true;
            processSelection(pt);
        }
    }

    if (!bPointFounded)
    {
        QPointF ptReal;
        m_baseWorkingSet.m_grid.transformScreenPointToRealPoint(pEvent->pos(),
            ptReal);

        bool bZoneFounded = false;
        foreach(Data::CWPZone zone, m_workingSet.m_workpieceZones)
        {
            if (zone.contains(ptReal))
            {
                m_znSelectedZone = zone;
                bZoneFounded = true;
                break;
            }
        }

        if (!bZoneFounded) cleanSelection();
        updateSelLineInfo();
    }

    pWidget->update();

    return true;
}

bool CZonesPainter::keyPressEventHandler( QWidget *pWidget, QKeyEvent * pEvent )
{
    bool bHandled = true;

    switch(pEvent->key())
    {
    case Qt::Key_Escape:
        // Clean workspace
        onClearWorkspace();
        break;

    case Qt::Key_Delete:
        // Delete selected zone
        onDeleteSelected();
        break;

    case Qt::Key_F2:
        // Prepare final data
        onCompileData();
        break;

    case Qt::Key_Shift:
        // Set the zone type
        onChangeType();
        break;

    default:
        bHandled = false;
    }

    if (bHandled)
        return true;

    if (pEvent->modifiers() == Qt::ControlModifier)
    {
        bHandled = true;
        switch(pEvent->key())
        {
        case Qt::Key_D:
            // Delete previos zone
            onDeleteLastCreated();
            break;

        case Qt::Key_R:
            // Restore last deleted zone
            onRestoreLastDeleted();
            break;

        case Qt::Key_X:
            // Clean deletion history
            onClearDeletionHistory();
            break;

        case Qt::Key_B:
            // Build ZoneMap
            onBuildZoneMap();
            break;

        default:
            bHandled = false;
        }
    }

    return (!bHandled);
}

void CZonesPainter::drawFinalLines( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen mainLinesPen(Qt::blue, 2);
    QBrush mainLinesBrush(Qt::blue);
    QPen auxLinesPen(Qt::darkGreen, 2);
    QBrush auxLinesBrush(Qt::darkGreen);

    Data::CWPLinesVector::iterator it =
        m_workingSet.m_workpieceLines.begin();
    Data::CWPLinesVector::iterator itLast =
        m_workingSet.m_workpieceLines.end();

    for (; it != itLast; ++it)
    {
        QPoint ptLineFirst;
        QPoint ptLineSecond;
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            it->getLine().p1(), ptLineFirst);
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            it->getLine().p2(), ptLineSecond);

        QPen *pLinePen = 0;
        QBrush *pLineBrush = 0;
        switch (it->getType())
        {
        case Data::CWPLine::WPLine_Main:
            pLinePen = &mainLinesPen;
            pLineBrush = &mainLinesBrush;
            break;
        case Data::CWPLine::WPLine_Aux:
            pLinePen = &auxLinesPen;
            pLineBrush = &auxLinesBrush;
            break;
        case Data::CWPLine::WPLine_GridCompact:
            assert(false && "what the fuck? This case should never be!");
            break;
        }

        painter.setPen(*pLinePen);
        painter.setBrush(*pLineBrush);

        painter.drawLine(ptLineFirst, ptLineSecond);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CZonesPainter::drawGridCompactionLines( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen gridCompactionLinesPen(Qt::magenta, 2);
    QBrush gridCompactionLinesBrush(Qt::magenta);

    Data::CWPLinesVector::iterator it =
        m_workingSet.m_workpieceGridCompactionLines.begin();
    Data::CWPLinesVector::iterator itLast =
        m_workingSet.m_workpieceGridCompactionLines.end();

    for (; it != itLast; ++it)
    {
        QPoint ptLineFirst;
        QPoint ptLineSecond;
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            it->getLine().p1(), ptLineFirst);
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            it->getLine().p2(), ptLineSecond);

        painter.setPen(gridCompactionLinesPen);
        painter.setBrush(gridCompactionLinesBrush);

        painter.drawLine(ptLineFirst, ptLineSecond);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CZonesPainter::drawFinalZones( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen oneZonesPen(Qt::darkRed, 2);
    QBrush oneZonesBrush(Qt::red);
    QPen zeroZonesPen(Qt::darkRed, 2);
    QBrush zeroZonesBrush(Qt::red, Qt::DiagCrossPattern);

    foreach(Data::CWPZone zone, m_workingSet.m_workpieceZones)
    {
        QPen *pZonePen = &oneZonesPen;
        QBrush *pZoneBrush = &oneZonesBrush;
        if (zone.getType() == Data::CWPZone::WPZone_Zero)
        {
            pZonePen = &zeroZonesPen;
            pZoneBrush = &zeroZonesBrush;
        }
        painter.setPen(*pZonePen);
        painter.setBrush(*pZoneBrush);

        Data::CPointsVector points(zone.getPoints());
        QPolygon poly;
        foreach(QPointF ptReal, points)
        {
            QPoint pt;
            m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(ptReal, pt);
            poly << pt;
        }

        painter.drawPolygon(poly);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CZonesPainter::drawZonesPoints( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen pen(Qt::red, 1);
    QBrush brush(Qt::blue);

    painter.setPen(pen);
    painter.setBrush(brush);

    foreach(QPointF pt, m_workingSet.m_workpiecePoints)
    {
        QRectF rc;
        getWPPointMarker(pt, rc);
        painter.drawRect(rc);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CZonesPainter::drawSelectedPoints( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen pen(Qt::red, 1);
    QBrush brush(Qt::yellow);

    painter.setPen(pen);
    painter.setBrush(brush);

    foreach(QPointF pt, m_selectedPoints)
    {
        QRectF rc;
        getWPPointMarker(pt, rc);
        painter.drawRect(rc);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CZonesPainter::drawSelectedZone( QPainter &painter )
{
    if (m_znSelectedZone.isNull()) return;

    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    Qt::BrushStyle bs = Qt::SolidPattern;
    if (m_znSelectedZone.getType() == Data::CWPZone::WPZone_Zero)
        bs = Qt::DiagCrossPattern;

    QPen pen(Qt::darkRed, 2);
    QBrush brush(Qt::yellow, bs);

    painter.setPen(pen);
    painter.setBrush(brush);

    Data::CPointsVector points(m_znSelectedZone.getPoints());
    QPolygon poly;
    foreach(QPointF ptReal, points)
    {
        QPoint pt;
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(ptReal, pt);
        poly << pt;
    }
    painter.drawPolygon(poly);

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CZonesPainter::getWPPointMarker( const QPointF& pt, QRectF &rect )
{
    QPoint ptPointMarker;
    m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(pt, ptPointMarker);

    rect.setX((double)ptPointMarker.x() - g_fWPPointMarkerWidth / 2.0f);
    rect.setY((double)ptPointMarker.y() - g_fWPPointMarkerHeight / 2.0f);
    rect.setWidth(g_fWPPointMarkerWidth);
    rect.setHeight(g_fWPPointMarkerHeight);
}

void CZonesPainter::processSelection( const QPointF& pt )
{
    Data::CPointsVector::iterator it = std::find(m_selectedPoints.begin(),
        m_selectedPoints.end(), pt);

    if (it != m_selectedPoints.end()) m_selectedPoints.erase(it);
    else
    {
        if (m_selectedPoints.size() == 4) return ;
        m_selectedPoints.push_back(pt);
    }

    if (m_selectedPoints.size() == 4)
    {
        if (Data::CWPZone::isWPZone(m_selectedPoints))
        {
            m_workingSet.m_workpieceZones.push_back(
                Data::CWPZone(m_selectedPoints, m_currentZoneType));
            m_selectedPoints.clear();
        }
    }
}

void CZonesPainter::updateSelLineInfo()
{
    m_workingSet.m_labelSelZone->setText(m_znSelectedZone.getStringShort());
}

void CZonesPainter::cleanSelection()
{
    m_znSelectedZone.setNull();
    updateSelLineInfo();
}

bool CZonesPainter::buildZoneMapString( QString &sZoneMapUserFriendly,
                                       size_t *pnXDimension /*= 0*/,
                                       size_t *pnYDimension /*= 0*/ )
{
    bool bOutputDimensionsAvaliable = ((pnXDimension != 0) &&
        (pnYDimension != 0));
    if (bOutputDimensionsAvaliable)
    {
        *pnXDimension = 0;
        *pnYDimension = 0;
    }

    m_workingSet.m_sZoneMap.clear();
    sZoneMapUserFriendly.clear();

    if (m_workingSet.m_workpieceZones.empty())
        return false;

    // If there is only one zone set the zone map to "    1".
    //
    if (m_workingSet.m_workpieceZones.size() == 1)
    {
        sZoneMapUserFriendly = COutputFormatter::formatValue(
            m_workingSet.m_workpieceZones[0].getZoneMapTypeInt(),
            OutputFormat_I2);
        if (bOutputDimensionsAvaliable)
        {
            *pnXDimension = 1;
            *pnYDimension = 1;
        }
        m_workingSet.m_sZoneMap = COutputFormatter::formatValue(
            m_workingSet.m_workpieceZones[0].getZoneMapTypeInt(),
            OutputFormat_I5);
        return true;
    }

    // Sort zones by center point's coords (in top-bottom - left-right order).
    //
    std::sort(m_workingSet.m_workpieceZones.begin(),
        m_workingSet.m_workpieceZones.end(), CZonesComparator());

    size_t nMaxXDimension = 0;
    size_t nCurrentXDimension = 0;

    // Calculating nMaxXDimension: nMaxXDimension is the X dimension of the
    // first line.
    //
    QPointF ptLastCenterPoint(
        m_workingSet.m_workpieceZones[0].getCenterPoint());
    Data::CWPZonesVector::iterator it =
        m_workingSet.m_workpieceZones.begin() + 1;
    Data::CWPZonesVector::iterator itLast = m_workingSet.m_workpieceZones.end();
    for (; it != itLast; ++it)
    {
        if (qFuzzyCompare(ptLastCenterPoint.y(),
            it->getCenterPoint().y()))
            ++nMaxXDimension;
        else break;
    }
    if (bOutputDimensionsAvaliable)
    {
        *pnXDimension = nMaxXDimension + 1;
        *pnYDimension = 1;
    }

    typedef QVector<QVector<int> > CZoneMap;
    CZoneMap zoneMap;
    // Push first vector (line)
    zoneMap.push_back(QVector<int>());

    // Start zone map building:
    // 1) Write first zone's type.
    zoneMap.back().push_back(
        m_workingSet.m_workpieceZones[0].getZoneMapTypeInt());

    // 2) Write left zones' types.
    //
    it = m_workingSet.m_workpieceZones.begin() + 1;
    for (; it != itLast; ++it)
    {
        QPointF ptCurCentralPoint(it->getCenterPoint());
        if (qFuzzyCompare(ptLastCenterPoint.y(), ptCurCentralPoint.y()))
        {
            ++nCurrentXDimension;
            if (nCurrentXDimension > nMaxXDimension)
            {
                sZoneMapUserFriendly = tr("Wrong workpiece geometry:\nLines X"
                    "dimensions aren't equal.");
                m_workingSet.m_sZoneMap.clear();
                if (bOutputDimensionsAvaliable)
                {
                    *pnXDimension = 0;
                    *pnYDimension = 0;
                }
                return true;
            }
        }
        else
        {
            nCurrentXDimension = 0;
            if(bOutputDimensionsAvaliable)
                ++(*pnYDimension);
            zoneMap.push_back(QVector<int>());
            ptLastCenterPoint = ptCurCentralPoint;
        }

        zoneMap.back().push_back(it->getZoneMapTypeInt());
    }

    // Now zone map is builded so we can format and output it
    //
    size_t nZonesInLineCount = 0;

    // Assume that we have an vector of vectors with equal sizes.
    //
    for (size_t i = 0; i < zoneMap.size(); ++i)
    {
        for (size_t j = 0; j < zoneMap.back().size(); ++j)
        {
            int iZoneType = zoneMap[i][j];
            sZoneMapUserFriendly += COutputFormatter::formatValue(iZoneType,
                OutputFormat_I2);

            if (nZonesInLineCount ==
                CZonesPainterWorkingSet::g_nZonesInLineCount)
            {
                m_workingSet.m_sZoneMap += COutputFormatter::formatEndl();
                nZonesInLineCount = 0;
            }
            m_workingSet.m_sZoneMap += COutputFormatter::formatValue(iZoneType,
                OutputFormat_I5);
            ++nZonesInLineCount;
        }
        sZoneMapUserFriendly += COutputFormatter::formatEndl();
    }
    return true;
}

bool CZonesPainter::genereateOutputString( bool bEmitSignals /*= true*/ )
{
    m_workingSet.m_sOutput.clear();

    // 0) Copy working set data to leave the possibility of further editing.
    //
    Data::CWPZonesVector workpieceZones(m_workingSet.m_workpieceZones);

    // 1) Delete all Zero Zones from the working set copy.
    //
    workpieceZones.erase(std::remove_if(workpieceZones.begin(),
        workpieceZones.end(), COneZonesIdentifier()), workpieceZones.end());

    Data::CWPZonesVector::iterator itZone;
    Data::CWPZonesVector::iterator itZoneEnd;

    // 2) Calculate the Zone Map and its dimensions.
    //
    QString sZoneMapUserFriendly;
    size_t nXZoneMapDimension = 0;
    size_t nYZoneMapDimension = 0;
    if (!buildZoneMapString(sZoneMapUserFriendly, &nXZoneMapDimension,
        &nYZoneMapDimension))
        return false;

    // 3) Calculate the Zones' Grid-Compaction poits.
    //
    for (itZone = workpieceZones.begin(), itZoneEnd = workpieceZones.end();
        itZone != itZoneEnd; ++itZone)
    {
        foreach(Data::CWPLine gridCompactionLine,
            m_workingSet.m_workpieceGridCompactionLines)
        {
            if (!itZone->processGridCompactionLine(gridCompactionLine.getLine()))
                return false;
        }
    }

    // 4) Get all points of the workpiece and count the One-Zones quantity.
    //
    size_t nOnesZonesCount = 0;
    std::vector<QPointF> allPointsSTL;
    foreach(Data::CWPZone zone, workpieceZones)
    {
        ++nOnesZonesCount;
        Data::CPointsVector wpPoints(zone.getPoints());
        Data::CPointsVector wpGridCompactionPoints(
            zone.getGridCompactionPoints());
        std::copy(wpPoints.begin(),
            wpPoints.end(), std::back_inserter(allPointsSTL));
        std::copy(wpGridCompactionPoints.begin(),
            wpGridCompactionPoints.end(), std::back_inserter(allPointsSTL));

        //         allPointsSTL.insert(allPointsSTL.end(), wpPoints.begin(),
        //             wpPoints.end());
        //         allPointsSTL.insert(allPointsSTL.end(), wpGridCompactionPoints.begin(),
        //             wpGridCompactionPoints.end());
    }

    if (!nOnesZonesCount)
        // There is no output.
        return false;

    // 5) Remove duplicate points
    //
    std::sort(allPointsSTL.begin(), allPointsSTL.end(),
        CPointsFLessComparator());
    allPointsSTL.erase(std::unique(allPointsSTL.begin(), allPointsSTL.end()),
        allPointsSTL.end());

    QString sOutput;
    QTextStream ss(&sOutput);

    // 6) Write first line of the output. See documentation.
    //
    ss << COutputFormatter::formatValue(nOnesZonesCount, OutputFormat_I5);
    ss << COutputFormatter::formatValue(nXZoneMapDimension, OutputFormat_I5);
    ss << COutputFormatter::formatValue(nYZoneMapDimension, OutputFormat_I5);
    ss << COutputFormatter::formatValue(allPointsSTL.size(), OutputFormat_I5);
    ss << COutputFormatter::formatEndl();

    // 7) Write all one-zones indexes. Two zone in one line
    //
    bool bFirstInLine = true;
    Data::CPointsVector allPointsQt = Data::CPointsVector::fromStdVector(
        allPointsSTL);
    for (itZone = workpieceZones.begin(), itZoneEnd = workpieceZones.end();
        itZone != itZoneEnd; ++itZone)
    {
        QString sZoneOutput;
        if (!itZone->getOutputString(allPointsQt, sZoneOutput))
            return false;
        if (bFirstInLine)
        {
            bFirstInLine = false;
            ss << sZoneOutput;
        }
        else
        {
            ss << sZoneOutput;
            ss << COutputFormatter::formatEndl();
            bFirstInLine = true;
        }
    }
    if (!bFirstInLine)
        ss << COutputFormatter::formatEndl();

    // 8) Write Zone-Map
    //
    ss << m_workingSet.m_sZoneMap;
    ss << COutputFormatter::formatEndl();

    // 9) Write One-Zones fragmentation preferences
    //
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_iZonesFragmenationXCount,
        OutputFormat_I5);
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_iZonesFragmenationYCount,
        OutputFormat_I5);
    ss << COutputFormatter::formatEndl();

    // 10) Write all points. See documentation.
    //
    foreach(QPointF pt, allPointsSTL)
    {
        ss << COutputFormatter::formatValue(pt.x(), OutputFormat_F6_2);
        ss << COutputFormatter::formatValue(pt.y(), OutputFormat_F6_2);
        ss << COutputFormatter::formatEndl();
    }

    ss << flush;

    m_workingSet.m_sOutput = sOutput;
    m_workingSet.m_labelZoneMap->setText(sZoneMapUserFriendly);

    if (bEmitSignals)
        emit dataReady();
    return true;
}

void CZonesPainter::onCompileData()
{
    if (QMessageBox::question(m_pWorkAreaWidget, tr("Are you sure?"),
        tr("Write output workpiece data?"), QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes) == QMessageBox::Yes)
        genereateOutputString();
}

void CZonesPainter::onClearWorkspace()
{
    m_workingSet.m_workpieceZones.clear();
    cleanSelection();
    m_pWorkAreaWidget->update();
}

void CZonesPainter::onChangeType()
{
    m_currentZoneType = (m_currentZoneType == Data::CWPZone::WPZone_One)?
        Data::CWPZone::WPZone_Zero : Data::CWPZone::WPZone_One;
    m_workingSet.m_labelZoneType->setText(Data::CWPZone::getTypeString(
        m_currentZoneType));
}

void CZonesPainter::onRestoreLastDeleted()
{
    if (m_deletedZones.empty())
        return;

    m_workingSet.m_workpieceZones.push_back(m_deletedZones.top());
    m_znSelectedZone = m_deletedZones.top();
    m_deletedZones.pop();

    updateSelLineInfo();
    m_pWorkAreaWidget->update();
}

void CZonesPainter::onDeleteSelected()
{
    if (m_znSelectedZone.isNull())
        return;

    Data::CWPZonesVector::iterator it = std::find(
        m_workingSet.m_workpieceZones.begin(),
        m_workingSet.m_workpieceZones.end(),
        m_znSelectedZone);

    if (it != m_workingSet.m_workpieceZones.end())
    {
        m_deletedZones.push(*it);
        m_workingSet.m_workpieceZones.erase(it);            
    }

    cleanSelection();
    m_pWorkAreaWidget->update();
}

void CZonesPainter::onDeleteLastCreated()
{
    if (m_workingSet.m_workpieceZones.empty())
        return;

    m_deletedZones.push(m_workingSet.m_workpieceZones.back());
    m_workingSet.m_workpieceZones.erase(
        m_workingSet.m_workpieceZones.end() - 1);

    m_znSelectedZone.setNull();
    updateSelLineInfo();

    m_pWorkAreaWidget->update();
}

void CZonesPainter::onClearDeletionHistory()
{
    m_deletedZones.clear();
}

void CZonesPainter::onBuildZoneMap()
{
    QString sZonesMap;
    if (!buildZoneMapString(sZonesMap)) sZonesMap = tr("Not enought data.");
    m_workingSet.m_labelZoneMap->setText(sZonesMap);
}