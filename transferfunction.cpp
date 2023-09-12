//The functions "calculateAmplitudGain", "calculateAmplitude", "calculatePhaseGain", "calculatePhase" share repetitive aspects due to their common use of the exprtk library.
//Since they all evaluate mathematical expressions with exprtk, I will provide detailed comments for expertk in the function "calculateAmplitudeGain".
//For reasons of clarity I won't go into every basic Exprtk aspect again in every function! I will continue to address unique or divergent aspects as needed.
//For further information, refer to http://www.partow.net/programming/exprtk/.

//Why going in the splitIntoImRe and splitString with the namespace std?
//Since exprtk can't operate with "Qt"-components, we can safe some memory by directly utilizing the "std" components from the start.

//Include necessary header files
#include "transferfunction.h"
#include "exprtk.hpp"
#include <cmath>
#include <QString>
#include <QDebug>
#include <QVector>

//Constructor definition for TransferFunction class
TransferFunction::TransferFunction(const QString& functionGain, const QString& functionNumerator, const QString& functionDenominator,
                                   double startFrequency, double endFrequency, double stepSize) :
                                   stepSize(stepSize), startFrequency(startFrequency), endFrequency(endFrequency),
                                   functionGain(functionGain), functionNumerator(functionNumerator), functionDenominator(functionDenominator)
{
    //Convert standard notation to exponential notation e.g. 1000 -> 10^3 ; 0.001 -> 10^-3
    startExponent = static_cast<int>(std::floor(std::log10(startFrequency)));
    endExponent = static_cast<int>(std::floor(std::log10(endFrequency)));
    function = functionGain + "(" + functionNumerator + "/(" + functionDenominator + "))";
    AmpData = (getCalculatedAmplitude());
    PhaseData = (getCalculatedPhase());
    phaseMargin = calculatePhaseMargin(AmpData.second,  AmpData.first, PhaseData.second);
    ampMargin = calculateGainMargin(AmpData.second, AmpData.first, PhaseData.second);
}

//Function to calculated Amplitude for each part of the function. Adds everything up.
QPair<QVector<double>, QVector<double>> TransferFunction::getCalculatedAmplitude(){

    QPair<QVector<double>, QVector<double>> GainData = calculateAmplitudeGain(functionGain, startExponent, endExponent, stepSize);
    QPair<QVector<double>, QVector<double>> NumeratorData = calculateAmplitude(splitString(functionNumerator.toStdString()), startExponent, endExponent, stepSize);
    QPair<QVector<double>, QVector<double>> DenominatorData = calculateAmplitude(splitString(functionDenominator.toStdString()), startExponent, endExponent, stepSize);
    //Add frequency steps to xAmpData
    QVector<double> xAmpData = GainData.second;
    QVector<double> yAmpData;
    //Adds everything up to total amplitude data
    for (int i = 0; i < GainData.first.size(); ++i) {
        yAmpData.append(GainData.first.at(i) + NumeratorData.first.at(i) - DenominatorData.first.at(i));
    }
    return qMakePair(xAmpData, yAmpData);
}

//Function to calculated Phase for each part of the function. Adds everything up.
QPair<QVector<double>, QVector<double>> TransferFunction::getCalculatedPhase(){

    QPair<QVector<double>, QVector<double>> GainData = calculatePhaseGain(functionGain, startExponent, endExponent, stepSize);
    QPair<QVector<double>, QVector<double>> NumeratorData = calculatePhase(splitString(functionNumerator.toStdString()), startExponent, endExponent, stepSize);
    QPair<QVector<double>, QVector<double>> DenominatorData = calculatePhase(splitString(functionDenominator.toStdString()), startExponent, endExponent, stepSize);
    //Add frequency steps to xAmpData
    QVector<double> xPhaseData = GainData.second;
    QVector<double> yPhaseData;
    //Adds everything up to total phase data
    for (int i = 0; i < GainData.first.size(); ++i) {
        yPhaseData.append(GainData.first.at(i) + NumeratorData.first.at(i) - DenominatorData.first.at(i));
    }
    return qMakePair(xPhaseData, yPhaseData);
}

//Function to calculate amplitude gain data
QPair<QVector<double>, QVector<double>> TransferFunction::calculateAmplitudeGain(const QString& gain, int startExponent, int endExponent, double stepSize){
    
    //Instantiate QVector<double> to store final results
    QVector<double> xAmpData;
    QVector<double> yAmpData;
    // Construct the expression string for amplitude calculation.
    std::string expression_String = "20.0*log10(abs(" + gain.toStdString() + "))";
    //Instantiate an symbol_table. Holder of external variables.
    //Initialize currentFrequency and add variable to symbol table
    double currentFrequency = 0.0;
    exprtk::symbol_table<double> symbolTable;
    symbolTable.add_variable("s", currentFrequency);
    //Instantiate an expression and register the symbolTable
    exprtk::expression<double> expression;
    expression.register_symbol_table(symbolTable);
    //Instantiate an parser object.
    exprtk::parser<double> parser;
    //Loop through frequency exponents.
    for (int exponent = startExponent; exponent <= endExponent; ++exponent) {
        //Loop through current power of ten 10^i
        for (double i = 0; i < 10;) {
            //Convert the frequency back to standard notation.
            //Also calculating the frequency with increasing exponent.
            currentFrequency = std::pow(10.0, exponent + i * 0.1);
            //Update the reference for s
            symbolTable.variable_ref("s") = currentFrequency;
            //Add the expression_String and expression object to the parser
            //Try to compile the expression with the current frequency s
            if (!parser.compile(expression_String, expression))
            {
                qDebug() << "Expression could not be evaluated.";
                continue;
            }
            //Add the values to the vectors
            double result = expression.value();
            yAmpData.append(result);
            xAmpData.append(currentFrequency);
            i += 10 / stepSize;
        }
    }
    return qMakePair(yAmpData, xAmpData);
}

