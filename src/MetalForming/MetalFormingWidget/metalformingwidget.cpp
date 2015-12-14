#include "metalformingwidget.h"
#include "MFProjectFileDefinitions.h"
#include "AboutDialog/AboutDialog.h"

#include <QDialog>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QMessageBox>
#include <cassert>

#ifdef _DEBUG
#pragma comment (lib, "QtSolutions_PropertyBrowser-2.5d.lib")
#else
#pragma comment (lib, "QtSolutions_PropertyBrowser-2.5.lib")
#endif // _DEBUG

CMetalFormingWidget::CMetalFormingWidget(QWidget *parent /*= 0*/)
    : QWidget(parent)
{
    ui.setupUi(this);

    m_pPropertiesVariantManager = new QtVariantPropertyManager();
    m_pPropertiesVariantFactory = new QtVariantEditorFactory();

    ui.propertiesEditor->setFactoryForManager(
        m_pPropertiesVariantManager, m_pPropertiesVariantFactory);
    ui.propertiesEditor->setPropertiesWithoutValueMarked(true);
    ui.propertiesEditor->setRootIsDecorated(false);
    ui.propertiesEditor->setVisible(false);
    ui.propertiesEditor->setResizeMode(QtTreePropertyBrowser::Interactive);
    ui.propertiesEditor->setSplitterPosition(100);
    ui.propertiesEditor->setAlternatingRowColors(true);

    ui.gbCoordsInfo->setVisible(false);
    ui.gbLineInfo->setVisible(false);
    ui.gbZoneInfo->setVisible(false);
    ui.gbZoneMapInfo->setVisible(false);
    ui.gbInstrumentInfo->setVisible(false);

    m_wsCoordsMon.m_gbWS = ui.gbCoordsInfo;
    m_wsCoordsMon.m_labelXPos = ui.labelXPos;
    m_wsCoordsMon.m_labelYPos = ui.labelYPos;

    m_wsScaleSetter.m_labelScale = ui.labelScale;
    m_wsScaleSetter.m_btnScaleIncrease = ui.btnScaleIncrease;
    m_wsScaleSetter.m_btnScaleDecrease = ui.btnScaleDecrease;
    m_wsScaleSetter.m_btnScaleReset = ui.btnScaleReset;
    m_wsScaleSetter.m_scrollArea = ui.paintAreaScroll;

    m_wsPropertiesSetter.m_propertiesManager = m_pPropertiesVariantManager;
    m_wsPropertiesSetter.m_propertiesEditor = ui.propertiesEditor;
    m_wsPropertiesSetter.m_btnCompileData = ui.btnCompileData;

    m_wsWPPainter.m_gbInfo = ui.gbLineInfo;
    m_wsWPPainter.m_labelLineAngle = ui.labelLineAngle;
    m_wsWPPainter.m_labelLineType = ui.labelLineType;
    m_wsWPPainter.m_labelSelLine = ui.labelSelLine;
    m_wsWPPainter.m_btnCompileData = ui.btnCompileData;
    m_wsWPPainter.m_btnClearWorkspace = ui.btnClearWorkspace;
    m_wsWPPainter.m_btnChangeType = ui.btnChangeType;
    m_wsWPPainter.m_btnRestoreLastDeleted = ui.btnRestoreLastDeleted;
    m_wsWPPainter.m_btnDeleteSelected = ui.btnDeleteSelected;
    m_wsWPPainter.m_btnDeleteLastCreated = ui.btnDeleteLastCreated;
    m_wsWPPainter.m_btnClearDeletionHistory = ui.btnClearDeletionHistory;

    m_wsZonesPainter.m_gbInfo = ui.gbZoneInfo;
    m_wsZonesPainter.m_labelZoneType = ui.labelZoneType;
    m_wsZonesPainter.m_labelSelZone = ui.labelSelZone;
    m_wsZonesPainter.m_gbZoneMapInfo = ui.gbZoneMapInfo;
    m_wsZonesPainter.m_labelZoneMap = ui.labelZoneMap;
    m_wsZonesPainter.m_btnCompileData = ui.btnCompileData;
    m_wsZonesPainter.m_btnClearWorkspace = ui.btnClearWorkspace;
    m_wsZonesPainter.m_btnChangeType = ui.btnChangeType;
    m_wsZonesPainter.m_btnRestoreLastDeleted = ui.btnRestoreLastDeleted;
    m_wsZonesPainter.m_btnDeleteSelected = ui.btnDeleteSelected;
    m_wsZonesPainter.m_btnDeleteLastCreated = ui.btnDeleteLastCreated;
    m_wsZonesPainter.m_btnClearDeletionHistory = ui.btnClearDeletionHistory;

    m_wsInstrumentsPainter.m_gbInfo = ui.gbInstrumentInfo;
    m_wsInstrumentsPainter.m_labelInstrumentAngle = ui.labelInstrumentAngle;
    m_wsInstrumentsPainter.m_labelSelInstrument = ui.labelSelInstrument;
    m_wsInstrumentsPainter.m_propertiesManager = m_pPropertiesVariantManager;
    m_wsInstrumentsPainter.m_propertiesEditor = ui.propertiesEditor;
    m_wsInstrumentsPainter.m_btnCompileData = ui.btnCompileData;
    m_wsInstrumentsPainter.m_btnClearWorkspace = ui.btnClearWorkspace;
    m_wsInstrumentsPainter.m_btnRestoreLastDeleted = ui.btnRestoreLastDeleted;
    m_wsInstrumentsPainter.m_btnDeleteSelected = ui.btnDeleteSelected;
    m_wsInstrumentsPainter.m_btnDeleteLastCreated = ui.btnDeleteLastCreated;
    m_wsInstrumentsPainter.m_btnClearDeletionHistory = ui.btnClearDeletionHistory;

    m_wsProjectActionsHandler.m_btnNewProject = ui.btnNewProject;
    m_wsProjectActionsHandler.m_btnLoadProject = ui.btnLoadProject;
    m_wsProjectActionsHandler.m_btnSaveProject = ui.btnSaveProject;
    m_wsProjectActionsHandler.m_btnSaveAsProject = ui.btnSaveAsProject;
}

