#include "intgrl_func.h"

#define OUT_FIELD_WIDTH 15

double f(double x, double y)
{
    //return y - x * x * (x - 3.0); //1
    return 1; //2
}

void readFile(std::istream& stream, InData& data)
{
    stream >> data.A >> data.B >> data.C >> data.yc >> data.hmin >> data.eps;
    if (data.A == data.C)
        data.direction = LR;
    else if (data.B == data.C)
        data.direction = RL;
    else
        throw std::string{ "C не совпадает с концами отрезка" };

    if (data.A > data.B)
        throw std::string{ "A > B" };
}

double rungeK2(double x, double y, double h)
{
    double k1 = h * f(x, y);
    double k2 = h * f(x + h, y + k1);
    return y + 0.5 * (k1 + k2);
}

double rungeK3(double x, double y, double h)
{
    double k1 = h * f(x, y);
    double k2 = h * f(x + 0.5 * h, y + 0.5 * k1);
    double k3 = h * f(x + h, y - k1 + 2.0 * k2);
    return y + 1.0 / 6.0 * (k1 + 4 * k2 + k3);
}

int processFile(std::istream& inStream, std::ostream& outStream)
{
    InData data;
    readFile(inStream, data);
    double h = (data.B - data.A) / 10.0;
    double x, endPoint, y = data.yc;
    int stepsCount = 0, errorStepsCount = 0, icod = 0;
    outStream << std::left << std::setw(OUT_FIELD_WIDTH) << 'x' << std::setw(OUT_FIELD_WIDTH) << 'y' << std::setw(OUT_FIELD_WIDTH) << "local error" << std::endl;
    if (data.direction == LR) {
        x = data.A;
        endPoint = data.B;
    } else if (data.direction == RL) {
        x = data.B;
        h = -h;
        endPoint = data.A;
    } else {
        throw std::string{ "„то-то пошло не так\n" };
    }

    double localError = 0;
    while (data.direction == LR && x<endPoint || data.direction == RL && x>endPoint) {
        double y2 = rungeK2(x, y, h);
        double y3 = rungeK3(x, y, h);
        localError = abs(y3 - y2);

        if (localError < data.eps) {
            icod = 0;
            if (stepsCount != 0)
                y = y2;
            printStep(outStream, x, y, localError);
            stepsCount++;

            if (data.direction == LR && endPoint - (x + h) < data.hmin || data.direction == RL && x - (endPoint + h) < data.hmin) {
                if (data.direction == LR && endPoint - x >= 2.0 * data.hmin || data.direction == RL && x - endPoint >= 2.0 * data.hmin) {
                    x = (data.direction == LR) ? endPoint - data.hmin : endPoint + data.hmin;
                    y = rungeK2(x, y, h);
                    printStep(outStream, x, y, localError);
                } else if (data.direction == LR && endPoint - x <= 1.5 * data.hmin || data.direction == RL && x - endPoint <= 1.5 * data.hmin) {
                    x = endPoint;
                }
                else if ((data.direction == LR && endPoint - x > 1.5 * data.hmin && endPoint - x < 2.0 * data.hmin) 
                    || (data.direction == RL && x - endPoint > 1.5 * data.hmin && x - endPoint < 2.0 * data.hmin)) {
                    x = (data.direction == LR) ? x + (endPoint - x) / 2.0 : x - (x - endPoint) / 2.0;
                    y = rungeK2(x, y, h);
                    printStep(outStream, x, y, localError);
                }
                x = endPoint;
                y = rungeK2(x, y, h);
                printStep(outStream, x, y, localError);
            }
            else {
                x += h;
            }
        } else {
            errorStepsCount++;

            while (localError > data.eps)
            {
                h /= 2.0;
                y2 = rungeK2(x, y, h);
                y3 = rungeK3(x, y, h);
                localError = abs(y3 - y2);
                icod = 0;
            }

            if (localError < data.eps / 4) {
                h *= 2.0;
            }

            if (h < data.hmin) {
                h = data.hmin;
                icod = 1;
            }

            y2 = rungeK2(x, y, h);
            //std::cout << "error\n";
            printStep(outStream, x, y, localError);

            y = y2;
            x += h;
        }
    }
    outStream << stepsCount << ' ' << errorStepsCount << ' ' << stepsCount + errorStepsCount << '\n';
    return icod;
}

void printStep(std::ostream& stream, double x, double y, double localError)
{
    stream << std::left << std::setw(OUT_FIELD_WIDTH) << x << std::setw(OUT_FIELD_WIDTH) << y << std::setw(OUT_FIELD_WIDTH) << localError << std::endl;
}