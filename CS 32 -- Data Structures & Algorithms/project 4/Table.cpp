#include "Table.h"
#include <cstdlib>
#include <cstring>

// StringParser

class StringParser
{
  public:
    StringParser(std::string text = "")
    {
        setString(text);
    }

    void setString(std::string text)
    {
        m_text = text;
        m_start = 0;
    }

    bool getNextField(std::string& field);

  private:
    std::string m_text;
    size_t m_start;
};

bool StringParser::getNextField(std::string& fieldText)
{
    m_start = m_text.find_first_not_of(" \t\r\n", m_start);
    if (m_start == std::string::npos)
    {
        m_start = m_text.size();
        fieldText = "";
        return false;
    }
    if (m_text[m_start] != '\'')
    {
        size_t end = m_text.find_first_of(" \t\r\n", m_start+1);
        fieldText = m_text.substr(m_start, end-m_start);
        m_start = end;
        return true;
    }
    fieldText = "";
    for (;;)
    {
        m_start++;
        size_t end = m_text.find('\'', m_start);
        fieldText += m_text.substr(m_start, end-m_start);
        m_start = (end != std::string::npos ? end+1 : m_text.size());
        if (m_start == m_text.size()  ||  m_text[m_start] != '\'')
            break;
        fieldText += '\'';
    }
    return true;
}

// Table Implementations

Table::Table(std::string keyColumn, const std::vector<std::string>& columns): m_keyColumn(keyColumn), m_columns(columns){}

Table::~Table(){}

bool Table::good() const{
    //checks if columns vector is empty
    if (m_columns.empty())
        return false;
    
    //stores if keyColumn is present in columns vector
    bool containsKey = false;
    
    //iterate through columns
    for (int i = 0; i < m_columns.size(); i++){
        //check if column element is an empty string
        if (m_columns[i] == "")
            return false;
        
        //check if keyColumn is present in columns vector
        if (m_keyColumn == m_columns[i])
            containsKey = true;
        
        //compare each column value to determine if duplicates
        for(int j = 0; j < m_columns.size(); j++){
            if (i != j && m_columns[i] == m_columns[j])
                return false;
        }
    }
    
    if (!containsKey)
        return false;
    
    return true;
}

bool Table::insert(const std::string& recordString){
    
    if (!good())
        return false;
    
    StringParser parser(recordString);
    std::string s;
    
    //find the index of the key column
    int indexKey = 0;
    for (int i = 0; i < m_columns.size(); i++){
        if (m_keyColumn == m_columns[i])
            indexKey = i;
    }
    
    std::vector<std::string> record;
    int count = 0;
    //create a vector of the values inputted in the recordString parameter and track count of values
    while(parser.getNextField(s) && s != ""){
        record.push_back(s);
        count++;
    }
    
    //checks if count of values meets the columns of the table
    if (count != m_columns.size())
        return false;
    
    //find hashcode and index of inserting
    int hashCode = hash(record[indexKey]);
    int index = mapFunc(hashCode);
    
    //change bucket value accordingly
    m_buckets[index].m_id = index;
    m_buckets[index].m_record.push_back(record);
    m_buckets[index].used = true;
    
    return true;
}

void Table::find(std::string key, std::vector<std::vector<std::string>>& records) const {
    
    if (good()){
        //find hashcode and index using key
        int keyHashCode = hash(key);
        int index = mapFunc(keyHashCode);
        
        std::list<std::vector<std::string>> :: const_iterator it;
        
        records.clear();
        //finds all records associated with the key
        for (it = m_buckets[index].m_record.begin(); it != m_buckets[index].m_record.end(); it++){
            records.push_back(*it);
        }
    }
}

