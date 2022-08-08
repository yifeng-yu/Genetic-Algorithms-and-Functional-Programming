#include "TSP.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
double calculateFitness(const double &lat1, const double &lat2, const double &lon1, const double &lon2){
    
    double dlon = fabs(lon2 - lon1);
    double dlat = fabs(lat2  - lat1);

    double a = pow(sin(dlat/2 * 0.0174533), 2) +
     cos(lat1 * 0.0174533) * cos(lat2 * 0.0174533)  *
    pow(sin(dlon/2 * 0.0174533), 2);
    double c = 2 * atan2(sqrt(a),sqrt(1-a));
    return 3961 * c;
}

double sumVector(const std::vector<double>& inputVector){
    return std::accumulate(inputVector.begin(), inputVector.end(), 0.0,
                                     [](const double&a, const double& b){
        return a+b;
    });
}

bool mySortFunction (std::pair<int, double> p1, std::pair<int, double> p2){
    return p1.second < p2.second;
}

std::vector<double> myTransformFunction(std::vector<double> inputVector, double prob)
{
    std::vector<double> ret;
    std::transform(inputVector.begin(), inputVector.end(), std::back_inserter(ret), [prob](const double& a){
        return a/prob;
    });
    return ret;
}
