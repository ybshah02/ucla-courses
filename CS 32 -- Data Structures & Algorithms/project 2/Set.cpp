#include "Set.h"


Set::Set(): m_size(0){
    //initialize linked list
    head = nullptr;
    tail = nullptr;
}

Set::~Set(){
    //create node pointer and set to head of linked list
    Node *p = head;
    //loop until p hits a nullptr
    while (p != nullptr){
        //create temp val that holds next p value
        Node *nextP = p->next;
        //delete current p val
        delete p;
        //set p to the next element of the linked list
        p = nextP;
    }
}


Set::Set(const Set& other){
    //check if other head and tail is empty
    if (other.head == nullptr && other.tail == nullptr){
        //set this object's head and tails to null ptrs as well
        this->head = nullptr;
        this->tail = nullptr;
    }else{
        this->head = new Node();
        //sets this object's head data value to other object's
        this->head->data = other.head->data;
        //set previous and next nodes to null ptrs
        this->head->prev = nullptr;
        this->head->next = nullptr;
        //set this object's tail data value to the head value
        this->tail = this->head;
        //set new node to this's head, and another node to other's head
        Node *p = this->head;
        Node *q = other.head;
        //iterate through
        while (q->next != nullptr){
            p->next = new Node();
            //set next this node's data to next other's node
            p->next->data = q->next->data;
            //set prev field of next node that follows p to p
            p->next->prev = p;
            //set next field of next node that follows p to nullptr
            p->next->next = nullptr;
            //increment
            p = p->next;
            q = q->next;
            //set tail to p node
            tail = p;
        }
    }
    //set size of this size to other's size
    this->m_size = other.m_size;
    
}


Set& Set::operator=(const Set& rhs){
    //check if this is equal to inputted Set object
    if (this != &rhs){
        //create temp value to swap values with and not change rhs's vals
        Set temp(rhs);
        swap(temp);
    }
    return *this;
}


bool Set::empty() const{
    //returns whether m_size is empty (0)
    if (size() == 0){
        return true;
    }else
        return false;
}

int Set::size() const{
    //return value of m_size
    return m_size;
    
}

bool Set::insert(const ItemType& value){
    //no repeated values
    if(contains(value))
        return false;
    //create new node object
    Node *p = new Node;
    //assign its value
    p->data = value;
    //empty list case
    if (empty()){
        //next and prev node set to null to insert value at the front
        p->next = nullptr;
        p->prev = nullptr;
        //insert element - head = tail in case of only one element
        head = p;
        tail = p;
    // >0 elements in list case
    }else{
        //item after p is set to head
        p->next = head;
        //previous head is set to null
        p->prev = nullptr;
        //previous head is set to point to p
        head->prev = p;
        //value of head is set to value of p
        head = p;
    }
    //increment size of array after insert
    m_size++;
    return true;
}

bool Set::erase(const ItemType& value){
    //if empty list or value doesn't exist in list, no need for erase functionality
    if (empty() || !contains(value))
        return false;
    
    //set node to begin at head
    Node *p = head;
    //if head is equal to value
    if (p->data == value){
        //set new head as next head
        head = head->next;
        delete head->prev;
        //decrement size of list
        m_size--;
        return true;
    //if tail is equal to value
    } else if (tail->data == value){
        //set new tail as previous tail
        tail = tail->prev;
        delete tail->next;
        //decrement size of list
        m_size--;
        return true;
    }
    //if value is in middle of list, increment through list
    while (p->next != nullptr){
        //if next node's data is equal to value
        if (p->next->data == value){
            //set next to next node as current next node
            p->next = p->next->next;
            delete p->next->prev;
            //decrement size of list
            m_size--;
            return true;
        }
        //increment through list
        p = p->next;
    }
    return false;
}

bool Set::contains(const ItemType& value) const{
    //check if list is empty
    if (empty())
        return false;
    //set node at beginning of list (head)
    Node *p = head;
    //iterate throughout list until p is a null ptr
    while (p != nullptr){
        //if value == node data value, return true
        if (p->data == value){
            return true;
        }
        //increment
        p = p->next;
    }
    //return false if no such value is found and already returned
    return false;
    
}

bool Set::get(int pos, ItemType& value) const{

    //checks whether list is empty and pos is in reasonable bounds
    if (!empty() && pos >= 0 && pos < size()){
    
        //have node at beginning of list
        Node *p = head;
        //iterate through the entire list
        while (p != nullptr){
            //tracks num of elements less than p
            int num_less = 0;
            //temp node to iterate through loop to compare with p node
            Node *p_temp = head;
            while (p_temp != nullptr){
                //if data val of p < data val of p temp, then increment num less
                if (p->data < p_temp->data){
                    num_less++;
                }
                //increment
                p_temp = p_temp->next;
            }
            //check if num_less is equal to num of pos inputted, if so set the value of value to the data value
            if (num_less == pos){
                value = p->data;
            }
            //increment
            p = p->next;
        }
    }else{
        return false;
    }
    return true;
    
}

void Set::swap(Set& other){
    
    if ((empty() && other.empty()) || this == &other)
        return;
    //swap nodes of the heads
    Node *p = head;
    head = other.head;
    other.head = p;
    //swap nodes of the tails
    Node *q = tail;
    tail = other.tail;
    other.tail = q;
    
    //swap sides of the two objects
    int temp_size = m_size;
    m_size = other.m_size;
    other.m_size = temp_size;
    
}

void unite(const Set& s1, const Set& s2, Set& result){
    
    for (int i = 0; i < s1.size(); i++){
        //create temp object that is set to a distinct value at each index i
        ItemType temp;
        s1.get(i, temp);
        //inserts into result the value
        result.insert(temp);
    }
    
    for (int i = 0; i < s2.size(); i++){
        //create temp object that is set to a distinct value at each index i
        ItemType temp;
        s2.get(i, temp);
        //inserts into result the value
        result.insert(temp);
    }
    
}

void difference(const Set& s1, const Set& s2, Set& result){
    for (int i = 0; i < s1.size(); i++){
        //create temp object that is set to a distinct value at each index i
        ItemType temp;
        s1.get(i, temp);
        if (!s2.contains(temp)){
            //inserts into result the value unless s2 contains the same value
            result.insert(temp);
        }
    }
    
    for (int i = 0; i < s2.size(); i++){
        //create temp object that is set to a distinct value at each index i
        ItemType temp;
        s2.get(i, temp);
        if (!s1.contains(temp)){
            //inserts into result the value unless s1 already contains the same value
            result.insert(temp);
        }
    }
}