CMetalFormingWidget::~CMetalFormingWidget()
{
    while (IMetalFormingStrategy *pPrevStrategy = ui.paintArea->popStrategy())
        delete pPrevStrategy;

    delete m_pPropertiesVariantManager;
    delete m_pPropertiesVariantFactory;
}

IMetalFormingStrategy * CMetalFormingWidget::createAndInitStrategy( CStrategies strategy )
{
    IMetalFormingStrategy *pStrategy = 0;
    switch(strategy)
    {
    case Strategy_CoordsMon:
        pStrategy = static_cast<IMetalFormingStrategy *>(
            new CCoordinatesMonitor(m_wsBase, ui.paintArea, m_wsCoordsMon));
        break;

    case Strategy_GridPainter:
        pStrategy = static_cast<IMetalFormingStrategy *>(new CGridPainter(
            m_wsBase, ui.paintArea));
        break;

    case Strategy_InstrsPainter:
        {
            CInstrumentsPainter *pIP = new CInstrumentsPainter(m_wsBase,
                ui.paintArea, m_wsInstrumentsPainter);

            connect(pIP, SIGNAL(multiplySelection(const QStringList&, int&)),
                this, SLOT(onMultiplySelection(const QStringList&, int&)));
            connect(pIP, SIGNAL(dataReady()), this, SLOT(onInstrumentsReady()));

            pStrategy = static_cast<IMetalFormingStrategy *>(pIP);
        }
        break;

    case Strategy_ProjActionsHandler:
        {
            CProjectsActionsHandler *pPAH = new CProjectsActionsHandler(
                m_wsBase, ui.paintArea, m_wsProjectActionsHandler);
            connect(pPAH, SIGNAL(saveProject(const QString &)),
                this, SLOT(onSaveProject(const QString &)));
            connect(pPAH, SIGNAL(loadProject(const QString &)),
                this, SLOT(onLoadProject(const QString &)));
            connect(pPAH, SIGNAL(newProject()),
                this, SLOT(onNewProject()));

            pStrategy = static_cast<IMetalFormingStrategy *>(pPAH);
        }
        break;

    case Strategy_PropsSetter:
        {
            CPropertiesSetter *pPS = new CPropertiesSetter(m_wsBase,
                ui.paintArea, m_wsPropertiesSetter);
            connect(pPS, SIGNAL(dataReady()), this,
                SLOT(onWPPropertiesDataReady()));

            pStrategy = static_cast<IMetalFormingStrategy *>(pPS);
        }
        break;

    case Strategy_ScaleSetter:
        {
            CScaleSetter *pSS = new CScaleSetter(m_wsBase, ui.paintArea,
                m_wsScaleSetter);
            connect(this, SIGNAL(projectLoaded(const QString &)),
                pSS, SLOT(onProjectLoaded()));
            connect(this, SIGNAL(pageFormatChanged(Data::CPageFormat)),
                pSS, SLOT(onPageFormatChanged(Data::CPageFormat)));

            pStrategy = static_cast<IMetalFormingStrategy *>(pSS);
        }
        break;

    case Strategy_WPPainter:
        {
            CWorkpiecePainter *pWPP = new CWorkpiecePainter(m_wsBase,
                ui.paintArea, m_wsWPPainter);
            connect(pWPP,
                SIGNAL(multiplySelection(const QStringList&, int&)), this,
                SLOT(onMultiplySelection(const QStringList&, int&)));
            connect(pWPP, SIGNAL(dataReady()), this,
                SLOT(onWPLinesReady()));

            pStrategy = static_cast<IMetalFormingStrategy *>(pWPP);
        }
        break;

    case Strategy_ZonesPainter:
        {
            CZonesPainter *pZP = new CZonesPainter(m_wsBase, ui.paintArea,
                m_wsZonesPainter);
            connect(pZP, SIGNAL(dataReady()), this, SLOT(onWPZonesReady()));

            pStrategy = static_cast<IMetalFormingStrategy *>(pZP);
        }
        break;

    default:
        assert("Wrong strategy identifier");
        break;
    }
    return pStrategy;
}

