#include "InstrumentsPainter.h"
#include "../Tools/Tools.h"
#include "../Data/WPInstrument/WPInstrumentStringConsts.h"
#include "../OutputFormatter/OutputFormatter.h"

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <Windows.h>
#include <algorithm>
#include <functional>

#include <QtSolutions/qtpropertymanager.h>
#include <QtSolutions/qttreepropertybrowser>


// Consts section...
//
double g_fWPInstrumentMarkerWidth = 10.0f;
double g_fWPInstrumentMarkerHeight = 10.0f;
unsigned int g_nWPInstrumentEndsMarkerRadius = 3;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
CInstrumentsPainter::CInstrumentsPainter(
    CBaseWorkingSet &baseWorkingSet, QWidget *pWorkAreaWidget,
    CInstrumentsPainterWorkingSet &workingSet):
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget), m_workingSet(workingSet)
{

}

void CInstrumentsPainter::init()
{
    m_ptMoved.setX(0);
    m_ptMoved.setY(0);
    m_ptPressed.setX(0);
    m_ptPressed.setY(0);
    m_bDrawningNextInstrument = false;

    m_workingSet.m_gbInfo->setVisible(true);
    updateSelInstrumentInfo();

    m_deletedInstruments.clear();

    m_workingSet.m_propertiesManager->clear();

    // Generating properties list.
    //
    createPropertiesList();
    connect(m_workingSet.m_propertiesManager,
        SIGNAL(valueChanged (QtProperty *, const QVariant &)),
        this, SLOT(onPropertyValueChanged (QtProperty *, const QVariant &)));

    m_workingSet.m_propertiesEditor->clear();

    // Connect toolbar buttons signals
    //
    connect(m_workingSet.m_btnCompileData, SIGNAL(clicked()), this,
        SLOT(onDataReady()));
    connect(m_workingSet.m_btnClearWorkspace, SIGNAL(clicked()), this,
        SLOT(onClearWorkspace()));
    connect(m_workingSet.m_btnRestoreLastDeleted, SIGNAL(clicked()), this,
        SLOT(onRestoreLastDeleted()));
    connect(m_workingSet.m_btnDeleteSelected, SIGNAL(clicked()), this,
        SLOT(onDeleteSelected()));
    connect(m_workingSet.m_btnDeleteLastCreated, SIGNAL(clicked()), this,
        SLOT(onDeleteLastCreated()));
    connect(m_workingSet.m_btnClearDeletionHistory, SIGNAL(clicked()), this,
        SLOT(onClearDeletionHistory()));

    m_workingSet.m_propertiesEditor->setVisible(true);
}

void CInstrumentsPainter::deinit()
{
    // Disconnect all signals/slots.
    disconnect();

    m_workingSet.m_gbInfo->setVisible(false);

    m_workingSet.m_propertiesEditor->clear();
    m_workingSet.m_propertiesManager->clear();
    m_workingSet.m_propertiesEditor->setVisible(false);
}

bool CInstrumentsPainter::compile()
{
    return generateOutputString(false);
}

bool CInstrumentsPainter::mousePressEventHandler(QWidget *pWidget,
                                                 QMouseEvent *pEvent )
{
    if (pEvent->buttons().testFlag(Qt::RightButton)) return true;
    m_ptPressed = pEvent->pos();
    m_ptMoved = m_ptPressed;
    m_baseWorkingSet.m_grid.bindScreenPointToGrid(m_ptPressed);
    m_baseWorkingSet.m_grid.bindScreenPointToGrid(m_ptMoved);
    m_bDrawningNextInstrument = true;
    cleanSelection();
    pWidget->update();
    return true;
}

bool CInstrumentsPainter::mouseReleaseEventHandler(QWidget *pWidget,
                                                   QMouseEvent *pEvent )
{
    if (!m_bDrawningNextInstrument) return true;
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

        m_workingSet.m_instruments.push_back(Data::CWPInstrument(
            QLineF(ptLineFirst, ptLineSecond),
            Data::CWPInstrument::WPInstrument_Default));
    }
    m_bDrawningNextInstrument = false;
    pWidget->update();
    return true;
}

