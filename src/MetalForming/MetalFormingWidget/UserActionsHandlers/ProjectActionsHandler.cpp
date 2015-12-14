#include "ProjectActionsHandler.h"
#include "../MFProjectFileDefinitions.h"

#include <QtGui//QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QKeyEvent>
#include <QtGui/QPushButton>

CProjectsActionsHandler::CProjectsActionsHandler(
    CBaseWorkingSet& baseWorkingSet,
    QWidget *pWorkAreaWidget,
    CProjectActionsHandlerWorkingSet &workingSet):
IMetalFormingStrategy(baseWorkingSet, pWorkAreaWidget), m_workingSet(workingSet)
{

}

void CProjectsActionsHandler::init()
{
    m_workingSet.m_sCurrentProjectFilePath.clear();

    // Connect toolbar buttons signals
    //
    connect(m_workingSet.m_btnNewProject, SIGNAL(clicked()), this,
        SLOT(onNewProjectQuery()));
    connect(m_workingSet.m_btnLoadProject, SIGNAL(clicked()), this,
        SLOT(onLoadProjectQuery()));
    connect(m_workingSet.m_btnSaveProject, SIGNAL(clicked()), this,
        SLOT(onSaveProjectQuery()));
    connect(m_workingSet.m_btnSaveAsProject, SIGNAL(clicked()), this,
        SLOT(onSaveAsProjectQuery()));
}

void CProjectsActionsHandler::deinit()
{
    disconnect();
}

bool CProjectsActionsHandler::keyPressEventHandler( QWidget *pWidget,
                                                   QKeyEvent * pEvent )
{
    bool bActionHandled = true;
    if (pEvent->modifiers() == Qt::ControlModifier)
    {
        switch (pEvent->key())
        {
        default:
            bActionHandled = false;
            break;
        case Qt::Key_L:
            onLoadProjectQuery();
            break;
        case Qt::Key_S:
            onSaveProjectQuery();
            break;
        case Qt::Key_A:
            onSaveAsProjectQuery();
            break;
        case Qt::Key_N:
            onNewProjectQuery();
            break;
        }
    }
    else
        bActionHandled = false;

    return (!bActionHandled);
}

void CProjectsActionsHandler::onSaveProjectQuery()
{
    if (m_workingSet.m_sCurrentProjectFilePath.isEmpty())
        onSaveAsProjectQuery();
    else
    {
        if (emit saveProject(m_workingSet.m_sCurrentProjectFilePath))
            QMessageBox::information(m_pWorkAreaWidget, tr("Project saved"),
            tr("Project saved"));
        else
            QMessageBox::critical(m_pWorkAreaWidget,
            tr("Project saving failed"),
            tr("Project saving failed"));
    }

}

void CProjectsActionsHandler::onSaveAsProjectQuery()
{
    QString sFilePath =  QFileDialog::getSaveFileName(m_pWorkAreaWidget,
        tr("Save As Metal Forming Project"), QString(),
        tr("Metal Forming Project Files (*") + MFProject::g_sProjectExtension +
        ")");

    if (!sFilePath.isEmpty())
    {
        if (emit saveProject(sFilePath))
            QMessageBox::information(m_pWorkAreaWidget, tr("Project saved"),
            tr("Project saved"));
        else
            QMessageBox::critical(m_pWorkAreaWidget,
            tr("Project saving failed"),
            tr("Project saving failed"));
    }
}

void CProjectsActionsHandler::onLoadProjectQuery()
{
    QString sFilePath =  QFileDialog::getOpenFileName(m_pWorkAreaWidget,
        tr("Open Metal Forming Project"), QString(),
        tr("Metal Forming Project Files (*") + MFProject::g_sProjectExtension +
        ")", 0, QFileDialog::ReadOnly);

    if (!sFilePath.isEmpty())
    {
        if (emit loadProject(sFilePath))
            QMessageBox::information(m_pWorkAreaWidget, tr("Project loaded"),
            tr("Project loaded"));
        else
            QMessageBox::critical(m_pWorkAreaWidget,
            tr("Project loading failed"),
            tr("Project loading failed"));
    }
}

void CProjectsActionsHandler::onNewProjectQuery()
{
    emit newProject();
    QMessageBox::information(m_pWorkAreaWidget, tr("New project created"),
        tr("New project created"));
}
