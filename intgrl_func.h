#pragma once
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>

double func(double x, double y);

enum Direction
{
	LR, RL
};

struct InData
{
	double A, B, C, yc;
	double hmin, eps;
	Direction direction;
};

void readFile(std::istream& stream, InData& data);

double rungeK2(double x, double y, double h, double (*f)(double, double));

double rungeK3(double x, double y, double h, double (*f)(double, double));

int processFile(std::istream& inStream, std::ostream& outStream, double (*f)(double, double));

void printStep(std::ostream& stream, double x, double y, double localError);