int Table::select(std::string query, std::vector<std::vector<std::string>>& records) const {
    
    if (!good()){
        return -1;
    }
    
    StringParser parser(query);
    std::string s;
    std::vector<std::string> queryStatement;

    //format query into vector of the three components
    while(parser.getNextField(s) && s != ""){
        queryStatement.push_back(s);
    }
    
    //check if the size of query is equal to the three required components
    if (queryStatement.size() != 3){
        records.clear();
        return -1;
    }
    
    //finds column of table for query search
    int col = -1;
    for(int i = 0; i < m_columns.size(); i++){
        if (queryStatement[0] == m_columns[i])
            col = i;
    }
    
    if(col == -1){
        records.clear();
        return -1;
    }
    
    records.clear();
    int countNotValid = 0;
    //iterature through the buckets
    for (int i = 0; i < MAX_BUCKETS; i++){
        if (m_buckets[i].used){
            std::string op = queryStatement[1];
            //convert operators to lowercase
            for (int i = 0; i < op.size(); i++){
                op[i] = tolower(op[i]);
            }
            
            std::string key = queryStatement[0];
            //case of string comparison
            if (op == ">" || op == ">=" || op == "<" || op == "<=" || op == "!=" || op == "==" || op == "="){
                
                std::string comparison = queryStatement[2];
                //check if comparison is empty
                if (comparison == ""){
                    records.clear();
                    return -1;
                }
                //iterate through linked list of values in bucket
                std::list<std::vector<std::string>> :: const_iterator it;
                for (it = m_buckets[i].m_record.begin(); it != m_buckets[i].m_record.end(); it++){
                    std::vector<std::string> item = *it;
                    //check operation type and add to records if comparison is true
                    if (op == ">"){
                        if (item[col] > comparison)
                            records.push_back(item);
                        }
                    
                    else if (op == ">="){
                        if (item[col] >= comparison)
                            records.push_back(item);
                    }
                    
                    else if (op == "<"){
                        if (item[col] < comparison)
                            records.push_back(item);
                    }
                    
                    else if (op == "<="){
                        if (item[col] <= comparison)
                            records.push_back(item);
                    }
                    
                    else if (op == "!="){
                        if (item[col] != comparison)
                            records.push_back(item);
                    }
                    
                    else if (op == "==" || op == "="){
                        if (item[col] == comparison)
                            records.push_back(item);
                    }
                }
            //numerical comparison case
            }else if (op == "lt" || op == "le" || op == "gt" || op == "ge" || op == "ne" || op == "eq"){
                
                std::string stringComparision = queryStatement[2];
                double comparison = 0;
                //convert value of field compared with to string
                bool validComparison = stringToDouble(stringComparision, comparison);
                //invalid field
                if (validComparison == false){
                    records.clear();
                    return -1;
                }
                
                //iterate through linked list of values in bucket
                std::list<std::vector<std::string>> :: const_iterator it;
                for (it = m_buckets[i].m_record.begin(); it != m_buckets[i].m_record.end(); it++){
                    std::vector<std::string> item = *it;
                    
                    std::string stringVal = item[col];
                    double val = 0;
                    //convert field of column to number
                    bool validNum = stringToDouble(stringVal, val);
                    
                    if(validNum){
                        //check operation type and add to records if comparison is true
                        if (op == "gt"){
                            if (val > comparison)
                                records.push_back(item);
                        }
                        
                        else if (op == "ge"){
                            if (val >= comparison)
                                records.push_back(item);
                        }
                        
                        else if (op == "lt"){
                            if (val < comparison)
                                records.push_back(item);
                        }
                        
                        else if (op == "le"){
                            if (val <= comparison)
                                records.push_back(item);
                        }
                        
                        else if (op == "ne"){
                            if (val != comparison)
                                records.push_back(item);
                        }
                        
                        else if (op == "eq"){
                            if (val == comparison)
                                records.push_back(item);
                        }
                    }else
                        countNotValid++;
                }

            }else{
                return -1;
            }
        }
    }
    
    return countNotValid;
}

bool stringToDouble(std::string s, double& d)
{
    char* end;
    d = std::strtof(s.c_str(), &end);
    return end == s.c_str() + s.size()  &&  !s.empty();
}