bool CInstrumentsPainter::mouseMoveEventHandler(QWidget *pWidget,
                                                QMouseEvent * pEvent )
{
    if (m_bDrawningNextInstrument)
    {
        if (pEvent->buttons().testFlag(Qt::RightButton))
        {
            m_bDrawningNextInstrument = false;
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
        double fAngle = currentLine.getYAsisAngle();


        m_workingSet.m_labelInstrumentAngle->setText(QString("%1(%2)").
            arg(fAngle, 0, 'f', 2).
            arg(360.0f - fAngle, 0, 'f', 2));
        pWidget->update();
    }
    return true;
}

bool CInstrumentsPainter::paintEventHandler( QWidget *pWidget,
                                            QPaintEvent *pEvent )
{
    QPainter painter(pWidget);
    drawWorkpice(painter);
    drawFinalInstruments(painter);
    if (m_bDrawningNextInstrument)
        drawCurrentInstrument(painter);
    drawSelection(painter);
    return true;
}

bool CInstrumentsPainter::mouseClickEventHandler( QWidget *pWidget,
                                                 QMouseEvent *pEvent )
{
    if (m_bDrawningNextInstrument) return false;
    Data::CWPInstrumentsVector::iterator it =
        m_workingSet.m_instruments.begin();
    Data::CWPInstrumentsVector::iterator itLast =
        m_workingSet.m_instruments.end();

    Data::CWPInstrumentsVector foundedInstruments;

    for (; it != itLast; ++it)
    {
        QRectF rcInctrumentMarker;
        getWPInstrumentMarkerRect(*it, rcInctrumentMarker);

        if (rcInctrumentMarker.contains(QPointF(pEvent->pos())))
            foundedInstruments.push_back(*it);
    }

    if (foundedInstruments.empty())
    {
        cleanSelection();
        return true;
    }
    if (foundedInstruments.size() == 1)
    {
        setSelection(*foundedInstruments.begin());
    }
    else
    {
        QStringList labels;
        it = foundedInstruments.begin();
        itLast = foundedInstruments.end();
        for (; it != itLast; ++it)
            labels.push_back(it->getStringLong());

        int selRes = -1;
        emit multiplySelection(labels, selRes);
        if (selRes == -1)
        {
            cleanSelection();
            return true;
        }

        setSelection(foundedInstruments[selRes]);
    }

    updateSelInstrumentInfo();
    return true;
}

bool CInstrumentsPainter::keyPressEventHandler( QWidget *pWidget,
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
        // Delete selected instrument
        onDeleteSelected();
        break;

    case Qt::Key_F2:
        // Prepare final data
        onDataReady();
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
            // Delete previos instrument
            onDeleteLastCreated();
            break;

        case Qt::Key_R:
            // Restore last deleted instrument
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


void CInstrumentsPainter::drawWorkpice( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen workpieceLinesPen(Qt::blue, 2, Qt::DashLine);
    QBrush workpieceLinesBrush(Qt::blue);
    painter.setPen(workpieceLinesPen);
    painter.setBrush(workpieceLinesBrush);

    foreach(Data::CWPLine line, m_workingSet.m_workpieceMainLines)
    {
        QPoint ptLineFirst;
        QPoint ptLineSecond;
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            line.getLine().p1(), ptLineFirst);
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            line.getLine().p2(), ptLineSecond);

        painter.drawLine(ptLineFirst, ptLineSecond);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CInstrumentsPainter::drawFinalInstruments( QPainter &painter )
{
    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen defInstrumentsPen(Qt::blue, 2);
    QBrush defInstrumentsBrush(Qt::blue);
    QPen dlInstrumentsPen(Qt::darkGreen, 2);
    QBrush dlInstrumentsBrush(Qt::darkGreen);
    QPen fcInstrumentsPen(Qt::magenta, 2);
    QBrush fcInstrumentsBrush(Qt::magenta);
    QPen ssInstrumentsPen(QColor(128, 128, 128), 2);
    QBrush ssInstrumentsBrush(QColor(128, 128, 128));

    Data::CWPInstrumentsVector::iterator it =
        m_workingSet.m_instruments.begin();
    Data::CWPInstrumentsVector::iterator itLast =
        m_workingSet.m_instruments.end();

    for (; it != itLast; ++it)
    {
        QPoint ptInstrumentFirst;
        QPoint ptInstrumentSecond;
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            it->getLine().p1(), ptInstrumentFirst);
        m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
            it->getLine().p2(), ptInstrumentSecond);

        QPen *pLinePen = 0;
        QBrush *pLineBrush = 0;

        switch (it->getType())
        {
        case Data::CWPInstrument::WPInstrument_Default:
            pLinePen = &defInstrumentsPen;
            pLineBrush = &defInstrumentsBrush;
            break;
        case Data::CWPInstrument::WPInstrument_DisturbedLoad:
            pLinePen = &dlInstrumentsPen;
            pLineBrush = &dlInstrumentsBrush;
            break;
        case Data::CWPInstrument::WPInstrument_FrictionConsidering:
            pLinePen = &fcInstrumentsPen;
            pLineBrush = &fcInstrumentsBrush;
            break;
        case Data::CWPInstrument::WPInstrument_StressStrain:
            pLinePen = &ssInstrumentsPen;
            pLineBrush = &ssInstrumentsBrush;
            break;
        }
        painter.setPen(*pLinePen);
        painter.setBrush(*pLineBrush);

        painter.drawEllipse(ptInstrumentFirst, g_nWPInstrumentEndsMarkerRadius,
            g_nWPInstrumentEndsMarkerRadius);
        painter.drawEllipse(ptInstrumentSecond, g_nWPInstrumentEndsMarkerRadius,
            g_nWPInstrumentEndsMarkerRadius);
        painter.drawLine(ptInstrumentFirst, ptInstrumentSecond);
        QRectF rcInstrumentMarker;
        getWPInstrumentMarkerRect(*it, rcInstrumentMarker);
        painter.drawRect(rcInstrumentMarker);
    }

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CInstrumentsPainter::drawCurrentInstrument( QPainter &painter )
{
    if (m_ptPressed == m_ptMoved)
        return;

    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen currentInstrumentPen(Qt::red, 2);
    QBrush currentInstrumentBrush(Qt::red, Qt::SolidPattern);

    painter.setPen(currentInstrumentPen);
    painter.setBrush(currentInstrumentBrush);

    QRectF rcInstrumentMarker;
    getWPInstrumentMarkerRect(m_ptPressed, m_ptMoved, rcInstrumentMarker);

    painter.drawEllipse(m_ptPressed, g_nWPInstrumentEndsMarkerRadius,
        g_nWPInstrumentEndsMarkerRadius);
    painter.drawEllipse(m_ptMoved, g_nWPInstrumentEndsMarkerRadius,
        g_nWPInstrumentEndsMarkerRadius);
    painter.drawLine(m_ptPressed, m_ptMoved);
    painter.drawRect(rcInstrumentMarker);

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CInstrumentsPainter::drawSelection( QPainter &painter )
{
    if (!selectionAvaliable())
        return;

    QPen oldPen(painter.pen());
    QBrush oldBrush(painter.brush());

    QPen selectedInstrumentPen(Qt::yellow, 2);
    QBrush selectedInstrumentBrush(Qt::darkYellow, Qt::SolidPattern);

    painter.setPen(selectedInstrumentPen);
    painter.setBrush(selectedInstrumentBrush);

    QPoint ptInstrumentFirst;
    QPoint ptInstrumentSecond;
    m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
        m_selectedInstrument.getLine().p1(), ptInstrumentFirst);
    m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
        m_selectedInstrument.getLine().p2(), ptInstrumentSecond);

    painter.drawEllipse(ptInstrumentFirst, g_nWPInstrumentEndsMarkerRadius,
        g_nWPInstrumentEndsMarkerRadius);
    painter.drawEllipse(ptInstrumentSecond, g_nWPInstrumentEndsMarkerRadius,
        g_nWPInstrumentEndsMarkerRadius);
    painter.drawLine(ptInstrumentFirst, ptInstrumentSecond);

    QRectF rcInstrumentMarker;
    getWPInstrumentMarkerRect(m_selectedInstrument, rcInstrumentMarker);
    painter.drawRect(rcInstrumentMarker);

    painter.setPen(oldPen);
    painter.setBrush(oldBrush);
}

