#include <iostream>
#include <string>
using namespace std;

int main() {
    
    string month;
    int month_code = 0;
    int day;
    int day_code;
    string day_of_week;
    int year;
    int year_code;
    int century_val;
    int century_code = 0;
    
    //collects user input -- asks for month, day, and year in a specified format, explained below
    cout << "Provide a month: ";
    //getline is used to make sure extra spaces and characters/words after the initial word aren't ignored and cause the program to potentially break
    getline(cin,month);
    
    //month must be of specified format -- must start with a capital letter and have no additional spaces before or after the month name. if the month isn't one of the 12 specified months listed, then 'Invalid month!" is printed to the console.
    if (month.compare("January") != 0 && month.compare("February") != 0 && month.compare("March") != 0 && month.compare("April") != 0 && month.compare("May") != 0 && month.compare("June") != 0 && month.compare("July") != 0 && month.compare("August") != 0 && month.compare("September") != 0 && month.compare("October") != 0 && month.compare("November") != 0 && month.compare("December") != 0){
        
        cout << "Invalid month!" << endl;
        return 0;
    }
    
    cout << "Provide a day: ";
    cin >> day;
    
    //this makes sure that the inputted day is a valid day between 1 and 31. if the day is outside those bounds, then 'Invalid day!' is printed to the console.
    if (day < 1 || day > 31){
        cout << "Invalid day!" << endl;
        return 0;
    }

    cout << "Provide a year: ";
    cin >> year;
    
    //this makes sure that the inputted year is a valid year between 1 and 3000. if the year is outside those bounds, then 'Invalid year!' is printed to the console.
    if (year < 1 || year > 3000){
        cout << "Invalid year!" << endl;
        return 0;
    }
    
    // if there are no errors on the inputs of the month, day, and year, then the month code is found based off the month inputted.
    if (month.compare("January") == 0 || month.compare("October") == 0){
        month_code = 1;
    }else if(month.compare("February") == 0 || month.compare("March") == 0 || month.compare("November") == 0){
        month_code = 4;
    }else if(month.compare("April") == 0 || month.compare("July") == 0){
        month_code = 0;
    }else if(month.compare("May") == 0){
        month_code = 2;
    }else if(month.compare("June") == 0){
        month_code = 5;
    }else if(month.compare("August") == 0){
        month_code = 3;
    }else if(month.compare("September") == 0 || month.compare("December") == 0){
        month_code = 6;
    }
    
    //the month code is changed by -1 if the year inputted is a leap year-- that is, if the year is divisble by 4 and not evenly divisble by 100, with the exception of being evenly divisble by 400. The first part of the if statement checks to make sure if the year is divisble by 4, it shouldn't be divisible by 100. If one of these isn't true, then the second part of the if statement makes sure that if the year is divisible by 100, then the year should be divisible by 400 as well.
    
    if ((month.compare("January") == 0 || month.compare("February") == 0) && ((year % 4 == 0 && year % 100 != 0 ) || (year % 400 == 0 && year % 100 == 0))){
        month_code -= 1;
    }
    
    // we then have to calculate the century value based on a mathematical formula - year value entered by user divided by 100 modulo 4. Using the century value, we can then assign the correct century code
    century_val = int (((year / 100) % 4));
    if (century_val == 0){
        century_code = -2;
    }else if(century_val == 1){
        century_code = 3;
    }else if(century_val == 2){
        century_code = 1;
    }else if(century_val == 3){
        century_code = -1;
    }
    
    // we then are able to calculate the year code given the formula year code = year value inputted modulo 100 divided by 4 plus the year value inputted modulo 100
    year_code = ((year % 100) / 4) + (year % 100);
    
    // we can then finally calculate the day code based off the elaborate formula (century code + day inputted + year code + month code) module 7
    day_code = (century_code + day + year_code + month_code) % 7;
    
    //after we find the day, we have to correlate the day code to the correct day of the week
    if (day_code == 0){
        day_of_week = "Sunday";
    }else if (day_code == 1 || day_code == -6){
        day_of_week = "Monday";
    }else if (day_code == 2 || day_code == -5){
        day_of_week = "Tuesday";
    }else if (day_code == 3 || day_code == -4){
        day_of_week = "Wednesday";
    }else if (day_code == 4 || day_code == -3){
        day_of_week = "Thursday";
    }else if (day_code == 5 || day_code == -2){
        day_of_week = "Friday";
    }else if (day_code == 6 || day_code == -1){
        day_of_week = "Saturday";
    }
    
    // prints the final message with the date originally inputted by the user and the day of the week the day falls on based on the calculations done above
    cout << month << " " << day << ", " << year << " was a " << day_of_week << "!" << endl;
    
    return 0;
}
