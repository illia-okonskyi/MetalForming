#pragma once
class QLabel;
class QPushButton;
class QScrollArea;

class CScaleSetterWorkingSet
{
public:
    // GUI objects
    //
    QLabel *m_labelScale;

    QPushButton *m_btnScaleIncrease;
    QPushButton *m_btnScaleDecrease;
    QPushButton *m_btnScaleReset;

    QScrollArea *m_scrollArea;
};