void CInstrumentsPainter::getWPInstrumentMarkerRect(
    const QPoint &ptInstrumentMarker, QRectF &rect )
{
    rect.setX((double)ptInstrumentMarker.x() -
        g_fWPInstrumentMarkerWidth / 2.0f);
    rect.setY((double)ptInstrumentMarker.y() -
        g_fWPInstrumentMarkerHeight / 2.0f);
    rect.setWidth(g_fWPInstrumentMarkerWidth);
    rect.setHeight(g_fWPInstrumentMarkerHeight);
}

void CInstrumentsPainter::getWPInstrumentMarkerRect(
    const Data::CWPInstrument &instrument, QRectF &rect )
{
    QPoint ptInstrumentMarker;
    m_baseWorkingSet.m_grid.transformRealPointToScreenPoint(
        instrument.getCenterPoint(), ptInstrumentMarker);

    getWPInstrumentMarkerRect(ptInstrumentMarker, rect);
}

void CInstrumentsPainter::getWPInstrumentMarkerRect(
    const QPoint &ptFirst, const QPoint &ptSecond, QRectF &rect )
{
    QPoint ptInstrumentMarker =
        QLineF(ptFirst, ptSecond).pointAt(0.5).toPoint();

    getWPInstrumentMarkerRect(ptInstrumentMarker, rect);
}

