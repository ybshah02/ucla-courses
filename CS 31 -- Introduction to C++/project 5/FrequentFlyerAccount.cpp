#include "FrequentFlyerAccount.h"

//constructor call that only takes name for paramater-cannot be empty- and sets balance to 0 upon creation
FrequentFlyerAccount::FrequentFlyerAccount(string name){
    if (name != ""){
        mName = name;
    }
    mBalance = 0;
}
//public accessor for name
string FrequentFlyerAccount::getName(){
    return mName;
}
//public accessor for balance
double FrequentFlyerAccount::getBalance(){
    return mBalance;
}

bool FrequentFlyerAccount::addFlightToAccount(PlaneFlight flight){
    //adds balance of flight mileage on condition that passenger name from flight is equal to account name
    //and cost is greater than 0 to ensure that free flights aren't counted in the addition of miles
    if (flight.getName() == mName && flight.getCost() > 0){
        mBalance += flight.getMileage();
        return true;
    }else{
        return false;
    }
}

//checks whether the account balance is greater than the mileage required for flight to earn a free flight
bool FrequentFlyerAccount::canEarnFreeFlight(double mileage){
    if(mBalance >= mileage){
        return true;
    }else{
        return false;
    }
}
//modifies a PlaneFlight object to make the flight a free flight
bool FrequentFlyerAccount::freeFlight(string fromCity, string toCity, double mileage, PlaneFlight& flight){
    //checks whether the mileage required for flight is enough for a free flight by call to canEarnFreeFlight function
    if (canEarnFreeFlight(mileage) && fromCity != "" && toCity != "" && fromCity != toCity){
        //sets name to account name
        flight.setName(mName);
        //sets setToCity to temp to ensure for conditions in which fromCity is changed to toCity and vice versa
        //so that the .set mutator functions are called without error
        flight.setToCity("temp");
        flight.setFromCity(fromCity);
        flight.setToCity(toCity);
        //sets cost to 0
        flight.setCost(0);
        //decreases the account balance by mileage of the flight
        flight.setMileage(mileage);
        mBalance = mBalance - mileage;
        return true;
    }else{
        return false;
    }
}
 