void CMetalFormingWidget::initToolbarForStrategy(CStrategies strategy)
{
    switch(strategy)
    {
    case Strategy_InstrsPainter:
        ui.btnCompileData->setEnabled(true);
        ui.btnClearWorkspace->setEnabled(true);
        ui.btnChangeType->setEnabled(false);
        ui.btnRestoreLastDeleted->setEnabled(true);
        ui.btnDeleteSelected->setEnabled(true);
        ui.btnDeleteLastCreated->setEnabled(true);
        ui.btnClearDeletionHistory->setEnabled(true);
        break;

    case Strategy_PropsSetter:
        ui.btnCompileData->setEnabled(true);
        ui.btnClearWorkspace->setEnabled(false);
        ui.btnChangeType->setEnabled(false);
        ui.btnRestoreLastDeleted->setEnabled(false);
        ui.btnDeleteSelected->setEnabled(false);
        ui.btnDeleteLastCreated->setEnabled(false);
        ui.btnClearDeletionHistory->setEnabled(false);
        break;

    case Strategy_WPPainter:
        ui.btnCompileData->setEnabled(true);
        ui.btnClearWorkspace->setEnabled(true);
        ui.btnChangeType->setEnabled(true);
        ui.btnRestoreLastDeleted->setEnabled(true);
        ui.btnDeleteSelected->setEnabled(true);
        ui.btnDeleteLastCreated->setEnabled(true);
        ui.btnClearDeletionHistory->setEnabled(true);
        break;

    case Strategy_ZonesPainter:
        ui.btnCompileData->setEnabled(true);
        ui.btnClearWorkspace->setEnabled(true);
        ui.btnChangeType->setEnabled(true);
        ui.btnRestoreLastDeleted->setEnabled(true);
        ui.btnDeleteSelected->setEnabled(true);
        ui.btnDeleteLastCreated->setEnabled(true);
        ui.btnClearDeletionHistory->setEnabled(true);
        break;

    default:
        assert("Wrong strategy identifier");
        break;
    }
}

