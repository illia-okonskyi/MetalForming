#pragma once
#include "../Data/Data.h"

class QGroupBox;
class QLabel;
class CWPLine;
class QPushButton;

class CZonesPainterWorkingSet
{
public:
    // Input data
    //
    QVector<QPointF> m_workpiecePoints;
    Data::CWPLinesVector m_workpieceLines;
    Data::CWPProperties m_wpProperties;
    Data::CWPLinesVector m_workpieceGridCompactionLines;
    
    // Working data
    //
    Data::CWPZonesVector m_workpieceZones;

    // Output data
    //
    QString m_sZoneMap;
    QString m_sOutput;

    // Output format parameters
    //
    enum
    {
        g_nZonesInLineCount = 16
    };

    // GUI objects
    //
    QGroupBox *m_gbInfo;
    QLabel *m_labelZoneType;
    QLabel *m_labelSelZone;

    QGroupBox *m_gbZoneMapInfo;
    QLabel *m_labelZoneMap;

    QPushButton *m_btnCompileData;
    QPushButton *m_btnClearWorkspace;
    QPushButton *m_btnChangeType;
    QPushButton *m_btnRestoreLastDeleted;
    QPushButton *m_btnDeleteSelected;
    QPushButton *m_btnDeleteLastCreated;
    QPushButton *m_btnClearDeletionHistory;
};