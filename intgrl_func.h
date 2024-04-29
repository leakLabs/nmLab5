#pragma once
#include <fstream>
#include <string>
#include <iomanip>
#include <iostream>

double f(double x, double y);

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

double rungeK2(double x, double y, double h);

double rungeK3(double x, double y, double h);

int processFile(std::istream& inStream, std::ostream& outStream);

void printStep(std::ostream& stream, double x, double y, double localError);
