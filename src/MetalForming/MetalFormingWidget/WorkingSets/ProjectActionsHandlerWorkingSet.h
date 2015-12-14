#pragma once
class QPushButton;

class CProjectActionsHandlerWorkingSet
{
public:
    // Input data
    //
    QString m_sCurrentProjectFilePath;

    // GUI objects
    //
    QPushButton *m_btnNewProject;
    QPushButton *m_btnLoadProject;
    QPushButton *m_btnSaveProject;
    QPushButton *m_btnSaveAsProject;
};