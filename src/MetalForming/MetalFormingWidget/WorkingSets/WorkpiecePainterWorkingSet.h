#pragma once
#include "../Data/Data.h"

class QGroupBox;
class QLabel;
class CWPLine;
class QPushButton;

class CWorkpiecePainterWorkingSet
{
public:
    // Working data
    //
    Data::CWPLinesVector m_workpieceLines;

    // Output data
    //
    QVector<QPointF> m_workpiecePoints;
    Data::CWPLinesVector m_workpieceGridCompactionLines;

    // GUI objects
    //
    QGroupBox *m_gbInfo;
    QLabel *m_labelLineAngle;
    QLabel *m_labelLineType;
    QLabel *m_labelSelLine;

    QPushButton *m_btnCompileData;
    QPushButton *m_btnClearWorkspace;
    QPushButton *m_btnChangeType;
    QPushButton *m_btnRestoreLastDeleted;
    QPushButton *m_btnDeleteSelected;
    QPushButton *m_btnDeleteLastCreated;
    QPushButton *m_btnClearDeletionHistory;
};