void CInstrumentsPainter::updateSelInstrumentInfo()
{
    m_workingSet.m_labelSelInstrument->setText(
        m_selectedInstrument.getStringShort());
}

void CInstrumentsPainter::cleanSelection()
{
    setSelection(Data::CWPInstrument());
}

void CInstrumentsPainter::setSelection( const Data::CWPInstrument &instrument )
{
    // Write changes via selected instrument into instruments repository
    //
    if (selectionAvaliable())
    {
        Data::CWPInstrumentsVector::iterator it = std::find(
            m_workingSet.m_instruments.begin(),
            m_workingSet.m_instruments.end(), m_selectedInstrument);
        if (it != m_workingSet.m_instruments.end())
            (*it) = m_selectedInstrument;
    }
    m_selectedInstrument = instrument;
    initSelInstrumentProperties();
    updateSelInstrumentInfo();
}

bool CInstrumentsPainter::selectionAvaliable()
{
    return (!m_selectedInstrument.isNull());
}

void CInstrumentsPainter::createPropertiesList()
{
    QtVariantProperty *pProperty = 0;

    // 1) Add instrument type property.
    //
    pProperty = m_workingSet.m_propertiesManager->addProperty(
        QtVariantPropertyManager::enumTypeId(),
        Data::CWPInstrumentStringConsts::instance().g_propertiesNamesList[
            Data::CWPInstrument::WPInstrProp_Type]);

    QStringList enumNames;
    for(int i = Data::CWPInstrument::WPInstrument_Default; i <
        Data::CWPInstrument::WPInstrumentsTypesCount; ++i)
        enumNames << Data::CWPInstrumentStringConsts::instance().g_instrumentTypesNamesList[i];

    pProperty->setAttribute(QLatin1String("enumNames"), enumNames);
    pProperty->setValue(0);

    m_workingSet.m_propertiesEditor->addProperty(pProperty);
    m_propertiesMap.insert(pProperty, Data::CWPInstrument::WPInstrProp_Type);
    m_inversePropertiesMap.insert(Data::CWPInstrument::WPInstrProp_Type,
        pProperty);

    // 2) Add workiece point movement property;
    // 3) Add instrument movement X property;
    // 4) Add instrument movement Y property;
    // 5) Add distrurbed load property;
    // 6) Add friction property.
    //
    for (int i = Data::CWPInstrument::WPInstrProp_WPPointsMovement;
        i < Data::CWPInstrument::WPInstrPropsCount; ++i)
    {
        pProperty = m_workingSet.m_propertiesManager->addProperty(
            QVariant::Double,
            Data::CWPInstrumentStringConsts::instance().g_propertiesNamesList[i]
        );
        pProperty->setValue(0.0f);
        pProperty->setAttribute(QLatin1String("decimals"), 6);

        m_workingSet.m_propertiesEditor->addProperty(pProperty);
        m_propertiesMap.insert(pProperty,
            Data::CWPInstrument::CWPInstrumentProperties(i));
        m_inversePropertiesMap.insert(
            Data::CWPInstrument::CWPInstrumentProperties(i), pProperty);
    }
}

