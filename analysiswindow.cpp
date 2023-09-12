//https://www.qcustomplot.com/index.php/introduction
//Lots of information about the QCustomPlots from the example file -> examples/plots/plot-examples.pro.

#include "analysiswindow.h"
#include <QPrinter>
#include <QPixmap>
#include <QPainter>


AnalysisWindow::AnalysisWindow(QWidget *parent, const TransferFunction& transferFunction) :
                               QWidget(parent), ui(new Ui::AnalysisWindow), transferFunction(transferFunction){

    ui->setupUi(this);
    ui->labelFunction->setText("G(s) = " + transferFunction.function);
    ui->label_startFrequency->setText("Start Frequency = " + QString::number(transferFunction.startFrequency));
    ui->label_endFrequency->setText("End Frequency = " + QString::number(transferFunction.endFrequency));
    ui->label_stepSize->setText("Step Size = " + QString::number(transferFunction.stepSize));
    ui->lineEditFilePath->setText("bodePlots");
    //Get data
    AmpData = transferFunction.AmpData;
    PhaseData = transferFunction.PhaseData;
    ampMargin = transferFunction.ampMargin;
    phaseMargin = transferFunction.phaseMargin;
    //Calling functions to generate plots and fill data
    generatingBodePlots();
    insertDataToPlot(AmpData.first,AmpData.second, PhaseData.first, PhaseData.second);
    updateInfoText(QString::number(ampMargin.first), QString::number(ampMargin.second), QString::number(phaseMargin.first), QString::number(phaseMargin.second));
    //Connect the export buttons to slots
    connect(ui->exportAsPNG, &QPushButton::clicked, this, &AnalysisWindow::handleExportAsPngClicked);
    connect(ui->exportAsPDF, &QPushButton::clicked, this, &AnalysisWindow::handleExportAsPdfClicked);
}

AnalysisWindow::~AnalysisWindow()
{
    delete ui;
}

void AnalysisWindow::generatingBodePlots(){
    //Labeling of the axes
    ui->amplitudePlot->yAxis->setLabel("Magnitude (db)");
    ui->phasePlot->yAxis->setLabel("Phase (deg)");
    ui->phasePlot->xAxis->setLabel("Frequenz (rad/s)");
    //Logarithmic scaling and grid
    ui->amplitudePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->amplitudePlot->xAxis->grid()->setSubGridVisible(true);
    ui->phasePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->phasePlot->xAxis->grid()->setSubGridVisible(true);
    //Allow user interactions
    ui->amplitudePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->phasePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void AnalysisWindow::insertDataToPlot(const QVector<double>& xAmpData, const QVector<double>& yAmpData, const QVector<double>& xPhaseData, const QVector<double>& yPhaseData){
    qDebug() << "Passed AmpData to AnalysisWindow: " << xAmpData << yAmpData;
    qDebug() << "Passed PhaseData to AnalysisWIndow: " << xPhaseData << yPhaseData;
    //Add Graph
    QCPGraph* ampGraph = ui->amplitudePlot->addGraph();
    QCPGraph* phaseGraph = ui->phasePlot->addGraph();
    //Add Data
    ampGraph->setData(xAmpData, yAmpData);
    phaseGraph->setData(xPhaseData, yPhaseData);
    //Chose line color and width
    ampGraph->setPen(QPen(Qt::red, 2));
    phaseGraph->setPen(QPen(Qt::red, 2));
    //Set Graph Visible
    ampGraph->setVisible(true);
    phaseGraph->setVisible(true);
    //ui->amplitudePlot->graph(0)->rescaleAxes(true);
    ui->amplitudePlot->xAxis->rescale();
    ui->amplitudePlot->yAxis->rescale();
    //ui->phasePlot->graph(0)->rescaleAxes(true);
    ui->phasePlot->xAxis->rescale();
    ui->phasePlot->yAxis->rescale();
    //Replot
    ui->amplitudePlot->replot();
    ui->phasePlot->replot();
}

void AnalysisWindow::updateInfoText(const QString& ampMargin, const QString& ampMarginFrequency, const QString& phaseMargin, const QString& phaseMarginFrequency){
    qDebug() << "Passed Margins (phase/amp): " << phaseMargin << " / " << ampMargin;
    ui->label_2->setText(phaseMarginFrequency + "rad/s");
    ui->label_4->setText(phaseMargin + "°");
    ui->label_6->setText(ampMarginFrequency + "rad/s");
    ui->label_8->setText(ampMargin + "db");
}

void AnalysisWindow::exportAsPdf(const QString& filePath){
    //https://stackoverflow.com/questions/24356736/qtextdocument-qpdfwriter-how-to-scale-output
    //https://doc.qt.io/qt-6/qpdfwriter.html
    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QPageSize::A4));
    int logicalDPIX=writer.logicalDpiX();
    const int PointsPerInch=72;
    QPainter painter(&writer);
    //Attempted to fix the scaling cause of pdf scaling bugs. For more info take a look at link above.
    QTransform t;
    float scaling=(float)logicalDPIX/PointsPerInch;  // 16.6
    t.scale(scaling/2,scaling/2);
    painter.setTransform(t);
    this->render(&painter);
}

void AnalysisWindow::exportAsPng(const QString& filePath){
    //https://www.qtcentre.org/threads/67425-Saving-the-entire-ui-form-as-an-Image-file-from-QTcreator
    //https://doc.qt.io/qt-6/qpixmap.html#save
    QPixmap pixmap(this->size());
    QPainter painter(&pixmap);
    this->render(&painter);
    pixmap.save(filePath, nullptr, -1);
}

void AnalysisWindow::handleExportAsPngClicked(){
    //Get path of project repository
    QString currentPath = QDir::currentPath();
    //Create file path with name
    QString filePath = currentPath + "/" + ui->lineEditFilePath->text() + ".png";
    //call function with file path
    exportAsPng(filePath);
}

void AnalysisWindow::handleExportAsPdfClicked(){
    //Get path of project repository
    QString currentPath = QDir::currentPath();
    //Create file path with name
    QString filePath = currentPath + "/" + ui->lineEditFilePath->text() + ".pdf";
    //Call function with file path
    exportAsPdf(filePath);
}

