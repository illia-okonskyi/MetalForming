#pragma once

#include "MetalFormingStrategy.h"
#include "../WorkingSets/ProjectActionsHandlerWorkingSet.h"

class CProjectsActionsHandler : public IMetalFormingStrategy
{
public:
    Q_OBJECT;

public:
    CProjectsActionsHandler(CBaseWorkingSet& baseWorkingSet,
        QWidget *pWorkAreaWidget, CProjectActionsHandlerWorkingSet &workingSet);

    virtual void init();
    virtual void deinit();

    virtual bool keyPressEventHandler(QWidget *pWidget, QKeyEvent * pEvent);

protected slots:
    void onSaveProjectQuery();
    void onSaveAsProjectQuery();
    void onLoadProjectQuery();
    void onNewProjectQuery();

signals:
        // This signals return true if operation was ended with success.
        //
        bool saveProject(const QString &sFilePath);
        bool loadProject(const QString &sFilePath);
        void newProject();

private:
    CProjectActionsHandlerWorkingSet &m_workingSet;
};