//Function to calculate amplitude data
QPair<QVector<double>, QVector<double>> TransferFunction::calculateAmplitude(const std::vector<std::string>& parts, int startExponent, int endExponent, double stepSize){
    //Instantiate QVector<double> to store final results
    QVector<double> yAmpData;
    QVector<double> xAmpData;
    //Instantiate an symbol_table. Holder of external variables.
    //Initialize currentFrequency and add variable to symbol table
    double currentFrequency = 0.0;
    exprtk::symbol_table<double> symbolTable;
    symbolTable.add_variable("s", currentFrequency);
    //Instantiate an expression and register the symbolTable
    exprtk::expression<double> expression;
    expression.register_symbol_table(symbolTable);
    //Instantiate an parser object.
    exprtk::parser<double> parser;
    //Loop through frequency exponents.
    for (int exponent = startExponent; exponent <= endExponent; ++exponent) {
        //Loop through current power of ten 10^i
        for (double i = 0; i < 10;) {
            //Convert the frequency back to standard notation.
            //Also calculating the frequency with increasing exponent.
            currentFrequency = std::pow(10.0, exponent + i * 0.1);
            //Update the reference for s.
            symbolTable.variable_ref("s") = currentFrequency;
            QVector<double> yPartAmpData;
            //Loop through all parts of the const std::vector<std::string>& parts (for the current frequency s).
            for (const std::string& part : parts) {
                // Construct the expression string for amplitude calculation.
                std::string expression_String = "20.0*log10(abs(" + part + "))";
                //Try to compile the expression with the current frequency s
                if (!parser.compile(expression_String, expression))
                {
                    qDebug() << "Expression could not be evaluated.";
                    continue;
                }
                //Add the values to the vectors               
                double result = expression.value();
                yPartAmpData.append(result);
            }
            //Calculate total amplitude by summing up all parts (still for the frequency s).
            double sum = 0.0;
            for(const double& partResult : yPartAmpData){
                sum += partResult;
            }
            yAmpData.append(sum);
            xAmpData.append(currentFrequency);
            i += 10 / stepSize;
        }
    }
    return qMakePair(yAmpData, xAmpData);
}

//Function to calculate phase gain data
QPair<QVector<double>, QVector<double>> TransferFunction::calculatePhaseGain(const QString& factor, int startExponent, int endExponent, double stepSize){
    //Since the gain doesn't depend on the current frequency, no exprtk parser is needed.
    //Just check if we have a positive or negative phase shift.
    QVector<double> yPhaseData;
    QVector<double> xPhaseData;
    double gain = factor.toDouble();
    double phase = 0.0;
    double currentFrequency = 0.0;
    if(gain < 0){
        phase = 180;
    }
    //For the purpose of consistency we will still create a QPair with the same stepSize and add the phase shift.
    for (int exponent = startExponent; exponent <= endExponent; ++exponent) {
        for (double i = 0; i < 10;) {
            currentFrequency = std::pow(10.0, exponent + i * 0.1);
            double result = phase;
            yPhaseData.append(result);
            xPhaseData.append(currentFrequency);
            i += 10 / stepSize;
        }
    }
    return qMakePair(yPhaseData, xPhaseData);
}

