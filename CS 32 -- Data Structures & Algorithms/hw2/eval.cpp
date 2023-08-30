#include <stdio.h>
#include <stack>
#include <string>
#include <cassert>
#include <iostream>
using namespace std;

void removeSpaces(string& infix);
bool validParenthesis(string infix);
bool validCharacters(string infix);
bool validOperators(string infix);
bool isValidInfix(string infix);
bool checkPrecedence(char operator1, char operator2);
string infixToPostfix(string infix, string& postfix);
int evaluate(string infix, string& postfix, bool& result);

void removeSpaces(string& infix){
    if (infix.empty())
        return;
    
    string temp;
    for (int i = 0; i < infix.size(); i++){
        if (infix.at(i) != ' '){
            temp += infix.at(i); //add every character except spaces to temp string
        }
    }
    infix = temp; //set temp string without spaces to infix
}

bool validParenthesis(string infix){
    if (infix.empty())
        return false;
    
    int num_open = 0;
    int num_close = 0;
    string parenthesis = "";
    
    for (int i = 0; i < infix.size(); i++){
        if (infix.at(i) == '('){
            parenthesis+=infix.at(i); //add parenthesis to parenthesis string
            num_open++; //iterate number of open parenthesis
        }
        else if (infix.at(i) == ')'){
            parenthesis+=infix.at(i); //add parenthesis to parenthesis string
            num_close++; //iterate number of closed parenthesis
        }
        
        if (i < infix.size() - 1 )
            if (infix.at(i) == '(' && infix.at(i+1) == ')') //cannot have nothing in between parenthesis
                return false;
    }
    
    if (num_open != num_close) //must be balance of open to closed parenthesis
        return false;
    
    stack<char> p;
    //parse through string of all parenthesis in order so that each opening parenthesis has a subsequent closing parenthesis
    for (int j = 0; j < parenthesis.size(); j++){
        if (parenthesis.at(j) == '('){
            p.push(parenthesis.at(j));
        }
        
        if (p.empty())  //there should be a beginning '('
            return false;
        
        if (parenthesis.at(j) == ')'){
            p.pop(); //if opening parenthesis has match to a closed parenthesis, pop the opening parenthesis
        }
    }
    return p.empty(); //return if p is empty or not -- if empty, all open parenthesis have a closed parenthesis, and thus returns true
}

bool validCharacters(string infix){
    if (infix.empty())
        return false;
    
    for (int i = 0; i < infix.size(); i++){
        if (isalpha(infix.at(i)) && (infix.at(i) != 'T' && infix.at(i) != 'F')) //check if a character is alphabetic -- if so, must be T or F
            return false;
        
        if (i < infix.size() - 1){
            if (isalpha(infix.at(i)) && isalpha(infix.at(i+1))) //check to make sure two characters are not subsequently next to each other
                return false;
        }
    }
    return true; //return true if no errors detected
}

bool validOperators(string infix){
    if (infix.empty())
        return false;
    
    for (int i = 0; i < infix.size(); i++){
        char ch = infix.at(i);
        if (!isalpha(ch) && ch != '(' && ch != ')' && ch != '&' && ch != '!' && ch != '^')
            return false;
        
        if (i < infix.size() - 1){
            char next_ch = infix.at(i+1);
            // '&','!','^','(' can have a following character that is an alphabet character, '!', or '('
            if((ch == '&' || ch == '^' || ch == '!' || ch == '(') && !(isalpha(next_ch) || next_ch == '!' || next_ch == '(' )){
                return false;
            }
            // ')' and alphabet characters can have a following character that is '&', '^', or ')'
            if((ch == ')' || isalpha(ch)) && !(next_ch == '&' || next_ch == '^' || next_ch == ')')){
                return false;
            }
        }
    }
    return true; //if nothing fails then we have a string with valid operators, hence return true
}

bool isValidInfix(string infix){
    
    if (infix.empty())
        return false;
    //validate parenthesis, characters, and operators
    if (validParenthesis(infix) && validCharacters(infix) && validOperators(infix)){
        unsigned long len = infix.size() - 1;
        //check infix starts with a letter, !, or ( and ends with a letter or )
        if ((isalpha(infix.at(0)) || infix.at(0) == '!' || infix.at(0) == '(') && (isalpha(infix.at(len)) || infix.at(len) == ')')){
            return true;
        }else
            return false;
    }else
        return false;
}

bool checkPrecedence(char operator1, char operator2){
    // ! supercedes all other operators
    if (operator1 == '!')
        return true;
    //& supercedes all other operators except !
    else if (operator1 == '&' && (operator1 == '&' || operator2 == '!'))
        return true;
    //^ only supercedes another ^ to the right in the sequence
    else if (operator1 == '^' && operator2 == '^')
        return true;
    else
        return false;
    
}

string infixToPostfix(string infix, string& postfix){
    postfix = "";
    stack<char> ops;
    
    for (int i = 0; i < infix.size(); i++){
        char ch = infix.at(i);
        switch (ch){
            default:
                postfix += ch;
                break;
            case '(':
                ops.push(ch);
                break;
            case ')':
                while(ops.top() != '('){
                    postfix+=ops.top();
                    ops.pop();
                }
                ops.pop();
                break;
            case '&':
            case '^':
                while(!ops.empty() && ops.top() != '(' && checkPrecedence(ops.top(), ch)){
                    postfix += ops.top();
                    ops.pop();
                }
                ops.push(ch);
                break;
            case '!':
                ops.push(ch);
                break;
        }
    }
    while(!ops.empty()){
        postfix += ops.top();
        ops.pop();
    }
    return postfix;
}

int evaluate(string infix, string& postfix, bool& result){
    
    removeSpaces(infix);
    if(!isValidInfix(infix)){
        return 1;
    }
    infixToPostfix(infix, postfix);
    stack<bool> op;
    for(int i = 0; i < postfix.size(); i++){
        char ch = postfix.at(i);
        if (isalpha(ch)){
            if (ch == 'T'){
                op.push(true);
            }else if (ch == 'F'){
                op.push(false);
            }
        }else if(ch == '!'){
            bool operand = op.top();
            op.pop();
            op.push(!operand);
        }
        else{
            bool operand2 = op.top();
            op.pop();
            bool operand1 = op.top();
            op.pop();
            if (ch == '&'){
                op.push(operand1 && operand2);
            }else if (ch == '^'){
                if ((operand1 || operand2) && (operand1 != operand2))
                    op.push(true);
                else
                    op.push(false);
            }
        }
    }
    result = op.top();
    
    return 0;
}
