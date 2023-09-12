#ifndef ANALYSISWINDOW_H
#define ANALYSISWINDOW_H

#include "qcustomplot.h"
#include "transferfunction.h"
#include "ui_analysiswindow.h"

namespace Ui {
class AnalysisWindow;
}

class AnalysisWindow : public QWidget
{
    Q_OBJECT

public:
    explicit AnalysisWindow(QWidget *parent, const TransferFunction& transferFunction);
    ~AnalysisWindow();
private:
    Ui::AnalysisWindow *ui;
    TransferFunction transferFunction;
    QPair<QVector<double>, QVector<double>> AmpData;
    QPair<QVector<double>, QVector<double>> PhaseData;
    QPair<double, double> ampMargin;
    QPair<double, double> phaseMargin;
    void exportAsPdf(const QString &filePath);
    void exportAsPng(const QString &filePath);
    void generatingBodePlots();
    void insertDataToPlot(const QVector<double>& xAmpData, const QVector<double>& yAmpData, const QVector<double>& xPhaseData, const QVector<double>& yPhaseData);
    void updateInfoText(const QString& ampMargin, const QString& ampMarginFrequency, const QString& phaseMargin, const QString& phaseMarginFrequency);
private slots:
    void handleExportAsPngClicked();
    void handleExportAsPdfClicked();
};

#endif // ANALYSISWINDOW_H