void CInstrumentsPainter::onPropertyValueChanged( QtProperty *pProperty,
                                                 const QVariant &value )
{
    QtVariantProperty *pVarProperty =
        dynamic_cast<QtVariantProperty *>(pProperty);
    if (!pVarProperty)
        return;

    CPropertiesMap::iterator I = m_propertiesMap.find(pVarProperty);
    m_selectedInstrument.setInstrumentPropertyValue(I.value(),
        I.key()->value());
    if (I.value() == Data::CWPInstrument::WPInstrProp_Type)
    {
        updateAvaliableProperties();
        updateSelInstrumentInfo();
    }
}

void CInstrumentsPainter::initSelInstrumentProperties()
{
    m_workingSet.m_propertiesEditor->clear();

    if (!selectionAvaliable())
        return;

    // Add all properties to the editor.
    // NOTE: We need to save properties order, so use keys sequence.
    //
    for (int i = Data::CWPInstrument::WPInstrProp_Type;
        i < Data::CWPInstrument::WPInstrPropsCount; ++i)
    {
        QtVariantProperty *pProperty = m_inversePropertiesMap[
            Data::CWPInstrument::CWPInstrumentProperties(i)];
            QVariant propValue;

            m_workingSet.m_propertiesEditor->addProperty(pProperty);
            m_selectedInstrument.getInstrumentPropertyValue(
                Data::CWPInstrument::CWPInstrumentProperties(i), propValue);

            pProperty->setValue(propValue);
            pProperty->setEnabled(
                m_selectedInstrument.isPropertyAvaliableForCurrentType(
                Data::CWPInstrument::CWPInstrumentProperties(i)));
    }
}

void CInstrumentsPainter::updateAvaliableProperties()
{
    CPropertiesMap::iterator I = m_propertiesMap.begin();
    CPropertiesMap::iterator E = m_propertiesMap.end();
    for (; I != E; ++I)
    {
        I.key()->setEnabled(
            m_selectedInstrument.isPropertyAvaliableForCurrentType(I.value()));
    }
}

