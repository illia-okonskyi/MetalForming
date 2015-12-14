#pragma once
#include "MetalFormingStrategy.h"
#include "../WorkingSets/ZonesPainterWorkingSet.h"

#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QtGui/QPainter>

class CZonesPainter : public IMetalFormingStrategy
{
public:
    Q_OBJECT;

public:
    CZonesPainter(CBaseWorkingSet &baseWorkingSet, QWidget *pWorkAreaWidget,
        CZonesPainterWorkingSet &workingSet);

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
    void dataReady();

protected slots:
    void onCompileData();
    void onClearWorkspace();
    void onChangeType();
    void onRestoreLastDeleted();
    void onDeleteSelected();
    void onDeleteLastCreated();
    void onClearDeletionHistory();
    void onBuildZoneMap();

protected:
    CZonesPainterWorkingSet &m_workingSet;

private:
    Data::CPointsVector m_selectedPoints;
    Data::CWPZone::CWPZoneType m_currentZoneType;
    Data::CWPZone m_znSelectedZone;
    QStack<Data::CWPZone> m_deletedZones;

private:
    void drawFinalLines(QPainter &painter);
    void drawGridCompactionLines(QPainter &painter);
    void drawFinalZones(QPainter &painter);
    void drawZonesPoints(QPainter &painter);

    void drawSelectedPoints(QPainter &painter);
    void drawSelectedZone(QPainter &painter);

    void getWPPointMarker(const QPointF& pt, QRectF &rect);

    void processSelection(const QPointF& pt);
    void cleanSelection();
    void updateSelLineInfo();

    bool buildZoneMapString(QString &sZoneMapUserFriendly, size_t *pnXDimension = 0,
        size_t *pnYDimension = 0);

    bool genereateOutputString(bool bEmitSignals = true);
};