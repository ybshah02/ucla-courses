#ifndef Table_h
#define Table_h

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <functional>

const int MAX_BUCKETS = 1000;

class Table
{
  public:
    Table(std::string keyColumn, const std::vector<std::string>& columns);
    ~Table();
    bool good() const;
    bool insert(const std::string& recordString);
    void find(std::string key, std::vector<std::vector<std::string>>& records) const;
    int select(std::string query, std::vector<std::vector<std::string>>& records) const;
    // We prevent a Table object from being copied or assigned by
    // making the copy constructor and assignment operator unavailable.
    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;
  private:
    std::string m_keyColumn;
    const std::vector<std::string> m_columns;
    
    struct BUCKET{
        unsigned int m_id;
        //linked list of vectors that hold the records -- resolves collision
        std::list<std::vector<std::string>> m_record;
        bool used = false;
    };
    
    unsigned int hash(std::string s) const{ return std::hash<std::string>()(s); }
    unsigned int mapFunc(unsigned int hashCode) const { return hashCode % MAX_BUCKETS; }
    BUCKET m_buckets[MAX_BUCKETS];
    
};

bool stringToDouble(std::string s, double& d);

#endif /* Table_h */