bool CInstrumentsPainter::generateOutputString( bool bEmitSignals /*= true*/ )
{
    // 1) Divide instruments by types
    //
    Data::CWPInstrumentsVector defaultInstrumentsHV;
    Data::CWPInstrumentsVector defaultInstrumentsC;
    Data::CWPInstrumentsVector disturbedLoadInstruments;
    Data::CWPInstrumentsVector frictionConsideringInstruments;
    Data::CWPInstrumentsVector stressStrainInstruments;
    foreach (Data::CWPInstrument instrument, m_workingSet.m_instruments)
    {
        Data::CWPInstrumentsVector *pVectorToPush = 0;
        switch(instrument.getType())
        {
        case Data::CWPInstrument::WPInstrument_Default:
            pVectorToPush = &defaultInstrumentsHV;
            if (instrument.isCanonical())
                pVectorToPush = &defaultInstrumentsC;
            break;
        case Data::CWPInstrument::WPInstrument_DisturbedLoad:
            pVectorToPush = &disturbedLoadInstruments;
            break;
        case Data::CWPInstrument::WPInstrument_FrictionConsidering:
            pVectorToPush = &frictionConsideringInstruments;
            break;
        case Data::CWPInstrument::WPInstrument_StressStrain:
            pVectorToPush = &stressStrainInstruments;
            break;
        }
        pVectorToPush->push_back(instrument);
    }

    // 2) Fill instruments parameters vectors.
    //
    Data::CDoublesVector movementsHV;
    Data::CDoublesVector movementsC;
    Data::CDoublesVector disturbedLoads;
    Data::CDoublesVector frictions;

    // 2.1) Process horisontal and vertical default instruments.
    //
    foreach(Data::CWPInstrument instrument, defaultInstrumentsHV)
    {
        movementsHV.push_back(instrument.getWorkpicePointsMovement());
        movementsHV.push_back(instrument.getInstrumentMovementX());
        movementsHV.push_back(instrument.getInstrumentMovementY());
    }

    // 2.2) Process canonical default instruments.
    //
    foreach(Data::CWPInstrument instrument, defaultInstrumentsC)
    {
        movementsHV.push_back(instrument.getInstrumentMovementX());
        movementsHV.push_back(instrument.getInstrumentMovementY());
        movementsC.push_back(instrument.getWorkpicePointsMovement());
    }

    // 2.3) Process disturbed load instruments.
    //
    foreach(Data::CWPInstrument instrument, disturbedLoadInstruments)
        disturbedLoads.push_back(instrument.getDisturbedLoad());

    // 2.4) Process friction considering instruments.
    //
    foreach(Data::CWPInstrument instrument, frictionConsideringInstruments)
    {
        frictions.push_back(instrument.getFriction());
        movementsHV.push_back(instrument.getInstrumentMovementX());
        movementsHV.push_back(instrument.getInstrumentMovementY());
    }

    // 2.5) Process stress strain instruments.
    //
    foreach(Data::CWPInstrument instrument, stressStrainInstruments)
    {
        movementsHV.push_back(instrument.getInstrumentMovementX());
        movementsHV.push_back(instrument.getInstrumentMovementY());
    }

    // 3) Remove duplicate values.
    //
    std::sort(movementsHV.begin(), movementsHV.end());
    movementsHV.erase(std::unique(movementsHV.begin(), movementsHV.end()),
        movementsHV.end());
    std::sort(movementsC.begin(), movementsC.end());
    movementsC.erase(std::unique(movementsC.begin(), movementsC.end()),
        movementsC.end());
    std::sort(disturbedLoads.begin(), disturbedLoads.end());
    disturbedLoads.erase(std::unique(disturbedLoads.begin(),
        disturbedLoads.end()), disturbedLoads.end());
    std::sort(frictions.begin(), frictions.end());
    frictions.erase(std::unique(frictions.begin(), frictions.end()),
        frictions.end());

    // 4) Write output string.
    //
    QString sOutput;
    QTextStream ss(&sOutput);

    // 4.1) Write total instruments information.
    //
    ss << COutputFormatter::formatValue(defaultInstrumentsHV.size(),
        OutputFormat_I5);
    ss << COutputFormatter::formatValue(movementsHV.size(), OutputFormat_I5);
    ss << COutputFormatter::formatValue(defaultInstrumentsC.size(),
        OutputFormat_I5);
    ss << COutputFormatter::formatValue(movementsC.size(), OutputFormat_I5);
    ss << COutputFormatter::formatValue(disturbedLoadInstruments.size(),
        OutputFormat_I5);
    ss << COutputFormatter::formatValue(disturbedLoads.size(), OutputFormat_I5);
    ss << COutputFormatter::formatValue(
        frictionConsideringInstruments.size(), OutputFormat_I5);
    ss << COutputFormatter::formatValue(frictions.size(), OutputFormat_I5);
    ss << COutputFormatter::formatValue(
        (m_workingSet.m_wpProperties.m_bCalcMetherialDuctility? 1 : 0),
        OutputFormat_I5);
    ss << COutputFormatter::formatValue(stressStrainInstruments.size(),
        OutputFormat_I5);
    
    ss << COutputFormatter::formatEndl();

    // 4.2) Write horisontal and vertical default instruments.
    //
    foreach(Data::CWPInstrument instrument, defaultInstrumentsHV)
    {
        QString sInstrumentOutput;
        instrument.getOutputString(movementsHV, movementsC,
            disturbedLoads, frictions, sInstrumentOutput);
        ss << sInstrumentOutput;
        ss << COutputFormatter::formatEndl();
    }

    // 4.3) Write all horisontal and vertical movements.
    //
    size_t nValuesCounter = 0;
    foreach(double fMovement, movementsHV)
    {
        ss << COutputFormatter::formatValue(fMovement, OutputFormat_F10_2);
        if (nValuesCounter++ ==
            CInstrumentsPainterWorkingSet::nOutputRealNumbersInLineCount)
        {
            nValuesCounter = 0;
            ss << COutputFormatter::formatEndl();
        }
    }
    if (nValuesCounter)
        ss << COutputFormatter::formatEndl();

    // 4.4) Write canonical default instruments.
    //
    foreach(Data::CWPInstrument instrument, defaultInstrumentsC)
    {
        QString sInstrumentOutput;
        instrument.getOutputString(movementsHV, movementsC,
            disturbedLoads, frictions, sInstrumentOutput);
        ss << sInstrumentOutput;
        ss << COutputFormatter::formatEndl();
    }

    // 4.5) Write all canonical movements.
    //
    nValuesCounter = 0;
    foreach(double fMovement, movementsC)
    {
        ss << COutputFormatter::formatValue(fMovement, OutputFormat_F10_2);
        if (nValuesCounter++ ==
            CInstrumentsPainterWorkingSet::nOutputRealNumbersInLineCount)
        {
            nValuesCounter = 0;
            ss << COutputFormatter::formatEndl();
        }
    }
    if (nValuesCounter)
        ss << COutputFormatter::formatEndl();

    // 4.6) Write disturbed load instruments.
    //
    foreach(Data::CWPInstrument instrument, disturbedLoadInstruments)
    {
        QString sInstrumentOutput;
        instrument.getOutputString(movementsHV, movementsC,
            disturbedLoads, frictions, sInstrumentOutput);
        ss << sInstrumentOutput;
        ss << COutputFormatter::formatEndl();
    }

    // 4.7) Write all disturbed loads.
    //
    nValuesCounter = 0;
    foreach(double fDisturbedLoad, disturbedLoads)
    {
        ss << COutputFormatter::formatValue(fDisturbedLoad, OutputFormat_F10_2);
        if (nValuesCounter++ ==
            CInstrumentsPainterWorkingSet::nOutputRealNumbersInLineCount)
        {
            nValuesCounter = 0;
            ss << COutputFormatter::formatEndl();
        }
    }
    if (nValuesCounter)
        ss << COutputFormatter::formatEndl();

    // 4.8) Write friction considering instruments.
    //
    foreach(Data::CWPInstrument instrument, frictionConsideringInstruments)
    {
        QString sInstrumentOutput;
        instrument.getOutputString(movementsHV, movementsC,
            disturbedLoads, frictions, sInstrumentOutput);
        ss << sInstrumentOutput;
        ss << COutputFormatter::formatEndl();
    }

    // 4.9) Write all frictions.
    //
    nValuesCounter = 0;
    foreach(double fFriction, frictions)
    {
        ss << COutputFormatter::formatValue(fFriction, OutputFormat_F10_2);

        if (nValuesCounter++ ==
            CInstrumentsPainterWorkingSet::nOutputRealNumbersInLineCount)
        {
            nValuesCounter = 0;
            ss << COutputFormatter::formatEndl();
        }
    }
    if (nValuesCounter)
        ss << COutputFormatter::formatEndl();

    // 4.10) Write stress strain instruments.
    //
    foreach(Data::CWPInstrument instrument, stressStrainInstruments)
    {
        QString sInstrumentOutput;
        instrument.getOutputString(movementsHV, movementsC,
            disturbedLoads, frictions, sInstrumentOutput);
        ss << sInstrumentOutput;
        ss << COutputFormatter::formatEndl();
    }

    // 4.11) Write workpiece matherial parameners.
    //
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_fYoungModulus, OutputFormat_F10_2);
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_fPuassoneCoefficient, OutputFormat_F10_2);
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_fMetalFlowBoundary, OutputFormat_F10_2);
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_fKCoefficient, OutputFormat_F10_2);
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_fNCoefficient, OutputFormat_F10_2);
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_fACoefficient, OutputFormat_F10_2);
    ss << COutputFormatter::formatValue(
        m_workingSet.m_wpProperties.m_fBCoefficient, OutputFormat_F10_2);

    ss << COutputFormatter::formatEndl() << flush;
    m_workingSet.m_sOutput = sOutput;

    if (bEmitSignals)
        emit dataReady();
    return true;
}

