#include "calcGram.h"

int main(int argc, char *argv[])
{
	//Ensure file is in arguments
	if(argc<2)
	{
		cout << "There is no file name in the argument.\n";
		return 0;
	}

	inFile.open(argv[1]);

	//File unopened exception
	if (!inFile.is_open()) 
	{ 
		cout << "Error: File not opened"; 
		return 0; 
	}
	cout << "Now parsing the file\n";
	interpret();
	cout << "Done parsing the file\n";

	return 0;
}

void interpret()
{
	token=scanFun();
	while (token==ERROR) //skip invalid tokens
	{
		cout << "Error: invalid token\n";
		token=scanFun(); 
	}
	do
	{
		stmt_list();
	} while (!match(EOFSY)); //only finish parsing file when eof is reached
}

void stmt_list()
{
	if(token == ID || token == READSY || token == WRITESY) //predicted tokens
	{
		stmt();
		stmt_list();
	}
	else if (token==EOFSY); //<epsilon>
	else
	{
		cout << "Error (stmt_list): " << TokenString << " is an unexpected token\n";
		match(token); //advance past error
	}
}

void stmt()
{
	if (token == ID)	//ID := <expr>
	{
		string strID = TokenString; //save ID before advancement
		match(ID); //Advance token
		if (match(ASSOP)) //Begin assign output only if token matches, advances
		{
				expr();
				if (!tokStack.empty())
				{
					cout << "Assign: " << strID << " "; 
					cout << tokStack.top() << endl; //Output value given by <expr>
					symtab[strID] = tokStack.top();	//Assign value given by <expr> to its ID
					tokStack.pop(); //Advance stack
				}
		}
		//else incorrect grammar
	}
	else if (token == READSY)	//Read ID
	{
		match(READSY); //Advance token
		string strID = TokenString; //save ID string before advancement
		if (match(ID))	//Read statement only if ID expected token
		{
			cout << "Read: enter value for " << strID <<": ";
			double inval;
			//retrieve valid value
			while(1)
			{
				if (cin >> inval) break; //valid value
				else //invalid value; not a double
				{
					cout << "Invalid value; please enter a double: ";
					cin.clear();
					while(cin.get()!='\n'); //prevents infinite loop
				}
			}
			symtab[strID]=inval; //Assign inputed value to ID
		}
		//else incorrect grammar
	}
	else if (token == WRITESY) //Write <expr>
	{
		match(WRITESY); //Advance token
		expr();
		if (!tokStack.empty())
		{
			cout << "Write: ";
			cout << tokStack.top() << endl; //Output value given by <expr>
			tokStack.pop(); //Advance stack
		}
	}
	else cout << "ERROR(stmt): Tokens ID, READSY, or WRITESY expected\n"; //Incorrect grammar
}

void expr()
{
	term();
	term_tail();
}

void term_tail()
{
	if (token == ADDOP || token == SUBOP) //predicted tokens
	{
		add_op();
		term();
		double lVal, rVal, op;

		//Retrieve values from stack
		if (!tokStack.size()<3)
		{
			rVal = tokStack.top();
			tokStack.pop();
			op = tokStack.top();
			tokStack.pop();
			lVal = tokStack.top();
			tokStack.pop();

			//Compute, push result on stack
			if (op==1.0) //addition
			{
				tokStack.push(lVal+rVal);
			}
			else if (op==0.0) //subtraction
			{
				tokStack.push(lVal-rVal);
			}
			else cout << "Error with <term_tail> computation\n";

			term_tail();	//continue parsing
		}
	}
	else if (token==RPARAN||token==ID||token==READSY||token==WRITESY||token==EOFSY); //<epsilon>
	else 
	{
		cout <<"ERROR (term_tail): " << TokenString << " is an unpredicted token\n";
		match(token); //advance past error
	}
}

void term()
{
	factor();
	factor_tail();
}

void factor_tail()
{
	if (token == MULOP || token == DIVOP) //predicted tokens
	{
		mul_op();
		factor();

		//Retrieve values from stack
		if (!tokStack.size()<3)
		{
			double rVal, lVal, op;
			rVal = tokStack.top();
			tokStack.pop();
			op = tokStack.top();
			tokStack.pop();
			lVal = tokStack.top();
			tokStack.pop();

			//Compute, push result on stack
			if (op==1.0) //multiply
			{
				tokStack.push(lVal*rVal);
			}
			else if (op==0.0) //divide
			{
				if (rVal!=0) //cannot divide by zero
					tokStack.push(lVal/rVal);
				else
				{
					cout << "Error: Can't divide by zero\n";
					tokStack.push(0);
				}
			}
			else cout << "Error with <factor_tail> computation\n";
		
			factor_tail(); //continue parsing
		}
	}
	else if (token==ADDOP||token==SUBOP||token==RPARAN||token==ID||token==ID||token==READSY||token==WRITESY||token==EOFSY); //<epsilon
	else
	{
		cout << "ERROR (factor_tail): " << TokenString << " is an unexpected token\n";
		match(token); //advance past error
	}
}

