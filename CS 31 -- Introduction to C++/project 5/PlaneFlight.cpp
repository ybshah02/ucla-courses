#include "PlaneFlight.h"

PlaneFlight::PlaneFlight(string passengerName, string fromCity, string toCity, double cost, double mileage) {
    //passenger name cannot be empty
    if (passengerName != ""){
        mName = passengerName;
    }
    //cost must be greater than equal to 0
    if (cost >= 0){
        mCost = cost;
    }else{
        mCost = -1;
    }
    //fromCity and toCity cannot be empty and not be equal to each other
    if (fromCity != "" && toCity != "" && fromCity != toCity){
        mFromCity = fromCity;
        mToCity = toCity;
    }
    //mileage must be greater than 0
    if (mileage > 0){
        mMileage = mileage;
    }else{
        mMileage = -1;
    }
    
}
//public accessor for passenger name
string PlaneFlight::getName(){
    return mName;
}
//mutator for name on condition paramater isn't a empty string
void PlaneFlight::setName(string passengerName){
    if (passengerName != ""){
        mName = passengerName;
    }
}
//public accessor for cost
double PlaneFlight::getCost(){
    return mCost;
}
//mutator for cost on condition parameter is a cost greater than equal to 0
void PlaneFlight::setCost(double cost){
    if (cost >= 0){
        mCost = cost;
    }else{
        mCost = -1;
    }
}
//public accessor for fromCity
string PlaneFlight::getFromCity(){
    return mFromCity;
}
//mutator for fromCity on condition parameter isn't empty and not equal to toCity
void PlaneFlight::setFromCity(string fromCity){
    if (fromCity != "" && fromCity != mToCity){
        mFromCity = fromCity;
    }
}
//public accessor for toCity
string PlaneFlight::getToCity(){
    return mToCity;
}
//mutator for toCity on condition parameter isn't empty and not equal to fromCity
void PlaneFlight::setToCity(string toCity){
    if (toCity != "" && toCity != mFromCity){
        mToCity = toCity;
    }
}
//public accessor for mileage
double PlaneFlight::getMileage(){
    return mMileage;
}
//mutator for mileage on condition mileage parameter is greater than 0
void PlaneFlight::setMileage(double mileage){
    if (mileage > 0){
        mMileage = mileage;
    }else{
        mMileage = -1;
    }
}
