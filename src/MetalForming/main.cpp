#include "MainWindow\mainwindow.h"
#include <QtGui/QApplication>
#include <QtCore/QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Install current languadge translate
    //
    QTranslator translatorQt;
    QTranslator translatorApp;
    translatorApp.load("MetalForming", "../Translate");
    translatorQt.load("Qt", "../Translate");
    app.installTranslator(&translatorQt);
    app.installTranslator(&translatorApp);

    CMainWindow mainWindow(0, Qt::WindowMaximizeButtonHint |
        Qt::WindowMinimizeButtonHint);
    mainWindow.setWindowState(Qt::WindowMaximized);
    mainWindow.show();
    QString sStartupProject;
    if (argc > 1)
        sStartupProject = argv[1];
    mainWindow.initMetalFormingWidget(sStartupProject);
    return app.exec();
}
