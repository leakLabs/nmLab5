#include "intgrl_func.h"

#define OUT_FIELD_WIDTH 15

float f(float x, float y)
{
    return 3 * x * x;
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

float rungeK2(float x, float y, float h)
{
    float k1 = h * f(x, y);
    float k2 = h * f(x + h, y + k1);
    return y + 0.5 * (k1 + k2);
}

float rungeK3(float x, float y, float h)
{
    float k1 = h * f(x, y);
    float k2 = h * f(x + 0.5 * h, y + 0.5 * k1);
    float k3 = h * f(x + h, y - k1 + 2 * k2);
    return y + 1.0 / 6.0 * (k1 + 4 * k2 + k3);
}

int processFile(std::istream& inStream, std::ostream& outStream)
{
    InData data;
    readFile(inStream, data);
    float h = (data.B - data.A) / 10;
    float x, endPoint, y = data.yc;
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

    float localError = 0;
    while (data.direction == LR && x<endPoint || data.direction == RL && x>endPoint) {
        float y2 = rungeK2(x, y, h);
        float y3 = rungeK3(x, y, h);
        localError = abs(y3 - y2);

        if (localError < data.eps) {
            icod = 0;
            stepsCount++;
            y = y2;
            printStep(outStream, x, y, localError);

            if (data.direction == LR && endPoint - (x + h) < data.hmin || data.direction == RL && x - (endPoint + h) < data.hmin) {
                if (data.direction == LR && endPoint - x >= 2 * data.hmin || data.direction == RL && x - endPoint >= 2 * data.hmin) {
                    x = (data.direction == LR) ? endPoint - data.hmin : endPoint + data.hmin;
                    y = rungeK2(x, y, h);
                    printStep(outStream, x, y, localError);
                } else if (data.direction == LR && endPoint - x <= 1.5 * data.hmin || data.direction == RL && x - endPoint <= 1.5 * data.hmin) {
                    x = endPoint;
                }
                else if ((data.direction == LR && endPoint - x > 1.5 * data.hmin && endPoint - x < 2 * data.hmin) 
                    || (data.direction == RL && x - endPoint > 1.5 * data.hmin && x - endPoint < 2 * data.hmin)) {
                    x = (data.direction == LR) ? x + (endPoint - x) / 2 : x - (x - endPoint) / 2;
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
                h /= 2;
                y2 = rungeK2(x, y, h);
                y3 = rungeK3(x, y, h);
                localError = abs(y3 - y2);
                icod = 0;
            }

            if (localError < data.eps / 4) {
                h *= 2;
            }

            if (h < data.hmin) {
                h = data.hmin;
                icod = 1;
            }

            y2 = rungeK2(x, y, h);
            printStep(outStream, x, y, localError);

            y = y2;
            x += h;
        }
    }
    outStream << stepsCount << ' ' << errorStepsCount << ' ' << stepsCount + errorStepsCount << '\n';
    return icod;
}

void printStep(std::ostream& stream, float x, float y, float localError)
{
    stream << std::left << std::setw(OUT_FIELD_WIDTH) << x << std::setw(OUT_FIELD_WIDTH) << y << std::setw(OUT_FIELD_WIDTH) << localError << std::endl;
}