#pragma once
#include <string>
#include <vector>
#include <random>

struct Location
{
	std::string mName;
	double mLatitude = 0.0;
	double mLongitude = 0.0;
};

struct Population
{
	std::vector<std::vector<int>> mMembers;
};

double calculateFitness(const double &lat1, const double &lat2, const double &lon1, const double &lon2);

double sumVector(const std::vector<double>& inputVector);

bool mySortFunction (std::pair<int, double> p1, std::pair<int, double> p2);

std::vector<double> myTransformFunction(std::vector<double> inputVector, double prob);
