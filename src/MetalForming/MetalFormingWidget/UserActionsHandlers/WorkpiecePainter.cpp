#include "WorkpiecePainter.h"
#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QKeyEvent>

#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtCore/QStringList>

#include <Windows.h>
#include <algorithm>
#include <functional>

#include "../Tools/Tools.h"
double g_fWPLineMarkerWidth = 10.0f;
double g_fWPLineMarkerHeight = 10.0f;
unsigned int g_nWPLineEndsMarkerRadius = 3;


CWorkpiecePainter::CWorkpiecePainter(
    CBaseWorkingSet &baseWorkingSet, QWidget *pWorkAreaWidget,
     CWorkpiecePainterWorkingSet &workingSet):
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget), m_workingSet(workingSet)
{

}

void CWorkpiecePainter::init()
{
    m_currentLineType = Data::CWPLine::WPLine_Main;
    m_ptMoved.setX(0);
    m_ptMoved.setY(0);
    m_ptPressed.setX(0);
    m_ptPressed.setY(0);
    m_bDrawningNextLine = false;

    m_workingSet.m_gbInfo->setVisible(true);
    m_workingSet.m_labelLineAngle->setText("-");
    m_workingSet.m_labelLineType->setText(
        Data::CWPLine::getTypeString(m_currentLineType));
    updateSelLineInfo();

    m_deletedLines.clear();

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

void CWorkpiecePainter::deinit()
{
    disconnect();

    m_workingSet.m_gbInfo->setVisible(false);
}

bool CWorkpiecePainter::compile()
{
    fillWorkpiecePoints(false);
    return true;
}

bool CWorkpiecePainter::mousePressEventHandler(QWidget *pWidget,
                                               QMouseEvent *pEvent )
{
    if (pEvent->buttons().testFlag(Qt::RightButton)) return true;
    m_ptPressed = pEvent->pos();
    m_ptMoved = m_ptPressed;
    findWPLineByEndPoint(m_ptPressed, m_lnAngleLine);
    m_baseWorkingSet.m_grid.bindScreenPointToGrid(m_ptPressed);
    m_baseWorkingSet.m_grid.bindScreenPointToGrid(m_ptMoved);
    m_bDrawningNextLine = true;
    cleanSelection();
    pWidget->update();
    return true;
}

bool CWorkpiecePainter::mouseReleaseEventHandler(QWidget *pWidget,
                                                 QMouseEvent *pEvent )
{
    if (!m_bDrawningNextLine) return true;
    QPoint ptReleased = pEvent->pos();
    m_baseWorkingSet.m_grid.bindScreenPointToGrid(ptReleased);
    if (ptReleased != m_ptPressed)
    {
        QPointF ptLineFirst;
        QPointF ptLineSecond;
        m_baseWorkingSet.m_grid.transformScreenPointToRealPoint(m_ptPressed,
            ptLineFirst);
        m_baseWorkingSet.m_grid.transformScreenPointToRealPoint(ptReleased,
            ptLineSecond);

        m_workingSet.m_workpieceLines.push_back(Data::CWPLine(
            QLineF(ptLineFirst, ptLineSecond), m_currentLineType));
    }
    m_bDrawningNextLine = false;
    pWidget->update();
    return true;
}

bool CWorkpiecePainter::mouseMoveEventHandler(QWidget *pWidget,
                                              QMouseEvent * pEvent )
{
    if (m_bDrawningNextLine)
    {
        if (pEvent->buttons().testFlag(Qt::RightButton))
        {
            m_bDrawningNextLine = false;
            return true;
        }
        else if (pEvent->buttons().testFlag(Qt::LeftButton))
        {
            m_ptMoved = pEvent->pos();
            m_baseWorkingSet.m_grid.bindScreenPointToGrid(m_ptMoved);
        }

        QPointF ptPressedReal;
        QPointF ptMovedReal;
        m_baseWorkingSet.m_grid.transformScreenPointToRealPoint(m_ptPressed,
            ptPressedReal);
        m_baseWorkingSet.m_grid.transformScreenPointToRealPoint(m_ptMoved,
            ptMovedReal);
        Data::CWPLine currentLine(QLineF(ptPressedReal, ptMovedReal));
        double fAngle = (m_lnAngleLine.isNull())? currentLine.getYAsisAngle() :
            m_lnAngleLine.getAngleTo(currentLine);
        
        
        m_workingSet.m_labelLineAngle->setText(QString("%1(%2)").
            arg(fAngle, 0, 'f', 2).
            arg(360.0f - fAngle, 0, 'f', 2));
        pWidget->update();
    }
    return true;
}

bool CWorkpiecePainter::paintEventHandler( QWidget *pWidget,
                                          QPaintEvent *pEvent )
{
    QPainter painter(pWidget);
    drawFinalLines(painter);
    if (m_bDrawningNextLine)
        drawCurrentLine(painter);
    drawSelection(painter);
    return true;
}

bool CWorkpiecePainter::mouseClickEventHandler( QWidget *pWidget,
                                               QMouseEvent *pEvent )
{
    if (m_bDrawningNextLine) return false;
    Data::CWPLinesVector::iterator it =
        m_workingSet.m_workpieceLines.begin();
    Data::CWPLinesVector::iterator itLast =
        m_workingSet.m_workpieceLines.end();

    Data::CWPLinesVector foundedLines;

    for (; it != itLast; ++it)
    {
        QRectF rcLineMarker;
        getWPLineMarkerRect(*it, rcLineMarker);

        if (rcLineMarker.contains(QPointF(pEvent->pos())))
            foundedLines.push_back(*it);
    }

    if (foundedLines.empty())
    {
        cleanSelection();
        return true;
    }
    if (foundedLines.size() == 1)
    {
        m_lnSelectedLine = *foundedLines.begin();
    }
    else
    {
        QStringList labels;
        it = foundedLines.begin();
        itLast = foundedLines.end();
        for (; it != itLast; ++it)
            labels.push_back(it->getStringLong());

        int selRes = -1;
        emit multiplySelection(labels, selRes);
        if (selRes == -1)
        {
            cleanSelection();
            return true;
        }
        
        m_lnSelectedLine = foundedLines[selRes];
    }

    updateSelLineInfo();
    return true;
}

bool CWorkpiecePainter::keyPressEventHandler( QWidget *pWidget,
                                             QKeyEvent * pEvent )
{
    bool bHandled = true;

    switch(pEvent->key())
    {
    case Qt::Key_Escape:
        // Clean workspace
        onClearWorkspace();
        break;

    case Qt::Key_Delete:
        // Delete selected line
        onDeleteSelected();
        break;

    case Qt::Key_F2:
        // Prepare final data
        onCompileData();
        break;

    case Qt::Key_Shift:
        // Change line type
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
            // Delete previos line
            onDeleteLastCreated();
            break;

        case Qt::Key_R:
            // Restore last deleted line
            onRestoreLastDeleted();
            break;

        case Qt::Key_X:
            // Clean deletion history
            onClearDeletionHistory();
            break;

        default:
            bHandled = false;
        }
    }

    return (!bHandled);
}

