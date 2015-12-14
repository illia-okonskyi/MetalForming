#pragma once
#include "MetalFormingStrategy.h"
#include "../WorkingSets/InstrumentsPainterWorkingSet.h"

#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QtGui/QPainter>

#include <QtSolutions/qtvariantproperty>

class CInstrumentsPainter : public IMetalFormingStrategy
{
public:
    Q_OBJECT;

public:
    CInstrumentsPainter(CBaseWorkingSet &baseWorkingSet,
        QWidget *pWorkAreaWidget, CInstrumentsPainterWorkingSet &workingSet);

    virtual void init();
    virtual void deinit();

    virtual bool mousePressEventHandler(QWidget *pWidget, QMouseEvent *pEvent);
    virtual bool mouseReleaseEventHandler(QWidget *pWidget,
        QMouseEvent *pEvent);
    virtual bool mouseMoveEventHandler(QWidget *pWidget, QMouseEvent * pEvent);
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
    void onPropertyValueChanged(QtProperty *pProperty, const QVariant &value );

    void onDataReady();
    void onClearWorkspace();
    void onRestoreLastDeleted();
    void onDeleteSelected();
    void onDeleteLastCreated();
    void onClearDeletionHistory();

protected:
    CInstrumentsPainterWorkingSet &m_workingSet;

private:
    QPoint m_ptPressed;
    QPoint m_ptMoved;
    bool m_bDrawningNextInstrument;
    Data::CWPInstrument m_selectedInstrument;

    QStack<Data::CWPInstrument> m_deletedInstruments;
    typedef QMap<QtVariantProperty *,
        Data::CWPInstrument::CWPInstrumentProperties> CPropertiesMap;
    typedef QMap<Data::CWPInstrument::CWPInstrumentProperties,
        QtVariantProperty *> CInversePropertiesMap;

    CPropertiesMap m_propertiesMap;
    CInversePropertiesMap m_inversePropertiesMap;

private:
    void drawWorkpice(QPainter &painter);
    void drawFinalInstruments(QPainter &painter);
    void drawCurrentInstrument(QPainter &painter);
    void drawSelection(QPainter &painter);

    void getWPInstrumentMarkerRect(const QPoint &ptInstrumentMarker,
        QRectF &rect);
    void getWPInstrumentMarkerRect(const Data::CWPInstrument &instrument,
        QRectF &rect);
    void getWPInstrumentMarkerRect(const QPoint &ptFirst,
        const QPoint &ptSecond, QRectF &rect);

    void updateSelInstrumentInfo();

    void setSelection(const Data::CWPInstrument &instrument);
    void cleanSelection();
    bool selectionAvaliable();

    void createPropertiesList();
    void initSelInstrumentProperties();
    void updateAvaliableProperties();

    bool generateOutputString(bool bEmitSignals = true);
};