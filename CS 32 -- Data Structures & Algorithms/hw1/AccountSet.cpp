
#include "AccountSet.h"

AccountSet::AccountSet():m_set(){}  

bool AccountSet::add(unsigned long acctNum){
    //functionality of Set class
    return m_set.insert(acctNum);
}


int AccountSet::size() const{
    //functionality of Set class
    return m_set.size();
}


void AccountSet::print() const{
    unsigned long val;
    for (int i = 0; i < size(); i++){
        m_set.get(i,val);
        //prints in order of largest to smallest because val size is 0
        std::cout << val << std::endl;
    }
}