void CWorkpiecePainter::drawFinalLines( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen mainLinesPen(Qt::blue, 2);
    QBrush mainLinesBrush(Qt::blue);
    QPen auxLinesPen(Qt::darkGreen, 2);
    QBrush auxLinesBrush(Qt::darkGreen);
    QPen gridCompactLinesPen(Qt::magenta, 2);
    QBrush gridCompactLinesBrush(Qt::magenta);

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
            pLinePen = &gridCompactLinesPen;
            pLineBrush = &gridCompactLinesBrush;
            break;
        }

        painter.setPen(*pLinePen);
        painter.setBrush(*pLineBrush);

        painter.drawEllipse(ptLineFirst, g_nWPLineEndsMarkerRadius,
            g_nWPLineEndsMarkerRadius);
        painter.drawEllipse(ptLineSecond, g_nWPLineEndsMarkerRadius,
            g_nWPLineEndsMarkerRadius);
        painter.drawLine(ptLineFirst, ptLineSecond);
        QRectF rcLineMarker;
        getWPLineMarkerRect(*it, rcLineMarker);
        painter.drawRect(rcLineMarker);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);

}

void CWorkpiecePainter::drawCurrentLine( QPainter &painter )
{
    if (m_ptPressed == m_ptMoved) return;
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen currentLinePen(Qt::red, 2);
    QBrush currentLineBrush(Qt::red, Qt::SolidPattern);

    painter.setPen(currentLinePen);
    painter.setBrush(currentLineBrush);

    QRectF rcLineMarker;
    getWPLineMarkerRect(m_ptPressed, m_ptMoved, rcLineMarker);

    painter.drawEllipse(m_ptPressed, g_nWPLineEndsMarkerRadius,
        g_nWPLineEndsMarkerRadius);
    painter.drawEllipse(m_ptMoved, g_nWPLineEndsMarkerRadius,
        g_nWPLineEndsMarkerRadius);
    painter.drawLine(m_ptPressed, m_ptMoved);
    painter.drawRect(rcLineMarker);

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CWorkpiecePainter::drawSelection( QPainter &painter )
{
    if (m_lnSelectedLine.isNull()) return;
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen selectedLinePen(Qt::yellow, 2);
    QBrush selectedLineBrush(Qt::darkYellow, Qt::SolidPattern);

    painter.setPen(selectedLinePen);
    painter.setBrush(selectedLineBrush);

    QPoint ptLineFirst;
    QPoint ptLineSecond;
    m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
        m_lnSelectedLine.getLine().p1(), ptLineFirst);
    m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
        m_lnSelectedLine.getLine().p2(), ptLineSecond);

    painter.drawEllipse(ptLineFirst, g_nWPLineEndsMarkerRadius,
        g_nWPLineEndsMarkerRadius);
    painter.drawEllipse(ptLineSecond, g_nWPLineEndsMarkerRadius,
        g_nWPLineEndsMarkerRadius);
    painter.drawLine(ptLineFirst, ptLineSecond);
    QRectF rcLineMarker;
    getWPLineMarkerRect(m_lnSelectedLine, rcLineMarker);
    painter.drawRect(rcLineMarker);

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CWorkpiecePainter::findWPLineByEndPoint( const QPoint &pt,
                                             Data::CWPLine &line )
{
    Data::CWPLinesVector::iterator it =
        m_workingSet.m_workpieceLines.begin();
    Data::CWPLinesVector::iterator itLast =
        m_workingSet.m_workpieceLines.end();

    QPoint ptBinded(pt);
    QPointF ptReal;

    m_baseWorkingSet.m_grid.bindScreenPointToGrid(ptBinded);
    m_baseWorkingSet.m_grid.transformScreenPointToRealPoint(ptBinded, ptReal);

    for (; it != itLast; ++it)
    {
        if ((it->getLine().p2().toPoint() == ptReal.toPoint())
            || (it->getLine().p1().toPoint() == ptReal.toPoint()))
        {
            line = *it;
            return ;
        }
    }

    line.setNull();
}

