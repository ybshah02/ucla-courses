#ifndef SET_INCLUDED
#define SET_INCLUDED

#include <iostream>
#include <string>

using ItemType = std::string;

class Set
{
  public:
    //constructor
    Set();
    //destructor
    ~Set();
    //copy constructor
    Set(const Set& other);
    //assignment operator
    Set& operator=(const Set& shs);
    
    bool empty() const;
    int size() const;
    bool insert(const ItemType& value);
    bool erase(const ItemType& value);
    bool contains(const ItemType& value) const;
    bool get(int pos, ItemType& value) const;
    void swap(Set& other);
    
    struct Node{
        ItemType data;
        Node *next;
        Node *prev;
    };
    
private:
    Node *head;
    Node *tail;
    int m_size;
};

void unite(const Set& s1, const Set& s2, Set& result);
void difference(const Set& s1, const Set& s2, Set& result);

#endif /* Set_h */
