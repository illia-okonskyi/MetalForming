#pragma once
#include "../Data/Data.h"

class QGroupBox;
class QLabel;
class CWPLine;
class QtVariantPropertyManager;
class QtTreePropertyBrowser;
class QPushButton;

class CInstrumentsPainterWorkingSet
{
public:
    // Input data
    //
    Data::CWPLinesVector m_workpieceMainLines;
    Data::CWPProperties m_wpProperties;

    // Working data
    //
    Data::CWPInstrumentsVector m_instruments;

    // Output data
    //
    QString m_sOutput;

    // Output format parameters
    //
    enum
    {
        nOutputRealNumbersInLineCount = 6
    };

    // GUI objects
    //
    QGroupBox *m_gbInfo;
    QLabel *m_labelInstrumentAngle;
    QLabel *m_labelSelInstrument;

    QtVariantPropertyManager *m_propertiesManager;
    QtTreePropertyBrowser *m_propertiesEditor;

    QPushButton *m_btnCompileData;
    QPushButton *m_btnClearWorkspace;
    QPushButton *m_btnRestoreLastDeleted;
    QPushButton *m_btnDeleteSelected;
    QPushButton *m_btnDeleteLastCreated;
    QPushButton *m_btnClearDeletionHistory;
};