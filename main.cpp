
#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // Festlegen der Größenrichtlinien für das Widget in Ihrer MainWindow-Klasse
    w.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    w.setMinimumSize(640, 360);
    w.setMaximumSize(1920, 1080);
    // Set focus to the window
    w.show();
    w.activateWindow();
    w.raise();
    w.setFocus();
    return a.exec();
}
