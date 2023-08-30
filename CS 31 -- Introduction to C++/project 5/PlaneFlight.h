#ifndef PlaneFlight_h
#define PlaneFlight_h

#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

class PlaneFlight {
private:
    string mName;
    double mCost;
    string mFromCity;
    string mToCity;
    double mMileage;

public:
    
    PlaneFlight(string passengerName, string fromCity, string toCity, double cost, double mileage);

    string getName();
    
    void setName(string passengerName);
    
    double getCost();
    
    void setCost(double cost);
    
    string getFromCity();
    
    void setFromCity(string fromCity);
    
    string getToCity();
    
    void setToCity(string toCity);
    
    double getMileage();
    
    void setMileage(double mileage);
    
};

#endif
