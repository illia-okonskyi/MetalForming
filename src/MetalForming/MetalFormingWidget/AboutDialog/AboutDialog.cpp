#include "AboutDialog.h"
#include <Shlwapi.h>

CAboutDialog::CAboutDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.label, SIGNAL(linkActivated(const QString&)),
        this, SLOT(browseLink(const QString&)));
}

CAboutDialog::~CAboutDialog()
{

}

void CAboutDialog::browseLink( const QString &sLink )
{
    ShellExecuteA(0, "open", sLink.toAscii(), 0, 0, SW_SHOW);
}
