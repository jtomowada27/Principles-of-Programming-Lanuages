/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM" //MODIFIED AFTER ERROR// TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c,basex,c01,c86,temp;
    input.GetChar(c);
    temp = c;
    //cout << "Line89"<<basex<<"|"<<c<<endl;
    //if (isdigit(c)) {//this was the orginal
    if(isalnum(c)){//isalnum(c)
        if (c == '0') {
            tmp.lexeme = "0";
        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) //isdigit(c)
            {
                tmp.lexeme += c;
                tmp.token_type = NUM;//UNCOMMENT IF WRONG////////////////////////////////
               // cout<<"Line100"<<basex<<"|"<<c<<endl;
                input.GetChar(c);
                //cout << "Line101"<<basex<<"|"<<c<<endl;
            }///////
            //input.UngetChar(temp);

            //////
            if (!input.EndOfInput()) 
            {
                input.UngetChar(c);
                //cout << "Line109"<<basex<<"|"<<c<<endl;
            }
        }
        //}
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
        //modified
        //cout << "Line114"<<basex<<"|"<<c<<endl;
        input.GetChar(basex);//basex
        //cout<<"Line115"<<basex<<"|"<<c<<endl;
        //input.GetChar(c);
        //cout<<"Line117"<<basex<<"|"<<c<<endl;
        switch(basex){//basex
        case 'x'://can detemine that the token is a base number potentially
        input.GetChar(c01);//c01
        //cout << "Line121"<<basex<<"|"<<c01<<"|"<<c<<endl;
        	if(c01 == '0')//c01
        	{
        	input.GetChar(c86);//c86
        		if(c86 == '8')//c86
        		{
        		tmp.lexeme += "x08";
        		tmp.token_type = BASE08NUM;
        		tmp.line_no = line_no;
        		return tmp;
        		}// is x08 base
        		else
        		{
        		input.UngetChar(c86);//c86
        		input.UngetChar(c01);//c01
        		input.UngetChar(basex);//basex 
        		tmp.token_type = NUM;
        		tmp.line_no = line_no;
        		return tmp;
        		}// is not x08 base, removes '08'
        	//input.UngetChar(c01);//REMOVE IF WRONG
        	}
        	else if(c01 == '1')//c01
        	{
        	input.GetChar(c86);//c86
        		if(c86 == '6')//c86
        		{
        		tmp.lexeme += "x16";
        		tmp.token_type = BASE16NUM;
        		tmp.line_no = line_no;
        		return tmp;
        		}//is x16 base
        		else
        		{
        		input.UngetChar(c86);//c86
        		//input.UngetChar(c01);//c01
        		//input.UngetChar(basex);//basex
        		}//is not x16 base, removes 'x16'
        	//input.UngetChar(c01);//REMOVE IF WRONG
        	}
        	else
        	{
        	input.UngetChar(basex);//basex
        	tmp.token_type = NUM;
        	tmp.line_no = line_no;
        	return tmp;
        	}
        case '.':
        input.GetChar(c);//.# or .alpha
        if(isdigit(c))
        {
        tmp.lexeme += '.';
        	while(!input.EndOfInput() && isdigit(c))//can make it eof??maybe not,not file
        	{
        	tmp.lexeme += c;
        	input.GetChar(c);
        	tmp.token_type = REALNUM;
        	tmp.line_no = line_no;
        	}
        	input.UngetChar(c);
        	return tmp;
        }
        else
        {
        input.UngetChar(c);
        input.UngetChar(basex);
        tmp.line_no = line_no;
        return tmp;
        }
        default:
        input.UngetChar(basex);
        tmp.line_no = line_no;
        return tmp;
        }
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}
Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {//cout << "digit"<<endl;
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {//cout<<"alpha"<<endl;
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;
//cout << "token"<<endl;
            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    //cout << "here"<<endl;
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
        //cout << "boi"<<endl;
    }
}
