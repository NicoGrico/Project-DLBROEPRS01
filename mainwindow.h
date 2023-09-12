
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "analysiswindow.h"
#include "transferfunction.h"
#include <QList>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QStringList frequencyStartOptions = { "0.0001", "0.001", "0.01", "0.1", "1" };
    QStringList frequencyEndOptions = { "10", "100", "1000", "10000", "100000" };

    Ui::MainWindow *ui;
    QList<AnalysisWindow*> analysisWindowList;
    void getInput();
    void updateTextBrowser(QString text);
signals:
    void inputReceived(QString functionGain, QString functioNumerator, QString functionDenominator, double startFrequency, double endFrequency, double stepSize);
    void outputReceived(TransferFunction transferFunction);
private slots:
    void handleInputReceived(const QString &functionGain, const QString &functionNumerator, const QString &functionDenominator, double startFrequency, double endFrequency, double stepSize);
    void handleOutputReceived(TransferFunction transferFunction);

};

#endif // MAINWINDOW_H
