#include <iostream>
#include <string>
#include <cctype>
#include <cassert>

using namespace std;

int extractNumber(string results, int index);
int indexNumber(string results, int index);
bool isValidResultString(string results);
int positiveTests(string results);
int negativeTests(string results);
int totalTests(string results);
int batches(string results);

int main() {
    string str = "R5+5-0";
    bool huh = isValidResultString(str);
    int totalPos = positiveTests(str);
    int totalNeg = negativeTests(str);
    int total = totalTests(str);
    int numBatches = batches(str);
    
    if (huh == true){
        cout << "heck yeah" << endl;
    }else{
        cout << "oh no" << endl;
    }
    
    cout << "positive tests: " << totalPos << endl;
    cout << "negative tests: " << totalNeg << endl;
    cout << "total tests: " << total << endl;
    cout << "num of batches: " << numBatches << endl;
    
    return 0;
}

//function extracts a number from a string at index given until a character at some index is not a digit
int extractNumber(string results, int index){
    
    //track the integer value found
    int num = 0;
    
    //index cannot go beyond the string
    if (index < results.length()){
        //modified to include 0 in the case that the only digit for a pos/neg test result is 0
        if (results[index] >= '0' && results[index] <='9'){
            while (index < results.length() && isdigit(results[index])){
                // extract one digit and add it to the cumulative value held in num
                int digit = results[index] - '0';
                num = num * 10 + digit;
                index += 1;
            }
        }else{
            return -1;
        }
    }else{
        return -1;
    }
    return num;
}

//function counts the index at which extractNumber ends -- finds index of first non-digit character after the number
int indexNumber(string results, int index){
    //index cannot go beyond the string
    if (index < results.length()){
        //modified to include 0 in the case that the only digit for a pos/neg test result is 0
        if (results[index] >= '0' && results[index] <='9'){
            while (index < results.length() && isdigit(results[index])){
                //index increases by 1 for every digit in a number
                index += 1;
            }
        }else{
            return -1;
        }
    }else{
        return -1;
    }
    return index;
}

bool isValidResultString(string results){
    //i tracks index of whole string while parsing through it
    int i = 0;
    //next_batch is a boolean that tracks whether to continue the do-while loop on the basis that there is another batch of covid information
    bool next_batch = false;
    //correct_batch is a boolean that tracks whether the batches are correctly formatted--if one batch out of n number of batches is incorrect, this will be set to false and the program will exit
    bool correct_batch = false;
    do {
        //checks that first index of batch is equal to R
        if (results[i] == 'R'){
            
            // booleans check that each batch has both a positive and negative case value, and tracks the number for each
            bool has_pos = false;
            bool has_neg = false;
            int num_pos = 0;
            int num_neg = 0;
            
            //extracts number right after first character 'R' which is the total cases
            int total_cases = extractNumber(results, i+1);
            
            //total cases must be greater than 0
            if (total_cases > 0) {
                //j is the index of the first + or - after total cases
                int j = indexNumber(results, i+1) ;
                //boolean tracks whether while loop should continue-as long as true, while loop will execute
                bool cont = true;
                while(cont){
                    //if first digit of a number is 0 and there is a second digit that is greater than or equal to 0, the program exits because of leading zeros
                    if (results[j+1] == '0' && results[j+2] >= '0'){
                        return false;
                    }
                    //number after +/- is extracted
                    int num = extractNumber(results, j+1);
                    //if +, it is indicated that a positive case value exists and tracks that number; if -, it is indicated that a negative case value exists and tracks that number
                    if (results[j] == '+' && num >= 0){
                        num_pos = num;
                        has_pos = true;
                    }else if (results[j] == '-' && num >= 0){
                        num_neg = num;
                        has_neg = true;
                    }else{
                        return false;
                    }
                    //j is now index of second +/-, R, or is out of bounds of string
                    j = indexNumber(results, j+1);
                    //if the character at index is not R and is less than the bounds of string, program still needs to find the second +/-, which means the while loop will execute again. if it is R or out of bounds of string, while loop won't execute.
                    if (results[j] != 'R' && j < results.size()){
                        cont = true;
                    }else{
                        cont = false;
                    }
                }
                //for the found batch, the batch needs to have a positive case value, negative case value, and the number of positive and negative cases must equal the total cases found in the beginning. otherwise, the code will exit.
                if (has_pos == true && has_neg == true && (num_pos + num_neg == total_cases)){
                    correct_batch = true;
                }else{
                    return false;
                }
                //if the string at index j is R, next_batch will be true and the do-while loop will execute once again to find the next batch of results.
                if (results[j] == 'R'){
                    next_batch = true;
                    //set i to the index j so as to make sure the do while for the next batch starts at its beginning value 'R'
                    i = j;
                }else{
                    next_batch = false;
                }
                
            }else{
                return false;
            }
        }else{
            return false;
        }
    }while(next_batch);
    return correct_batch;
}

int positiveTests(string results){
    //track total positive
    int num_pos = 0;
    //makes sure that the string is valid
    if (isValidResultString(results) == true){
        //loop parses throughout entire string and finds where results at index i is the '+' character
        for (int i = 0; i < results.size(); i++){
            if (results[i] == '+'){
                //extracts number at index + 1 (after the '+' character) and adds onto itself in order to make sure the total positive tests is calculated
                num_pos = num_pos + extractNumber(results, i+1);
            }
        }
    }else{
        return -1;
    }
    return num_pos;
}

int negativeTests(string results){
    //track total negative
    int num_neg = 0;
    //makes sure that the string is valid
    if (isValidResultString(results) == true){
        //loop parses throughout entire string and finds where results at index i is the '-' character
        for (int i = 0; i < results.size(); i++){
            if (results[i] == '-'){
                //extracts number at index + 1 (after the '-' character) and adds onto itself in order to make sure the total negative tests is calculated
                num_neg = num_neg + extractNumber(results, i+1);
            }
        }
    }else{
        return -1;
    }
    return num_neg;
}

int totalTests(string results){
    //track total cases
    int total_cases = 0;
    //makes sure that the string is valid
    if (isValidResultString(results) == true){
        //loop parses throughout entire string and finds where results at index i is the 'R' character
        for (int i = 0; i < results.size(); i++){
            if (results[i] == 'R'){
                //extracts number at index + 1 (after the 'R' character) and adds onto itself in order to make sure the total cases is calculated
                total_cases = total_cases + extractNumber(results, i+1);
            }
        }
    }else{
        return -1;
    }
    return total_cases;
}

int batches(string results){
    //track total batches
    int num_batches = 0;
    //makes sure that the string is valid
    if (isValidResultString(results) == true){
        for (int i = 0; i < results.size(); i++){
            //loop parses throughout entire string and finds where results at index i is the 'R' character
            if (results[i] == 'R'){
                //for each 'R', num_batches is added to by 1
                num_batches += 1;
            }
        }
    }else{
        return -1;
    }
    return num_batches;
}
