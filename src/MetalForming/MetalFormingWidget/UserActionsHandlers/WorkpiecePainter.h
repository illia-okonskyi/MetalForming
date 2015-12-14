#pragma once
#include "MetalFormingStrategy.h"
#include "../WorkingSets/WorkpiecePainterWorkingSet.h"

#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QtGui/QPainter>

class CWorkpiecePainter : public IMetalFormingStrategy
{
public:
    Q_OBJECT;

public:
    CWorkpiecePainter(CBaseWorkingSet &baseWorkingSet, QWidget *pWorkAreaWidget,
    CWorkpiecePainterWorkingSet &workingSet);

    virtual void init();
    virtual void deinit();

    virtual bool mousePressEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool mouseReleaseEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool mouseMoveEventHandler (QWidget *pWidget, QMouseEvent * pEvent);
    virtual bool mouseClickEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool keyPressEventHandler(QWidget *pWidget, QKeyEvent * pEvent);

    virtual bool paintEventHandler(QWidget *pWidget, QPaintEvent *pEvent);

    // Returns true if input & working data have been transformed into
    // output data with success.
    bool compile();

signals:
    void multiplySelection(const QStringList &, int &);
    void dataReady();

protected slots:
    void onCompileData();
    void onClearWorkspace();
    void onChangeType();
    void onRestoreLastDeleted();
    void onDeleteSelected();
    void onDeleteLastCreated();
    void onClearDeletionHistory();

protected:
    CWorkpiecePainterWorkingSet &m_workingSet;

private:
    Data::CWPLine::CWPLineType m_currentLineType;
    QPoint m_ptPressed;
    QPoint m_ptMoved;
    bool m_bDrawningNextLine;
    Data::CWPLine m_lnSelectedLine;
    Data::CWPLine m_lnAngleLine;

    QStack<Data::CWPLine> m_deletedLines;

private:
    void drawFinalLines(QPainter &painter);
    void drawCurrentLine(QPainter &painter);
    void drawSelection(QPainter &painter);

    void findWPLineByEndPoint(const QPoint &pt, Data::CWPLine &line);

    void getWPLineMarkerRect(const QPoint &ptLineMarker, QRectF &rect);
    void getWPLineMarkerRect(const Data::CWPLine &line, QRectF &rect);
    void getWPLineMarkerRect(const QPoint &ptFirst,
        const QPoint &ptSecond, QRectF &rect);


    void cleanSelection();
    void updateSelLineInfo();

    void fillWorkpiecePoints(bool bEmitSignals = true);

    bool findFuzzyEntrance(const QPointF &ptToFind);
};