void CMetalFormingWidget::init(const QString &sStartupProjectFilePath
                               /*= QString()*/)
{
     ui.paintArea->setMinimumSize(QSize(1050, 1485));
     ui.paintArea->setMaximumSize(QSize(1050, 1485));

    ui.paintArea->pushStrategy(createAndInitStrategy(Strategy_GridPainter));
    ui.paintArea->pushStrategy(createAndInitStrategy(Strategy_CoordsMon));
    ui.paintArea->pushStrategy(createAndInitStrategy(Strategy_ScaleSetter));
    ui.paintArea->pushStrategy(createAndInitStrategy(
        Strategy_ProjActionsHandler));


    pushPropertiesSetterStrategy();

    ui.paintArea->setFocus();

    if (!sStartupProjectFilePath.isEmpty())
        onLoadProject(sStartupProjectFilePath);
    else
        onNewProject();
}

void CMetalFormingWidget::onSetPropertiesClicked()
{
    if (IMetalFormingStrategy *pPrevStrategy =
        ui.paintArea->popStrategy())
        delete pPrevStrategy;

    pushPropertiesSetterStrategy();
}

void CMetalFormingWidget::onDrawWorkpiceClicked()
{
    updateStrategySelectorsButtons(EditState_WPLines);

    if (IMetalFormingStrategy *pPrevStrategy =
        ui.paintArea->popStrategy())
        delete pPrevStrategy;

    initToolbarForStrategy(Strategy_WPPainter);
    ui.paintArea->pushStrategy(createAndInitStrategy(Strategy_WPPainter));
    ui.paintArea->update();
    ui.paintArea->setFocus();
}

void CMetalFormingWidget::onDrawZonesClicked()
{
    updateStrategySelectorsButtons(EditState_WPZones);

    if (IMetalFormingStrategy *pPrevStrategy =
        ui.paintArea->popStrategy())
        delete pPrevStrategy;

    
    initToolbarForStrategy(Strategy_ZonesPainter);
    ui.paintArea->pushStrategy(createAndInitStrategy(Strategy_ZonesPainter));
    ui.paintArea->update();
    ui.paintArea->setFocus();
}


void CMetalFormingWidget::onDrawInstrumentsClicked()
{
    updateStrategySelectorsButtons(EditState_WPInstrs);

    if (IMetalFormingStrategy *pPrevStrategy =
        ui.paintArea->popStrategy())
        delete pPrevStrategy;

    initToolbarForStrategy(Strategy_InstrsPainter);
    ui.paintArea->pushStrategy(createAndInitStrategy(Strategy_InstrsPainter));
    ui.paintArea->update();
    ui.paintArea->setFocus();
}

void CMetalFormingWidget::onMultiplySelection( const QStringList &labels, int &res )
{
    CMultiplySelectionDialog dialog(this);
    dialog.initItems(labels);
    res = -1;
    dialog.exec();
    if (dialog.result()) res = dialog.getSelItem();
    ui.paintArea->setFocus();
}

void CMetalFormingWidget::onWPPropertiesDataReady()
{
    processWPPropsCompiledData();
    QMessageBox::information(this, tr("Properties compile success"),
        tr("Workpiece properties have been compiled successfully."));

    applyCurrentPageFormat();
}

void CMetalFormingWidget::onWPLinesReady()
{
    processWPLinesCompiledData();
    QMessageBox::information(this, tr("Lines compile success"),
        tr("Workpiece lines have been compiled successfully."));
}

void CMetalFormingWidget::onWPZonesReady()
{
    processWPZonesCompiledData();
    QMessageBox::information(this, tr("Zones compile success"),
        tr("Workpiece zones have been compiled successfully."));
}

