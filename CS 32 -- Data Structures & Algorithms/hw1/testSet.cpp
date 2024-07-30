
#include "Set.h"
#include <iostream>
#include <string>
#include <cassert>

int main(){
    Set ss;
        ss.insert("lavash");
        ss.insert("roti");
        ss.insert("chapati");
        ss.insert("injera");
        ss.insert("roti");
        ss.insert("matzo");
        ss.insert("injera");
        assert(ss.size() == 5);  // duplicate "roti" and "injera" were not added
        std::string x;
        ss.get(0, x);
        assert(x == "roti");  // "roti" is less than exactly 0 items in ss
        ss.get(4, x);
        assert(x == "chapati");  // "chapati" is less than exactly 4 items in ss
        ss.get(2, x);
        assert(x == "lavash");  // "lavash" is less than exactly 2 items in ss
    
    Set ss2;
    
    ss2.insert("hi");
    ss2.insert("my");
    ss2.insert("name");
    ss2.insert("is");
    ss2.insert("yash");
    ss2.insert("shah");
    ss2.insert("and");
    ss2.insert("I");
    ss2.insert("love");
    ss2.insert("cs32");
    
    ss.swap(ss2);
     
}