void CWorkpiecePainter::getWPLineMarkerRect( const QPoint &ptLineMarker,
                                            QRectF &rect )
{
    rect.setX((double)ptLineMarker.x() - g_fWPLineMarkerWidth / 2.0f);
    rect.setY((double)ptLineMarker.y() - g_fWPLineMarkerHeight / 2.0f);
    rect.setWidth(g_fWPLineMarkerWidth);
    rect.setHeight(g_fWPLineMarkerHeight);
}

void CWorkpiecePainter::getWPLineMarkerRect( const Data::CWPLine &line,
                                            QRectF &rect )
{
    QPoint ptLineMarker;
    m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
        line.getCenterPoint(), ptLineMarker);

    getWPLineMarkerRect(ptLineMarker, rect);
}

void CWorkpiecePainter::getWPLineMarkerRect(
    const QPoint &ptFirst, const QPoint &ptSecond, QRectF &rect )
{
    QPoint ptLineMarker = QLineF(ptFirst, ptSecond).pointAt(0.5).toPoint();

    getWPLineMarkerRect(ptLineMarker, rect);
}

void CWorkpiecePainter::updateSelLineInfo()
{
    m_workingSet.m_labelSelLine->setText(m_lnSelectedLine.getStringShort());
}

void CWorkpiecePainter::cleanSelection()
{
    m_lnSelectedLine.setNull();
    updateSelLineInfo();
}