void CMetalFormingWidget::onInstrumentsReady()
{
    processWPInstrumentsCompiledData();
    QMessageBox::information(this, tr("Instruments compile success"),
        tr("Workpiece instruments have been compiled successfully."));
}

void CMetalFormingWidget::pushPropertiesSetterStrategy()
{
    updateStrategySelectorsButtons(EditState_WPProps);

    initToolbarForStrategy(Strategy_PropsSetter);
    ui.paintArea->pushStrategy(createAndInitStrategy(Strategy_PropsSetter));
    ui.paintArea->update();
    ui.paintArea->setFocus();
}

bool CMetalFormingWidget::onSaveProject( const QString &sFileName )
{   
    QFile file(sFileName);
    if (!file.open(QIODevice::WriteOnly))
        return false;

    QDataStream out(&file);

    // 1) Write MFProjectSignature and version
    //
    out << MFProject::g_iProjectSignature;
    out << MFProject::g_iSupportedProjectVersion;

    // 2) Write PropertiesSetterWorkingSet
    //
    out << m_wsPropertiesSetter.m_ptZero;
    out << m_wsPropertiesSetter.m_wpProperties;

    // 3) Write WorkpiecePainterWorkingSet
    out << m_wsWPPainter.m_workpieceLines;

    // 4) Write ZonesPainterWorkingSet
    out << m_wsZonesPainter.m_workpiecePoints;
    out << m_wsZonesPainter.m_workpieceLines;
    out << m_wsZonesPainter.m_wpProperties;
    out << m_wsZonesPainter.m_workpieceGridCompactionLines;
    out << m_wsZonesPainter.m_workpieceZones;

    // 5) Write InstrumentsPainterWorkingSet
    //
    out << m_wsInstrumentsPainter.m_workpieceMainLines;
    out << m_wsInstrumentsPainter.m_wpProperties;
    out << m_wsInstrumentsPainter.m_instruments;

    file.close();

    // 6) Save current project file path.
    m_wsProjectActionsHandler.m_sCurrentProjectFilePath = sFileName;

    // 7) Emit projectSaved signal to parent window and user actions handlers
    //    can window handle it.
    emit projectSaved(sFileName);
    return true;
}

