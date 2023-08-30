
#include "newSet.h"

Set::Set():m_size_max(DEFAULT_MAX_ITEMS),m_size(0){
    //initialize array
    m_arr = new ItemType[DEFAULT_MAX_ITEMS];
}

Set::Set(int size):m_size(0){
    //initialize array
    m_arr = new ItemType[DEFAULT_MAX_ITEMS];
    if(size > 0){
        m_size_max = size;
    }
}

Set::Set(const Set& other):m_size(other.m_size), m_size_max(other.m_size_max){
    //initialize array
    m_arr = new ItemType[DEFAULT_MAX_ITEMS];
    //add each element of array other to m_array
    for (int i = 0; i < m_size_max; i++){
        m_arr[i] = other.m_arr[i];
    }
}

Set& Set::operator=(const Set& other){
    //if same array, return pointer to this array
    if (&other == this){
        return *this;
    }
    delete [] m_arr;
    //initialize array and it's size and capacity
    m_arr = new ItemType[m_size_max];
    m_size = other.m_size;
    m_size_max = other.m_size_max;
    //add each element of array other to m_array
    for (int i = 0; i < m_size_max; i++){
        m_arr[i] = other.m_arr[i];
    }
    return *this;
}
//destructor to delete array
Set::~Set(){
    delete [] m_arr;
}

bool Set::empty() const{

    if (m_size > 0){
        return false;
    }
    
    return true;
}

int Set::size() const{
    return m_size;
}

bool Set::insert(const ItemType& value){
    
    if (m_size == m_size_max || contains(value)){
        return false;
    }
    
    m_arr[m_size] = value;
    m_size++;
    return true;
    
}

bool Set::erase(const ItemType& value){
    
    if(contains(value) == false){
        return false;
    }
    
    int i = 0;
    bool cont = true;
    while (cont == true){
        if (m_arr[i] == value){
            cont = false;
        }
        i++;
    }
    
    for (int j = i - 1; j < m_size-1; j++){
        m_arr[j] = m_arr[j+1];
    }
    
    if (m_size < DEFAULT_MAX_ITEMS){
        m_arr[m_size] = m_arr[m_size+1];
    }else if (m_size == DEFAULT_MAX_ITEMS){
        ItemType temp_arr[0];
        m_arr[m_size] = temp_arr[0];
    }
    
    m_size--;
    return true;
}

bool Set::contains(const ItemType& value) const{
    int i = 0;
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
                if (m_arr[j] < m_arr[k]){
                    num_less++;
                }
            }
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
    //swaps content of array
    ItemType* temp_arr = m_arr;
    m_arr = other.m_arr;
    other.m_arr = temp_arr;
    //swaps sizes
    int temp_size = m_size;
    m_size = other.m_size;
    other.m_size = temp_size;
    //swaps capacity of array
    int temp_size_max = m_size_max;
    m_size_max = other.m_size_max;
    other.m_size_max = temp_size_max;
}


