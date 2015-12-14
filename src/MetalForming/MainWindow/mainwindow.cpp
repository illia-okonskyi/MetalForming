#include "mainwindow.h"
#include <QtGui/QKeyEvent>

CMainWindow::CMainWindow(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags), m_sMainWindowBaseCaption(tr("Metal Forming"))
{
    ui.setupUi(this);
    setMainWindowCaption(QString());

    connect(ui.metalForming, SIGNAL(projectLoaded(const QString &)),
        this, SLOT(onProjectNameChanged(const QString &)));
    connect(ui.metalForming, SIGNAL(projectSaved(const QString &)),
        this, SLOT(onProjectNameChanged(const QString &)));
}

CMainWindow::~CMainWindow()
{

}

void CMainWindow::initMetalFormingWidget(
    const QString &sStartupProjectFilePath /*= QString()*/ )
{
    ui.metalForming->init(sStartupProjectFilePath);
}

void CMainWindow::onProjectNameChanged( const QString &sProjectFilePath )
{
    // In program caption we are showing only file name without extension.
    //
    QString sProjectFileName = sProjectFilePath;
    sProjectFileName.replace(QChar('\\'), QChar('/'));
    sProjectFileName = sProjectFileName.section('/', -1);
    setMainWindowCaption(sProjectFileName.section('.', 0, 0));
}

void CMainWindow::setMainWindowCaption( const QString &sProjectName )
{
    QString sDisplayProjectName = tr("New Project");
    if (!sProjectName.isEmpty())
        sDisplayProjectName = sProjectName;
    setWindowTitle(m_sMainWindowBaseCaption + tr(" - ") + sDisplayProjectName);
}

void CMainWindow::keyPressEvent( QKeyEvent * pEvent )
{
    // Do not pass Escape key to the QDialog to prevent close
    //
    if (pEvent->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(pEvent);
}