bool CMetalFormingWidget::onLoadProject( const QString &sFileName )
{
    QFile file(sFileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QDataStream in(&file);

    // 1) Check the file format signature (is it the MFProject File?)
    //
    int iFileSignature = 0;
    in >> iFileSignature;
    if (iFileSignature != MFProject::g_iProjectSignature)
    {
        file.close();
        return false;
    }

    // 2) Check the project file version(is this version supported?)
    //
    int iFileVersion = 0;
    in >> iFileVersion;
    if (iFileVersion != MFProject::g_iSupportedProjectVersion)
    {
        file.close();
        return false;
    }

    // 3) Check is this all the file or not?
    //
    if(file.atEnd())
    {
        file.close();
        return false;
    }

    // 4) Read PropertiesSetterWorkingSet
    //
    in >> m_wsPropertiesSetter.m_ptZero;
    in >> m_wsPropertiesSetter.m_wpProperties;

    // 5) Read WorkpiecePainterWorkingSet
    in >> m_wsWPPainter.m_workpieceLines;

    // 6) Read ZonesPainterWorkingSet
    in >> m_wsZonesPainter.m_workpiecePoints;
    in >> m_wsZonesPainter.m_workpieceLines;
    in >> m_wsZonesPainter.m_wpProperties;
    in >> m_wsZonesPainter.m_workpieceGridCompactionLines;
    in >> m_wsZonesPainter.m_workpieceZones;

    // 7) Read InstrumentsPainterWorkingSet
    //
    in >> m_wsInstrumentsPainter.m_workpieceMainLines;
    in >> m_wsInstrumentsPainter.m_wpProperties;
    in >> m_wsInstrumentsPainter.m_instruments;

    file.close();

    // 8) Save current project file path.
    m_wsProjectActionsHandler.m_sCurrentProjectFilePath = sFileName;

    // 9) Set the Properties Editor strategy for reload project into view.
    onSetPropertiesClicked();

    // 10) Apply new page format
    applyCurrentPageFormat();

    // 11) Emit projectLoaded signal to parent window and user actions handlers
    //     can window handle it.
    emit projectLoaded(sFileName);

    return true;
}

void CMetalFormingWidget::onNewProject()
{
    // 1) Clear PropertiesSetterWorkingSet
    //
    m_wsPropertiesSetter.m_ptZero.setX(0);
    m_wsPropertiesSetter.m_ptZero.setY(0);
    m_wsPropertiesSetter.m_wpProperties.clearAllProperties();

    // 2) Clear WorkpiecePainterWorkingSet
    m_wsWPPainter.m_workpieceLines.clear();

    // 3) Clear ZonesPainterWorkingSet
    m_wsZonesPainter.m_workpiecePoints.clear();
    m_wsZonesPainter.m_workpieceLines.clear();
    m_wsZonesPainter.m_wpProperties.clearAllProperties();
    m_wsZonesPainter.m_workpieceGridCompactionLines.clear();
    m_wsZonesPainter.m_workpieceZones.clear();

    // 4) Read InstrumentsPainterWorkingSet
    //
    m_wsInstrumentsPainter.m_workpieceMainLines.clear();
    m_wsInstrumentsPainter.m_wpProperties.clearAllProperties();
    m_wsInstrumentsPainter.m_instruments.clear();

    // 5) Save current project file path.
    m_wsProjectActionsHandler.m_sCurrentProjectFilePath.clear();

    // 6) Set the Properties Editor strategy for reload project into view.
    onSetPropertiesClicked();

    // 7) Apply new page format
    applyCurrentPageFormat();

    // 8) Emit projectLoaded signal to parent window and user actions handlers
    //    can window handle it.
    emit projectLoaded(QString());
}

void CMetalFormingWidget::applyCurrentPageFormat()
{
    QVariant vPageFormat;
    m_wsPropertiesSetter.m_wpProperties.getWPPropertyValue(
        Data::CWPProperties::WPProp_PageFormat, vPageFormat);
    emit pageFormatChanged(Data::CPageFormat(vPageFormat.toInt()));
}

void CMetalFormingWidget::onBuildProjectClicked()
{
    buildProject();
}

void CMetalFormingWidget::buildProject()
{
    // Starting project building...
    bool bStepComplileResult = true;

    // 1) Compile workpiece properties.
    //
    processWPPropsCompiledData();

    // 2) Compile workpiece lines.
    //
    CWorkpiecePainter *pWPP = new CWorkpiecePainter(m_wsBase,
        ui.paintArea, m_wsWPPainter);
    bStepComplileResult = pWPP->compile();
    delete pWPP;

    if (bStepComplileResult)
        processWPLinesCompiledData();
    else
    {
        QMessageBox::critical(this, tr("Build error"),
            tr("Error during workpiece lines compilation."));
        return;
    }

    // 3) Compile workpiece zones.
    //
    CZonesPainter *pZP = new CZonesPainter(m_wsBase, ui.paintArea,
        m_wsZonesPainter);
    bStepComplileResult = pZP->compile();
    delete pZP;

    if (bStepComplileResult)
        processWPZonesCompiledData();
    else
    {
        QMessageBox::critical(this, tr("Build error"),
            tr("Error during workpiece zones compilation."));
        return;
    }

    // 4) Compile instruments.
    //
    CInstrumentsPainter *pIP = new CInstrumentsPainter(m_wsBase,
        ui.paintArea, m_wsInstrumentsPainter);
    bStepComplileResult = pIP->compile();
    delete pIP;

    if (bStepComplileResult)
        processWPInstrumentsCompiledData();
    else
    {
        QMessageBox::critical(this, tr("Build error"),
            tr("Error during workpiece instruments compilation."));
        return;
    }

    // If we are here all building steps complited successfully. Now write
    // output file as the result of project builing.
    QString sOutFilePath(m_wsProjectActionsHandler.m_sCurrentProjectFilePath);
    if (sOutFilePath.isEmpty())
        sOutFilePath = tr("New Project") + MFProject::g_sProjectExtension;

    // NOTE: We need to change ONLY extension!
    //
    sOutFilePath.replace(sOutFilePath.length() -
        MFProject::g_sProjectExtension.length(),
        MFProject::g_sProjectExtension.length(),
        MFProject::g_sProjectOutputExtension);

    QFile fout(sOutFilePath);
    fout.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ss(&fout);
    ss << m_wsZonesPainter.m_sOutput << m_wsInstrumentsPainter.m_sOutput;
    ss.flush();
    fout.close();

    QMessageBox::information(this, tr("Build success"),
        tr("Project has been succesfully builded.\nOutput file: ") +
        sOutFilePath);
}

void CMetalFormingWidget::processWPPropsCompiledData()
{
    m_wsZonesPainter.m_wpProperties = m_wsPropertiesSetter.m_wpProperties;
    m_wsInstrumentsPainter.m_wpProperties = m_wsPropertiesSetter.m_wpProperties;
}

void CMetalFormingWidget::processWPLinesCompiledData()
{
    m_wsZonesPainter.m_workpiecePoints = m_wsWPPainter.m_workpiecePoints;

    // Now divide workpice lines into two vectors:
    // vector of (Main & Aux) and vector of (GridCompact).
    // Also we are pushing main WPLines into instrument drawning working set.
    //
    m_wsZonesPainter.m_workpieceLines.clear();
    m_wsZonesPainter.m_workpieceGridCompactionLines.clear();
    m_wsInstrumentsPainter.m_workpieceMainLines.clear();

    foreach(Data::CWPLine wpLine, m_wsWPPainter.m_workpieceLines)
    {
        if (wpLine.getType() == Data::CWPLine::WPLine_Main)
            m_wsInstrumentsPainter.m_workpieceMainLines.push_back(wpLine);

        Data::CWPLinesVector *pVector = &m_wsZonesPainter.m_workpieceLines;
        if (wpLine.getType() == Data::CWPLine::WPLine_GridCompact)
            pVector = &m_wsZonesPainter.m_workpieceGridCompactionLines;


        pVector->push_back(wpLine);
    }
}

void CMetalFormingWidget::processWPZonesCompiledData()
{
    // NOTE: nothing yet...
}

void CMetalFormingWidget::processWPInstrumentsCompiledData()
{
    // NOTE: nothing yet...
}

void CMetalFormingWidget::updateStrategySelectorsButtons( CEditStates es )
{
    QFont fntSetProps(ui.btnSetProperties->font());
    QFont fntDrawWPLines(ui.btnDrawWorkpiece->font());
    QFont fntDrawZones(ui.btnDrawZones->font());
    QFont fntDrawInstrs(ui.btnDrawInstruments->font());

    fntSetProps.setUnderline(es == EditState_WPProps);
    fntSetProps.setBold(es == EditState_WPProps);
    fntDrawWPLines.setUnderline(es == EditState_WPLines);
    fntDrawWPLines.setBold(es == EditState_WPLines);
    fntDrawZones.setUnderline(es == EditState_WPZones);
    fntDrawZones.setBold(es == EditState_WPZones);
    fntDrawInstrs.setUnderline(es == EditState_WPInstrs);
    fntDrawInstrs.setBold(es == EditState_WPInstrs);

    ui.btnSetProperties->setFont(fntSetProps);
    ui.btnDrawWorkpiece->setFont(fntDrawWPLines);
    ui.btnDrawZones->setFont(fntDrawZones);
    ui.btnDrawInstruments->setFont(fntDrawInstrs);
}

void CMetalFormingWidget::showAboutDialog()
{
    CAboutDialog dlg;
    dlg.exec();
}