void factor()
{
	if (token == LPARAN) // ( <expr )
	{
		match(LPARAN); //advance
		expr();
		match(RPARAN); //advance
	}
	else if (token == ID)
	{
		tokStack.push(symtab[TokenString]); //push ID value on stack
		match(ID); //advance
	}
	else if (token == NUMCON)
	{
		tokStack.push(StrToDbl(TokenString)); //Push converted string representing NUMCON onto stack
		match(NUMCON); //advance
	}
	else cout << "ERROR(factor): Expected tokens LPARAN, ID, or NUMCON\n"; //Incorrect grammar
}

void add_op()
{
	if (token == ADDOP)
	{
		match(ADDOP);
		tokStack.push(1.0);
	}
	else if (token == SUBOP)
	{
		match(SUBOP);
		tokStack.push(0.0);
	}
	else cout << "ERROR(add_op): Expected tokens ADDOP or SUBOP\n"; //Incorrect grammar
}

void mul_op()
{
	if (token == MULOP)
	{
		match(MULOP);
		tokStack.push(1.0);
	}
	else if (token == DIVOP)
	{ 
		match(DIVOP);
		tokStack.push(0.0);
	}
	else cout << "ERROR(mul_op): Expected tokens MULOP or DIVOP\n"; //Incorrect grammar
}

bool match(int expected)
{
	if(token == expected) //advance token
	{
		if(token!=EOFSY)
			token=scanFun();
		while (token==ERROR) //skip invalid tokens
		{
			cout << "Error: invalid token\n";
			token=scanFun(); 
		}
		return true;
	}
	else //Incorrect grammar
	{
		cout << "Expected token '" << decodeEnum(expected) << "' but instead recieved \"" << TokenString << "\"" << endl;
		return false;
	}
}

double StrToDbl(string s)
{
	double d;
	stringstream ss(s);
	ss>>d;
	return d;
}

int scanFun()
{
	char c; //Holds current character in file
	while (((c=inFile.get()) == ' ' || c=='\t') || c == '\n'); //Skips whitespaces

	TokenString=c; //Token string holds literal characters that flag token
	if(c=='(') return LPARAN;
	if(c==')') return RPARAN;
	if(c=='+') return ADDOP;
	if(c=='-') return SUBOP;
	if(c=='*') return MULOP;
	if(c=='/') //Check for division or comment
		if (inFile.get()=='*') //Check for comments
		{
			skipComment(); //Skips comments - file left at final character in comment (either '\n' or '/')
			return scanFun(); //Allows scanFun to "reset" - else input following comments becomes problematic
		}
		else //Division token
		{
			inFile.unget(); //Ungets token after '/'
			return DIVOP;
		}
	if(c==':') //Check for assignment operator; otherwise, illegal token
		if(inFile.get()=='=') 
		{ 
			TokenString = ":="; 
			return ASSOP;
		}
		else return ERROR;
	if((c>='0' && c<='9')) //NUMCON token can be many digits and can have decimal
	{
		while ((c=inFile.get()) >='0' && c<='9') //Get digits before decimal
			TokenString+=c; 
		if (c=='.') //Check for decimal
		{
			TokenString+='.';
			while((c=inFile.get()) >='0' && c<='9') //Get digits after decimal
				TokenString+=c;
			inFile.unget();	//Unget token after final digit
			return NUMCON;
		} 
		else //Number without decimal
		{
			inFile.unget(); //Unget token after final digit
			return NUMCON;
		}
	}
	if((c>='A'&& c<='Z') || (c>='a' && c<='z')) //Start with letter: ID, read, write, or illegal token
	{
		while (((c=inFile.get())>='A'&&c<='Z')||(c>='a'&&c<='z') || (c>='0'&&c<='9') || (c=='_')) //Moves through all legal characters for ID
			TokenString+=c;
		inFile.unget(); //Ungets last token after legal ID

		//Because keywords read and write are not case-sensitive:
		transform(TokenString.begin(), TokenString.end(), TokenString.begin(), tolower); //Transforms characters to lower-case

		//Check for specific token
		if(TokenString == "read") return READSY;
		if(TokenString == "write") return WRITESY;
		else return ID;
	}
	if (c==EOF)
		return EOFSY;
	else //Picks up any illegal tokens dropped off
		return ERROR;
}

void skipComment()
{
	int i;
	while(((i = inFile.get()) != '*' && i != '\n')&&i!=EOF); //Cycles through characters until * (which flags end of comment) or \n (because comments may only be on one line)
	if ((i == '\n' || i==EOF)|| (i=inFile.get()) == '/') //Checks to see if comment is actually over
	{
		if (i==EOF)
			inFile.unget();
		return;
	}
	else 
	{
		inFile.unget();
		skipComment();
	}
}

string decodeEnum (int x) //x is the token number
{
	switch (x)
	{
	case 0: return "EOFSY";
	case 1: return "ID";
	case 2: return "ASSOP";
	case 3: return "READSY";
	case 4: return "WRITESY";
	case 5: return "LPARAN";
	case 6: return "RPARAN";
	case 7: return "NUMCON";
	case 8: return "ADDOP";
	case 9: return "SUBOP";
	case 10: return "MULOP";
	case 11: return "DIVOP";
	default: return "ERROR";
	}
}