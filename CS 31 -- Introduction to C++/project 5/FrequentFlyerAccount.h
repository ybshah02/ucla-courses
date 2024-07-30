
#ifndef FrequentFlyerAccount_h
#define FrequentFlyerAccount_h

#include <stdio.h>
#include <iostream>
#include <string>
#include "PlaneFlight.h"

using namespace std;

class FrequentFlyerAccount {
private:
    string mName;
    double mBalance;
public:
    FrequentFlyerAccount(string name);
    
    string getName();
    
    double getBalance();
    
    bool addFlightToAccount(PlaneFlight flight);
    
    bool canEarnFreeFlight(double mileage);
    
    bool freeFlight(string fromCity, string toCity, double mileage, PlaneFlight& flight);
};

#endif
