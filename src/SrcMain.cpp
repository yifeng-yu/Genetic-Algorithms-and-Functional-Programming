#include "SrcMain.h"
#include <iostream>
#include <random>
#include "TSP.h"
#include <fstream>
#include <algorithm>
#include <string>

// Each pair has an integer corresponding to the individual in the population
// and a double that contains the fitness of that member.
// A pair is used so that when you later sort by fitness, you’ll still know which member of the population it corresponds to.

void ProcessCommandArgs(int argc, const char* argv[])
{

	// TODO
    std::string inputFile = argv[1];
    int popSize = atoi(argv[2]);
    int generation = atoi(argv[3]);
    int mutationChance = atoi(argv[4]);
    int seed = atoi(argv[5]);
    
    // parse file
    std::ifstream file(inputFile);
    std::string line;
    std::vector<std::string> tempLoc;
    int numLocations = 0;
    while(true){
        std::getline(file,line);
        if(file.fail()){
            break;
        }
        const char delim = ',';
        size_t start = 0;
        size_t delimLoc = line.find_first_of(delim,start);
        while(delimLoc != std::string::npos){
            tempLoc.emplace_back(line.substr(start,delimLoc - start));
            start = delimLoc+1;
            delimLoc = line.find_first_of(delim,start);
        }
        tempLoc.emplace_back(line.substr(start));
        numLocations++;
    }
    
    int size = tempLoc.size();
    std::vector<Location> realLocation;
    realLocation.reserve(numLocations);
    Location l;
    int i = 0;
    while( i < size){
        l.mName = tempLoc[i];
        l.mLatitude = stod(tempLoc[i+1]);
        l.mLongitude = stod(tempLoc[i+2]);
        realLocation.emplace_back(l);
        i = i + 3;
    }
    std::mt19937 mt1(seed);
    std::mt19937 &mt2 = mt1;
    std::ofstream outputFile("log.txt");
    std::vector<std::vector<int>> initialPop(popSize);
    
    for(size_t i = 0; i < generation + 1; i++){
        
        std::vector<int> popIndex;
        popIndex.reserve(numLocations);
        // push in 0~numLocations if it is initial pop
        if(i == 0){
            for(int i = 0; i < numLocations; i++){
                popIndex.emplace_back(i);
            }
            // shuffle and push again to initial population which is a 2d vector
            for(int i = 0; i < popSize; i++){
                std::vector<int> temp = popIndex;
                std::shuffle(temp.begin() + 1,temp.end(), mt2);
                initialPop[i] = temp;
            }
            outputFile << "INITIAL POPULATION:" << std::endl;
        }
        else {
            outputFile << "GENERATION: " << i << std::endl;
        }
        for(size_t i = 0; i < popSize; i++){
            for (size_t j = 0; j < numLocations - 1; j++){
                outputFile << initialPop[i][j] << ",";
            }
            outputFile << initialPop[i][numLocations-1] << std::endl;
        }
        // calculate each fitness
        std::vector<std::pair<int,double>> fitnessVector;
        fitnessVector.reserve(popSize);
        for (int i = 0; i < popSize; i++){
            std::vector<int> temp = initialPop[i];
            std::vector<double> sumV;
            for (int j = 0; j + 1 < temp.size(); j++){
                Location templ1 = realLocation[temp[j]];
                Location templ2 = realLocation[temp[j+1]];
                sumV.emplace_back(calculateFitness(templ1.mLatitude, templ2.mLatitude, templ1.mLongitude, templ2.mLongitude));
            }
            sumV.emplace_back(calculateFitness(realLocation[temp[numLocations-1]].mLatitude, realLocation[temp[0]].mLatitude, realLocation[temp[numLocations-1]].mLongitude, realLocation[0].mLongitude));
            fitnessVector.emplace_back(std::make_pair(i, sumVector(sumV)));
        }
        
        // output file
        outputFile << "FITNESS:" << std::endl;
        for(int i = 0; i < popSize; i++){
            outputFile << fitnessVector[i].first << ":" << fitnessVector[i].second << std::endl;
        }
        
        
        std::sort(fitnessVector.begin(), fitnessVector.end(), mySortFunction);
        std::vector<double> probVector(popSize);
        std::generate(probVector.begin(), probVector.end(), [popSize]{
            return 1.0/popSize;
        });
        probVector[fitnessVector[0].first] *= 6.0;
        probVector[fitnessVector[1].first] *= 6.0;
        for(int i = 2; i < fitnessVector.size()/2; i++){
            probVector[fitnessVector[i].first] *= 3.0;
        }
        double sum = std::accumulate(probVector.begin(), probVector.end(), 0.0,
                                         [](const double&a, const double& b){
            return a+b;
        });
        std::vector<double> normProbVector = myTransformFunction(probVector, sum);
        // output file
    //    Myfile << "Selection Probabilities:" << std::endl;
    //    for(int i = 0; i < popSize; i++){
    //        Myfile << i << ":" << normProbVector[i] << std::endl;
    //    }

        if(i == generation){
            std::pair<int, double> res = *std::min_element(fitnessVector.cbegin(), fitnessVector.cend(), [](const auto& lhs, const auto& rhs) {
                    return lhs.second < rhs.second;
                });
            std::vector<int> output = initialPop[res.first];
            output.push_back(0);
            outputFile << "SOLUTION: " << std::endl;
            for(int i = 0; i < output.size(); i++){
                outputFile << realLocation[output[i]].mName << std::endl;
            }
            outputFile << "DISTANCE: " << res.second << " miles" << std::endl;
            break;
        }
        std::vector<std::pair<int,int>> selectingPairs;
        
        for(int k = 0; k < popSize; k++){
            std::uniform_real_distribution<double> urd(0.0,1.0);
            double randDouble1 = urd(mt2);
            double randDouble2 = urd(mt2);
            
            double currRange = 0.0;
            int firstIndex = 0;
            int secIndex = 0;
            int find1 = 0;
            int find2 = 0;
            for (size_t i = 0; i < popSize; i++){
                currRange += normProbVector[i];
                if((randDouble1 < currRange) && find1 == 0){
                    firstIndex = i;
                    find1 = 1;
                }
                if((randDouble2 < currRange) && find2 == 0){
                    secIndex = i;
                    find2 = 1;
                }

            }
    //        Myfile << "Random Real Pair: ( " << randDouble1 << " " << randDouble2 << " )" << std::endl;
            selectingPairs.emplace_back(std::make_pair(firstIndex, secIndex));
        }
        outputFile << "SELECTED PAIRS:" << std::endl;
        for(int i = 0; i < popSize; i++){
            outputFile << "(" << selectingPairs[i].first << "," << selectingPairs[i].second << ")" << std::endl;
        }
        

       
        // crossover and mutation
        std::vector<std::vector<int>> newGenerationPop(popSize);
        for(int i = 0; i < selectingPairs.size(); i++){
            std::uniform_int_distribution<int> crossIndexPop(1, numLocations - 2);
            int crossIndex = 0;
            crossIndex = crossIndexPop(mt2);
            std::uniform_int_distribution<int> goFirstPop(0,1);
            int goFirst = 0;
            goFirst = goFirstPop(mt2);
            // 1 -> A first
            // 0 -> B first
            std::vector<int> firstP;
            std::vector<int> secondP;
            if(goFirst == 1){
                firstP = initialPop[selectingPairs[i].first];
                secondP = initialPop[selectingPairs[i].second];
            }
            else if (goFirst == 0){
                firstP = initialPop[selectingPairs[i].second];
                secondP = initialPop[selectingPairs[i].first];
            }
            std::vector<int> child(crossIndex+1);
            std::copy_n(firstP.begin(), crossIndex+1, child.begin());
            std::copy_if(secondP.begin(), secondP.end(), std::back_inserter(child), [&](const int& element){
                bool result = std::find(child.begin(), child.end(), element) == child.end();
                return result;
            });
            std::uniform_real_distribution<double> mutationRatePop;
            double mutationRate = mutationRatePop(mt2);
            if(mutationRate < mutationChance/100.0){
                std::uniform_int_distribution<int> swapPop(1,numLocations-1);
                int swapIndexA = swapPop(mt2);
                int swapIndexB = swapPop(mt2);
                std::swap(child[swapIndexA], child[swapIndexB]);
            }
            newGenerationPop[i] = child;
        
        }
        initialPop = newGenerationPop;
    }
    
    
    
}
