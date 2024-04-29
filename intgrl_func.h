#pragma once
#include <fstream>
#include <string>
#include <iomanip>

float f(float x, float y);

enum Direction
{
	LR, RL
};

struct InData
{
	float A, B, C, yc;
	float hmin, eps;
	Direction direction;
};

void readFile(std::istream& stream, InData& data);

float rungeK2(float x, float y, float h);

float rungeK3(float x, float y, float h);

int processFile(std::istream& inStream, std::ostream& outStream);

void printStep(std::ostream& stream, float x, float y, float localError);
