
#include "Set.h"

Set::Set():m_arr(){
    m_size = 0;
}

bool Set::empty() const{
    //size must be 0 for Set to be empty
    if (m_size > 0){
        return false;
    }
    
    return true;
}

int Set::size() const{
    return m_size;
}

bool Set::insert(const ItemType& value){
    //check size isn't at max size or already contains value
    if (m_size == DEFAULT_MAX_ITEMS || contains(value)){
        return false;
    }
    //assign the first element without a null / 0 element with value
    m_arr[m_size] = value;
    //increments size by 1
    m_size++;
    return true;
    
}

bool Set::erase(const ItemType& value){
    //checks if value actually exists in array
    if(contains(value) == false){
        return false;
    }
    
    int i = 0;
    bool cont = true;
    while (cont == true){
        if (m_arr[i] == value){
            cont = false;
        }
        //tracks index of i where value exists
        i++;
    }
    //shifts each element to the left
    for (int j = i - 1; j < m_size-1; j++){
        m_arr[j] = m_arr[j+1];
    }
    //ensures last element after shifting isn't a value to signify a decrease in array size
    if (m_size < DEFAULT_MAX_ITEMS){
        m_arr[m_size] = m_arr[m_size+1];
    }else if (m_size == DEFAULT_MAX_ITEMS){
        ItemType temp_arr[0];
        m_arr[m_size] = temp_arr[0];
    }
    //decrements size by 1
    m_size--;
    return true;
}

bool Set::contains(const ItemType& value) const{
    int i = 0;
    //loops until array contains value or is out of bounds
    while (i < m_size){
        if (m_arr[i] == value){
            return true;
        }
        i++;
    }
    return false;
}

bool Set::get(int i, ItemType& value) const{
    if (i >= 0 && i < size()){
    
        for (int j = 0; j < m_size; j++){
            int num_less = 0;
            for (int k = 0; k < m_size; k++){
                //compares each value with every other value to check how many values is one element less than another
                if (m_arr[j] < m_arr[k]){
                    num_less++;
                }
            }
            //copies value of element
            if (num_less == i){
                value = m_arr[j];
            }
        }
    }else{
        return false;
    }
    return true;
}

void Set::swap(Set& other){
    //gets max size to increment the array over
    int size;
    if (m_size > other.m_size){
        size = m_size;
    }else{
        size = other.m_size;
    }
    //swap algorithm
    for (int i = 0; i < size; i++){
        ItemType temp = m_arr[i];
        m_arr[i] = other.m_arr[i];
        other.m_arr[i] = temp;
    }
    //swaps size of each array for correspondance
    int temp = m_size;
    m_size = other.m_size;
    other.m_size = temp;
}
