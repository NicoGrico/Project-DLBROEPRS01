#include "mainwindow.h"
#include "analysiswindow.h"
#include "transferfunction.h"
#include "ui_mainwindow.h"

//Constructor definition for MainWindow class
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBoxStartFrequency->addItems(frequencyStartOptions);
    ui->comboBoxEndFrequency->addItems(frequencyEndOptions);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::getInput);
    connect(this, &MainWindow::inputReceived, this, &MainWindow::handleInputReceived);
    connect(this, &MainWindow::outputReceived, this, &MainWindow::handleOutputReceived);
}

//Destructor definition for MainWindow class
MainWindow::~MainWindow()
{
    delete ui;
}

//Function to get user input
void MainWindow::getInput(){
    //Get input values from UI and define default values ​​for try/catch.
    QString functionGain = ui->lineEditGain->text();
    QString functionNumerator = ui->lineEditNumerator->text();
    QString functionDenominator = ui->lineEditDenominator->text();
    double startFrequency = ui->comboBoxStartFrequency->currentText().toDouble();
    double endFrequency = ui->comboBoxEndFrequency->currentText().toDouble();
    double stepSize = 30;
    //Try to convert input to double. If successful emit signal with converted input values.
    //Else emit signal with default values.
    try {
        stepSize = ui->lineEditStepSize->text().toDouble();
        emit inputReceived(functionGain, functionNumerator, functionDenominator, startFrequency, endFrequency, stepSize);
    } catch (const std::invalid_argument& e) {
        emit inputReceived(functionGain, functionNumerator, functionDenominator, startFrequency, endFrequency, stepSize);
    } catch (const std::out_of_range& e) {
        emit inputReceived(functionGain, functionNumerator, functionDenominator, startFrequency, endFrequency, stepSize);
    }
}

//Function to instantiate a TransferFunction-class.
void MainWindow::handleInputReceived(const QString &functionGain, const QString &functionNumerator, const QString &functionDenominator, double startFrequency, double endFrequency, double stepSize){
    //Create TransferFunction object with values from inputReceived-signal.
    //Emit outputReceived
    TransferFunction transferFunction(functionGain, functionNumerator, functionDenominator, startFrequency, endFrequency, stepSize);
    emit outputReceived(transferFunction);
}

//Function to instantiate a AnalysisWindow-class.
void MainWindow::handleOutputReceived(TransferFunction transferFunction){
    //Create a AnalysisWindow object and pass transferFunction with data.
    AnalysisWindow* analysisWindow = new AnalysisWindow(nullptr, transferFunction);
    //Append to list. This allows multiple Windows to be held.
    analysisWindowList.append(analysisWindow);
    analysisWindow->show();
}

//Function to append text. For example, to display help and error.
void MainWindow::updateTextBrowser(QString text){
    ui->terminal->append(text);
}