QPair<QVector<double>, QVector<double>> TransferFunction::calculatePhase(const std::vector<std::string>& parts, int startExponent, int endExponent, double stepSize){
    
    //Zähler(oben) sT<<1 = 0°, sT=1 = +45° sT>>1 = +90° //Nenner(unten) sT<<1 = 0° sT=1 = -45° sT>>1 = -90° //Verstärkung = 0°
    //Instantiate QVector<double> to store final results
    QVector<double> yPhaseData;
    QVector<double> xPhaseData;
    //Instantiate an symbol_table. Holder of external variables.
    //Initialize currentFrequency and add variable to symbol table
    double currentFrequency = 0.0;
    exprtk::symbol_table<double> symbolTable;
    symbolTable.add_variable("s", currentFrequency);
    //Instantiate an expression and register the symbolTable
    exprtk::expression<double> expression;
    expression.register_symbol_table(symbolTable);
    //Instantiate an parser object.
    exprtk::parser<double> parser;
    //Loop through frequency exponents.
    for (int exponent = startExponent; exponent <= endExponent; ++exponent) {
        //Loop through current power of ten 10^i
        for (double i = 0; i < 10;) {
            //Convert the frequency back to standard notation.
            //Also calculating the frequency with increasing exponent.
            currentFrequency = std::pow(10.0, exponent + i * 0.1);
            //Update the reference for s.
            symbolTable.variable_ref("s") = currentFrequency;
            QVector<double> yPartPhaseData;
            //Loop through all parts of the const std::vector<std::string>& parts (for the current frequency s).
            for (const std::string& part : parts) {
                std::pair<std::string, std::string> splitImRe = splitIntoImRe(part);
                std::string expression_String = "atan2(" + splitImRe.first + "," + splitImRe.second + ")*180/3.14159";
                if (!parser.compile(expression_String, expression))
                {
                    qDebug() << "Expression could not be evaluated.";
                }
                double result = expression.value();
                yPartPhaseData.append(result);
            }
            //Calculate total amplitude by summing up all parts (still for the frequency s).
            double sum = 0.0;
            for (const double& partResult : yPartPhaseData){
                sum += partResult;
            }
            yPhaseData.append(sum);
            xPhaseData.append(currentFrequency);
            i += 10 / stepSize;
        }
    }
    return qMakePair(yPhaseData, xPhaseData);
}

QPair<double, double> TransferFunction::calculateGainMargin(const QVector<double>& yAmpData, const QVector<double>& xAmpData, const QVector<double>& yPhaseData){
    //Find index of nearest value to -180°
    int index = findNearestValueToX(yPhaseData, -180.0);
    //Get the amplitude value at the index
    double ampAtIndex = yAmpData[index];
    //Set ampMargin to 0° (value which instability occurs)
    double ampMargin = 0;
    //Subtracting ampMargin with ampAtIndex
    ampMargin -= ampAtIndex;
    return qMakePair(ampMargin, xAmpData[index]);
}

QPair<double, double> TransferFunction::calculatePhaseMargin(const QVector<double>& yAmpData, const QVector<double>& xAmpData, const QVector<double>& yPhaseData){
    //Find index of nearest value to 0.0db
    int index = findNearestValueToX(yAmpData, 0.0);
    //Get the phase value at the index
    double phaseAtIndex = yPhaseData[index];
    //Set PhaseMargin -180° (value which instability occurs)
    double phaseMargin = -180;
    //Subtracting phaseMargin with phaseAtIndex
    phaseMargin -= phaseAtIndex;
    return qMakePair(phaseMargin, xAmpData[index]);
}

std::vector<std::string> TransferFunction::splitString(const std::string& string){
    std::vector<std::string> parts;
    std::string expression = string;
    std::size_t start = 0;
    std::size_t end = 0;
    //Loop through end
    while(end != std::string::npos){
        start = expression.find("(", start);
        end = expression.find(")", start);
        //Extract substrings as long as the end of the string is not reached
        if(start != std::string::npos && end != std::string::npos){
            std::string part = expression.substr(start, end - start + 1);
            parts.push_back(part);
            start = end + 1;
        }
    }
    return parts;
}

std::pair<std::string, std::string> TransferFunction::splitIntoImRe(const std::string& string){

    std::string expression = string;
    //Find the position of the '+' sign in the string.
    std::size_t plusPos = expression.find("+");
    //If '+' is found, extract the parts before and after it.
    if (plusPos != std::string::npos) {
        std::string part1 = expression.substr(0, plusPos);
        std::string part2 = expression.substr(plusPos + 1);
        //Check and remove parentheses at the front and back.
        if (!part1.empty() && part1.front() == '(') {
            part1 = part1.substr(1);
        }
        if (!part2.empty() && part2.back() == ')') {
            part2.pop_back();
        }
        //Create a pair of strings holding the imaginary and real components.
        std::pair<std::string, std::string> stringPair(part1, part2);
        return stringPair;
    } else {
        //If "+" isn't found, it is a D/I-element. Real can be assumed as "0".
        std::string part1 = expression;
        std::string part2 = "0";
        std::pair<std::string, std::string> stringPair(part1, part2);
        return stringPair;
    }
}

int TransferFunction::findNearestValueToX(const QVector<double>& data, double X)
{
    int nearestIndex = -1;
    //Set minDistance to the largest possible positive value
    double minDistance = std::numeric_limits<double>::max();
    double targetValue = X; //Phase shift -180°, Gain 0.0dB
    //Search for a value in the data whose result when subtracted by X is close to zero.
    for (int i = 0; i < data.size(); ++i)
    {
        double currentDistance = std::abs(data[i] - targetValue);
        
        //Compares currentDIstance with minDistance. If currentDistance is less then minDistance. Set currentDistance as new minDistance.
        //After looping through all values of data, the index of the nearest value to X is stored in nearest Index.
        if (currentDistance < minDistance)
        {
            minDistance = currentDistance;
            nearestIndex = i;
        }
    }
    return nearestIndex;
}
