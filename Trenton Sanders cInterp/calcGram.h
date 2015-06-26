#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <stack>
#include <sstream>
using namespace std;

#ifndef _calcGram_H
#define _calcGram_H

/*
Trenton Sanders
CSC 3310

Calculator Grammar

<program>      ->   <stmt_list> $$
<stmt_list>    ->   <stmt> <stmt_list> | <epsilon>
<stmt>         ->   id := <expr> | read id | write <expr>
<expr>         ->   <term> <term_tail>
<term_tail>    ->   <add_op> <term> <term_tail> | <epsilon>
<term>         ->   <factor> <factor_tail>
<factor_tail>  ->   <mul_op> <factor> <factor_tail> | <epsilon>
<factor>       ->   ( <expr> ) | id | number
<add_op>       ->   + | -
<mul_op>       ->   * | /

Token Table (Type - Name - #)

End of System           - EOFSY   - 0
Language ID             - ID      - 1
Assignment Operator     - ASSOP   - 2
Read Keyword            - READSY  - 3
Write Keyword           - WRITESY - 4
Left Paranthesis        - LPARAN  - 5
Right Paranthesis       - RPARAN  - 6
Number Value            - NUMCON  - 7
Addition Operator       - ADDOP   - 8
Subtraction Operator    - SUBOP   - 9
Multiplication Operator - MULOP   - 10
Division Operator       - DIVOP   - 11
Invalid Token           - ERROR   - 12
*/

//Global Variables
enum CalcGramTok {EOFSY = 0, ID, ASSOP, READSY, WRITESY, LPARAN, RPARAN, NUMCON, ADDOP, SUBOP, MULOP, DIVOP, ERROR}; //Token list
string TokenString; //Used for grabbing literal tokens
int token; //Current token
ifstream inFile; //Input file
stack<double> tokStack; //Stack of tokens to interpret
map<string, double> symtab; //Table of ID values

//Scanner functions
int scanFun(); //Scans through the file, returns token # (0-12)
void skipComment(); //Used to skip over comments in the file
string decodeEnum(int); //Used to translate token #s into token names

//Interpreter functions
	//Non-terminals
		void interpret();
		void stmt_list();
		void stmt();
		void expr();
		void term_tail();
		void term();
		void factor_tail();
		void factor();
		void add_op(); //Stacks 1 for +, 0 for -
		void mul_op(); //Stacks 1 for *, 0 for /
	//Other
		bool match(int expected); //Used to verify token grammar, returns true if expected token matches current token
									//If true, also advances token
		double StrToDbl(string s); //Converts a string into a double using stringstream

#endif