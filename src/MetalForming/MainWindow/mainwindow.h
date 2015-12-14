#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QtGui/QDialog>
#include "ui_mainwindow.h"

class CMainWindow : public QDialog
{
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~CMainWindow();

    void initMetalFormingWidget(
        const QString &sStartupProjectFilePath = QString());

protected slots:
    void onProjectNameChanged(const QString &sProjectFilePath);

protected:
    virtual void keyPressEvent (QKeyEvent * pEvent);
private:
    void setMainWindowCaption(const QString &sProjectName);

private:
    QString m_sMainWindowBaseCaption;

private:
    Ui::CMainWindowClass ui;
};

#endif // CMAINWINDOW_H