void CWorkpiecePainter::fillWorkpiecePoints( bool bEmitSignals /*= true*/ )
{
    m_workingSet.m_workpiecePoints.clear();
    Data::CWPLinesVector::iterator itLine1 =
        m_workingSet.m_workpieceLines.begin();
    Data::CWPLinesVector::iterator itLast =
        m_workingSet.m_workpieceLines.end();

    for (; itLine1 != itLast; ++itLine1)
    {
        if (itLine1->getType() == Data::CWPLine::WPLine_GridCompact)
            continue;

        Data::CWPLinesVector::iterator itLine2 = itLine1 + 1;
        for (; itLine2 != itLast; ++itLine2)
        {
            if (itLine2->getType() == Data::CWPLine::WPLine_GridCompact)
                continue;

            QPointF ptIntersection;
            if (itLine1->getIntersectPoint(*itLine2, ptIntersection))
            {
                if (!findFuzzyEntrance(ptIntersection))
                    m_workingSet.m_workpiecePoints.push_back(ptIntersection);
            }
        }
    }

    if (bEmitSignals)
        emit dataReady();
}

bool CWorkpiecePainter::findFuzzyEntrance( const QPointF &ptToFind )
{
    if (m_workingSet.m_workpiecePoints.empty()) return false;

    foreach(QPointF pt, m_workingSet.m_workpiecePoints)
        if (Tools::isPointsFuzzyEqual(pt, ptToFind)) return true;

    return false;
}

void CWorkpiecePainter::onCompileData()
{
    if (QMessageBox::question(m_pWorkAreaWidget, tr("Are you sure?"),
        tr("Write workpiece points?"), QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes) == QMessageBox::Yes)
        fillWorkpiecePoints();
}

void CWorkpiecePainter::onClearWorkspace()
{
    m_workingSet.m_workpieceLines.clear();
    cleanSelection();
    m_pWorkAreaWidget->update();
}

void CWorkpiecePainter::onChangeType()
{
    switch (m_currentLineType)
    {
    case Data::CWPLine::WPLine_Main:
        m_currentLineType = Data::CWPLine::WPLine_Aux;
        break;
    case Data::CWPLine::WPLine_Aux:
        m_currentLineType = Data::CWPLine::WPLine_GridCompact;
        break;
    case Data::CWPLine::WPLine_GridCompact:
        m_currentLineType = Data::CWPLine::WPLine_Main;
        break;
    }
    m_workingSet.m_labelLineType->setText(Data::CWPLine::getTypeString(
        m_currentLineType));
}

void CWorkpiecePainter::onRestoreLastDeleted()
{
    if (m_deletedLines.empty())
        return;

    m_workingSet.m_workpieceLines.push_back(m_deletedLines.top());
    m_lnSelectedLine = m_deletedLines.top();
    m_deletedLines.pop();

    updateSelLineInfo();
    m_pWorkAreaWidget->update();
}

void CWorkpiecePainter::onDeleteSelected()
{
    if (m_lnSelectedLine.isNull())
        return;

    Data::CWPLinesVector::iterator it = std::find(
        m_workingSet.m_workpieceLines.begin(),
        m_workingSet.m_workpieceLines.end(),
        m_lnSelectedLine);

    if (it != m_workingSet.m_workpieceLines.end())
    {
        m_deletedLines.push(*it);
        m_workingSet.m_workpieceLines.erase(it);            
    }

    cleanSelection();
    m_pWorkAreaWidget->update();
}

void CWorkpiecePainter::onDeleteLastCreated()
{
    if (m_workingSet.m_workpieceLines.empty())
        return;

    m_deletedLines.push(m_workingSet.m_workpieceLines.back());
    m_workingSet.m_workpieceLines.erase(
        m_workingSet.m_workpieceLines.end() - 1);

    m_lnSelectedLine.setNull();
    updateSelLineInfo();
    m_pWorkAreaWidget->update();
}

void CWorkpiecePainter::onClearDeletionHistory()
{
    m_deletedLines.clear();
}