void CInstrumentsPainter::onDataReady()
{
    if (QMessageBox::question(m_pWorkAreaWidget, tr("Are you sure?"),
        tr("Write instruments?"), QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes) == QMessageBox::Yes)
        generateOutputString();

    m_pWorkAreaWidget->setFocus();
}

void CInstrumentsPainter::onClearWorkspace()
{
    m_workingSet.m_instruments.clear();
    cleanSelection();

    m_pWorkAreaWidget->update();
    m_pWorkAreaWidget->setFocus();
}

void CInstrumentsPainter::onRestoreLastDeleted()
{
    if (m_deletedInstruments.empty())
        return;

    m_workingSet.m_instruments.push_back(m_deletedInstruments.top());
    setSelection(m_deletedInstruments.top());
    m_deletedInstruments.pop();

    updateSelInstrumentInfo();

    m_pWorkAreaWidget->update();
    m_pWorkAreaWidget->setFocus();
}

void CInstrumentsPainter::onDeleteSelected()
{
    if (!selectionAvaliable())
        return;

    Data::CWPInstrumentsVector::iterator it = std::find(
        m_workingSet.m_instruments.begin(),
        m_workingSet.m_instruments.end(),
        m_selectedInstrument);

    if (it != m_workingSet.m_instruments.end())
    {
        m_deletedInstruments.push(*it);
        m_workingSet.m_instruments.erase(it);            
    }

    cleanSelection();

    m_pWorkAreaWidget->update();
    m_pWorkAreaWidget->setFocus();
}

void CInstrumentsPainter::onDeleteLastCreated()
{
    if (m_workingSet.m_instruments.empty())
        return;

    m_deletedInstruments.push(m_workingSet.m_instruments.back());
    m_workingSet.m_instruments.erase(
        m_workingSet.m_instruments.end() - 1);

    cleanSelection();

    m_pWorkAreaWidget->update();
    m_pWorkAreaWidget->setFocus();
}

void CInstrumentsPainter::onClearDeletionHistory()
{
    m_deletedInstruments.clear();

    m_pWorkAreaWidget->setFocus();
}