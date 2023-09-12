
#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include <QVector>

class TransferFunction
{
public:
    //Konstruktoren
    TransferFunction(const QString& functionGain, const QString& functionNumerator, const QString& functionDenominator, double startFrequency, double endFrequency, double stepSize);
    //variable
    QPair<QVector<double>, QVector<double>> AmpData;
    QPair<QVector<double>, QVector<double>> PhaseData;
    QPair<double, double> phaseMargin;
    QPair<double, double> ampMargin;
    QString function;
    double stepSize;
    double startFrequency;
    double endFrequency;
private:
    //Variable
    QString functionGain;
    QString functionNumerator;
    QString functionDenominator;

    int startExponent;
    int endExponent;

    //Methoden
    QPair<QVector<double>, QVector<double> > getCalculatedAmplitude();
    QPair<QVector<double>, QVector<double> > getCalculatedPhase();

    QPair<QVector<double>, QVector<double>> calculateAmplitudeGain(const QString& factor, int startExponent, int endExponent, double stepSize);
    QPair<QVector<double>, QVector<double>> calculateAmplitude(const std::vector<std::string>& parts, int startExponent, int endExponent, double stepSize);

    QPair<QVector<double>, QVector<double> > calculatePhaseGain(const QString& factor, int startExponent, int endExponent, double stepSize);
    QPair<QVector<double>, QVector<double>> calculatePhase(const std::vector<std::string>& parts, int startExponent, int endExponent, double stepSize);

    QPair<double, double> calculateGainMargin(const QVector<double>& yAmpData, const QVector<double>& xAmpData, const QVector<double>& yPhaseData);
    QPair<double, double> calculatePhaseMargin(const QVector<double>& yAmpData, const QVector<double>& xAmpData, const QVector<double>& yPhaseData);

    std::vector<std::string> splitString(const std::string& string);
    std::pair<std::string, std::string> splitIntoImRe(const std::string& string);

    int findNearestValueToX(const QVector<double> &data, double X);
};

#endif // TRANSFERFUNCTION_H
