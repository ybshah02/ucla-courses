
#include <iostream>
#include "AccountSet.h"

int main(){
    AccountSet acc_set;
    acc_set.add(56542);
    acc_set.add(12334);
    acc_set.add(63578);
    acc_set.add(65348);
    acc_set.add(35723);
    acc_set.add(87434);
    
    std::cout << acc_set.size() << std::endl;
    acc_set.print();
    
}

