#ifndef METALFORMINGWIDGET_H
#define METALFORMINGWIDGET_H

#include <QWidget>
#include "ui_metalformingwidget.h"

#include "UserActionsHandlers/GridPainter.h"
#include "UserActionsHandlers/PropertiesSetter.h"
#include "UserActionsHandlers/CoordinatesMonitor.h"
#include "UserActionsHandlers/ScaleSetter.h"
#include "UserActionsHandlers/WorkpiecePainter.h"
#include "UserActionsHandlers/ZonesPainter.h"
#include "UserActionsHandlers/InstrumentsPainter.h"

#include "UserActionsHandlers/ProjectActionsHandler.h"

#include "MultiplySelectionDialog/MultiplySelectionDialog.h"

#include <QtSolutions/qtpropertymanager.h>
#include <QtSolutions/qtvariantproperty>

class CMetalFormingWidget : public QWidget
{
    Q_OBJECT

public:
    CMetalFormingWidget(QWidget *parent = 0);
    ~CMetalFormingWidget();

    void init(const QString &sStartupProjectFilePath = QString());

signals:
    void projectLoaded(const QString &sProjectFilePath);
    void projectSaved(const QString &sProjectFilePath);
    void pageFormatChanged(Data::CPageFormat pf);

protected slots:
    void onSetPropertiesClicked();
    void onDrawWorkpiceClicked();
    void onDrawZonesClicked();
    void onDrawInstrumentsClicked();
    void onBuildProjectClicked();

    void onMultiplySelection(const QStringList &labels, int &res);

    void onWPPropertiesDataReady();
    void onWPLinesReady();
    void onWPZonesReady();
    void onInstrumentsReady();

    void pushPropertiesSetterStrategy();

    bool onSaveProject(const QString &sFileName);
    bool onLoadProject(const QString &sFileName);
    void onNewProject();

    void showAboutDialog();
private:
    Ui::CMetalFormingWidgetClass ui;

    QtVariantPropertyManager *m_pPropertiesVariantManager;
    QtVariantEditorFactory *m_pPropertiesVariantFactory;

private:
    CBaseWorkingSet m_wsBase;
    CPropertiesSetterWorkingSet m_wsPropertiesSetter;
    CWorkpiecePainterWorkingSet m_wsWPPainter;
    CCoordinatesMonitorWorkingSet m_wsCoordsMon;
    CScaleSetterWorkingSet m_wsScaleSetter;
    CZonesPainterWorkingSet m_wsZonesPainter;
    CInstrumentsPainterWorkingSet m_wsInstrumentsPainter;

    CProjectActionsHandlerWorkingSet m_wsProjectActionsHandler;

private:
    enum CStrategies
    {
        Strategy_CoordsMon,
        Strategy_GridPainter,
        Strategy_InstrsPainter,
        Strategy_ProjActionsHandler,
        Strategy_PropsSetter,
        Strategy_ScaleSetter,
        Strategy_WPPainter,
        Strategy_ZonesPainter
    };
    enum CEditStates
    {
        EditState_WPProps,
        EditState_WPLines,
        EditState_WPZones,
        EditState_WPInstrs
    };

    IMetalFormingStrategy *createAndInitStrategy(CStrategies strategy);
    void initToolbarForStrategy(CStrategies strategy);
    
    void processWPPropsCompiledData();
    void processWPLinesCompiledData();
    void processWPZonesCompiledData();
    void processWPInstrumentsCompiledData();

    void buildProject();

    void updateStrategySelectorsButtons(CEditStates es);
private:
    void applyCurrentPageFormat();
};

#endif // METALFORMINGWIDGET_H
