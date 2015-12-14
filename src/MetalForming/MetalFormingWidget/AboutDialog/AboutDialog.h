#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "ui_AboutDialog.h"

class CAboutDialog : public QDialog
{
    Q_OBJECT

public:
    CAboutDialog(QWidget *parent = 0);
    ~CAboutDialog();

protected slots:
    void browseLink(const QString &sLink);

private:
    Ui::CAboutDialogClass ui;
};

#endif // ABOUTDIALOG_H
