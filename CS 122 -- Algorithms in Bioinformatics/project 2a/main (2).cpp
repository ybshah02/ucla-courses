//
//  main.cpp
//  Project3
//
//  Created by Sari Abu-Hamad on 5/22/23.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <unordered_map>
#include <map>
#include <fstream>
#include <random>
#include <list>
using namespace std;

string combinePath(vector<string> pathItems){
    int length = (int)pathItems[0].length();
    string resultPath = pathItems[0];
    for (int index = 1; index < pathItems.size(); index++){
        resultPath += pathItems[index][length-1];
    }
    return resultPath;
}

vector<string> createCycleFromPath(map<string,vector<string>> inputGraph, string startNode){
    vector<string> cycleItems;
    vector<string> remainingEdges = {startNode};
    while (!remainingEdges.empty()){
        string currentNode = remainingEdges.back();
        if (!inputGraph[currentNode].empty()){
            string edge = inputGraph[currentNode].back();
            inputGraph[currentNode].pop_back();
            remainingEdges.push_back(edge);
        }
        else{
            cycleItems.push_back(remainingEdges.back());
            remainingEdges.pop_back();
        }
    }
    reverse(cycleItems.begin(), cycleItems.end());
    cycleItems.pop_back();
    return cycleItems;
}

vector<string> createPath(map<string,vector<string>> inputGraph){
    string endNode = "";
    for (auto itr = inputGraph.begin(); itr != inputGraph.end(); itr++){
        for (int index = 0; index < itr->second.size(); index++){
            if (inputGraph.find(itr->second[index]) == inputGraph.end())
                endNode = itr->second[index];
        }
    }
    string startNode = "";
    for (auto itr = inputGraph.begin(); itr != inputGraph.end(); itr++){
        int countElements = 0;
        string element = itr->first;
        for (auto itr2 = inputGraph.begin(); itr2 != inputGraph.end(); itr2++){
            for (int index = 0; index < itr2->second.size(); index++){
                if (itr2->second[index] == element)
                    countElements++;
            }
        }
        if (itr->second.size() > countElements)
            startNode = itr->first;
    }
    inputGraph[endNode].push_back(startNode);
    return createCycleFromPath(inputGraph, startNode);
}

string reconstructString(vector<string> patternList, int k){
    int l = k - 1;
    map<string,multiset<string>> deBruijnGraph;
    for (auto pattern : patternList){
        deBruijnGraph[pattern.substr(0,l)].insert(pattern.substr(1,l));
    }
    map<string,vector<string>> convertedGraph;
    for (auto itr = deBruijnGraph.begin(); itr != deBruijnGraph.end(); itr++){
        for (auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++){
            convertedGraph[itr->first].push_back(*itr2);
        }
    }
    vector<string> ePath = createPath(convertedGraph);
    return combinePath(ePath);
}

int findPattern(vector<string> patternList, string kmer){
    for (int index = 0; index < patternList.size(); index++){
        if (patternList[index] == kmer)
            return index;
    }
    return -1;
}

int main() {
    ifstream inputFileStream("project3a_10000_spectrum.fasta");
    string currentLine;
    vector<string> spectrumItems;
    while (getline(inputFileStream,currentLine)){
        if (currentLine[0] == '>')
            continue;
        spectrumItems.push_back(currentLine);
    }
    int k = (int)spectrumItems[0].length();
    string reconstructedGenome = reconstructString(spectrumItems, k);
    map<int,int> readPositionsMap;
    for (int index = 0; index < reconstructedGenome.length()-k+1; index++){
        string genomeWindow = reconstructedGenome.substr(index,k);
        int readNumber = findPattern(spectrumItems, genomeWindow);
        if (readNumber == -1){
            continue;
        }
        readPositionsMap[index] = readNumber;
    }
    for (auto itr = readPositionsMap.begin(); itr != readPositionsMap.end(); itr++){
        cout << ">read_" << itr->second << endl;
    }
    return 0;
}
