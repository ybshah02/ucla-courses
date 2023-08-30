#include <iostream>
#include <string>
using namespace std;

int locateMaximum(const string array[], int n);
int countFloatingPointValues(const string array[], int n);
bool hasNoCapitals( const string array[ ], int n );
bool identicalValuesTogether(const string array[], int n);
bool hasTwoOrMoreDuplicates(const string array[], int n);
int shiftLeft(string array[], int n, int amount, string placeholder);
int replaceFirstAndLastOccurrences(string array[], int n, char charToFind, char charToReplace);

int main() {
    return 0;
}

int locateMaximum(const string array[], int n){
    int max_index;
    string max_num;
    if (n > 0){
        max_num = "";
        max_index = 0;
        //loop compares each value with > operator to determine relative max value
        for (int i = 0; i < n; i++){
            if(array[i] > max_num){
                //set new max
                max_num = array[i];
                //set index of the max value
                max_index = i;
            }
        }
    }else{
        return -1;
    }
    
    return max_index;
}

int countFloatingPointValues(const string array[], int n){
    int count;
    if(n > 0){
        count = 0;
        //loops through each element in the array
        for(int i=0; i < n; i++){
            //sets array of index i to string elem
            string elem = array[i];
            //checks elem doesn't start with + or -
            if (elem[0] != '+' && elem[0] != '-'){
                //tracks count of periods -- float can only have 1
                int prd_count = 0;
                //bool to track if element is valid
                bool num_valid = false;
                //loops through each character in elem
                for (int j=0; j < elem.length(); j++){
                    //checks to make sure that each character in elem is either a digit, or is a period of which prd count is less than 1
                    if(isdigit(elem[j]) || (elem[j] == '.' && prd_count < 1)){
                        num_valid = true;
                        if (elem[j] == '.'){
                            prd_count += 1;
                        }
                    }else{
                        num_valid = false;
                    }
                }
                if (num_valid == true){
                    count+=1;
                }
            }
        }
    }else{
        return -1;
    }
    return count;
}

bool hasNoCapitals( const string array[], int n ){
    bool has_capital;
    if (n > 0){
        has_capital = true;
        //loops through each element of string
        for (int i = 0; i < n; i++){
            int count_capital = 0;
            //sets array of index i to string elem
            string elem = array[i];
            //loops through each character in elem
            for(int j = 0; j < elem.size(); j++){
                //checks whether there is a capital in this element - adds one to counter if there is
                if (isupper(elem[j])){
                    count_capital += 1;
                }
            }
            //if at least one word has a capital, then we default to return false. if not, then has_capital doesn't have a capital
            if(count_capital > 0){
                return false;
            }else{
                has_capital = false;
            }
        }
    }else{
        return true;
    }
    //if no element has capital, then hasNoCapitals is true. otherwise false
    if (has_capital == false){
        return true;
    }else{
        return false;
    }
}

bool identicalValuesTogether(const string array[], int n){
    bool has_identical_vals;
    int count_identical_vals;
    if (n > 0){
        has_identical_vals = true;
        count_identical_vals = 0;
        //loops through each element of array -- n-1 because we use j = i + 1, in which there would be an out of bounds mistake
        for(int i = 0; i < n-1; i++){
            int j = i + 1;
            //checks whether element and element one index after is equal to each other -- hence identical vals
            if(array[i] == array[j]){
                count_identical_vals+=1;
            }
            //loops through each character of array
            for(int k = 0; k < n; k++){
                //checks if there are other identical values which are not located next to each other by validating if each element through indexes i and k is equal to one another
                if (array[i] == array[k] && i != k){
                    if (i > k){
                        for(int m = k; m < i-1; m++){
                            if(array[m] != array[m+1]){
                                return false;
                            }
                        }
                    }else if(i < k){
                        for(int n = i; n < k-1; n++){
                            if(array[n] != array[n+1]){
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
    else{
        return false;
    }
    if(count_identical_vals >= 0){
        has_identical_vals = true;
    }else{
        has_identical_vals = false;
    }
    
    return has_identical_vals;
}

bool hasTwoOrMoreDuplicates(const string array[], int n){
    bool has_duplicates;
    int count_pair;
    if(n > 0){
        has_duplicates = false;
        //counts number of duplicates
        count_pair = 0;
        //stores the last character that was checked for duplicated values
        string last_char = "";
        //loops through each element in array
        for(int i = 0; i < n; i++){
            int count_elem = 0;
            //checks to make sure character we are inspecting isn't the same as the character that was just inspected
            if(array[i] != last_char){
                for (int j = 0; j < n; j++){
                    if (i != j && array[i] == array[j]){
                        count_elem +=1;
                    }
                }
                if(count_elem == 1){
                    count_pair += 1;
                }else if(count_elem >= 2){
                    return true;
                }
            }
            if (count_elem > 0){
                last_char = array[i];
            }
        }
    }else{
        return false;
    }
    
    if (count_pair >= 2){
        has_duplicates = true;
    }else if (count_pair < 2){
        has_duplicates = false;
    }
    
    return has_duplicates;
}

int shiftLeft(string array[], int n, int amount, string placeholder){
    int num_placeholder;
    if(n > 0 && amount >= 0){
        num_placeholder = 0;
        if (amount > 0 && amount < n){
            for (int i = 0; i < n-amount; i++){
                string temp = array[i+amount];
                array[i] = temp;
            }
            for (int j = 0; j < amount; j++){
                array[n-j-1] = placeholder;
                num_placeholder +=1;
            }
        }else if(amount >= n){
            for(int k = 0; k < n; k++){
                array[k] = placeholder;
                num_placeholder+=1;
            }
        }
    }else{
        return -1;
    }
    
    return num_placeholder;
}

int replaceFirstAndLastOccurrences(string array[], int n, char charToFind, char charToReplace){
    int count;
    if(n > 0){
        count = 0;
        for(int i = 0; i < n; i++){
            string elem = array[i];
            int count_total = 0;
            int count_first = 0;
            int count_last = 0;
            for(int j = 0; j < elem.size(); j++){
                if (elem[j] == charToFind){
                    count_total+=1;
                }
                
                if (elem[j] == charToFind && count_first < 1){
                    elem[j] = charToReplace;
                    count_first+=1;
                    count+=1;
                }
            }
            if(count_total > 1){
                for(long k = elem.size()-1; k >= 0; k--){
                    if (elem[k] == charToFind && count_last < 1){
                        elem[k] = charToReplace;
                        count_last+=1;
                        count+=1;
                    }
                }
            }
            array[i] = elem;
        }
    }else{
        return -1;
    }
    return count;
}
