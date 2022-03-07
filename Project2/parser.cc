#include <iostream>
#include <istream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <cctype>
#include <cstring>
#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"
/*GRAMMAR LIST FOR PARSER
program -> glob_vars scope
glob_vars -> epsilon|var_list semicolon
var_list -> ID|ID COMMA var_list
scope -> ID LBRACE pub_vars priv_vars stmt_list RBRACE
pub_vars -> epsilong|PUBLIC COLON var_list SEMICOLON
priv_vars -> epsilon|PRIVATE COLON var_list SEMICOLON
stmt_list -> stmt|stmt stmt_list
stmt -> scope|ID EQUAL ID SEMICOLON
*/
using namespace std;
int counter = 0;
InputBuffer input;
void Parser::parse_program()
{
currScope = "::";
scopes.push_back(currScope);
Token token1 = lexer.GetToken();
	if (token1.token_type == ID)
	{
        Token token2 = lexer.GetToken();
		if (token2.token_type == COMMA)// || token2.token_type == SEMICOLON)
		{
        //cout << "boi1";
		lexer.UngetToken(token2);
		lexer.UngetToken(token1);
		parse_glob_vars();
		parse_scope();
		}
        else if(token2.token_type == SEMICOLON)
        {
        //cout << "boi2";
        lexer.UngetToken(token2);
		lexer.UngetToken(token1);
		parse_glob_vars();
		parse_scope(); 
        }
		else if (token2.token_type == LBRACE)
		{
		lexer.UngetToken(token2);
		lexer.UngetToken(token1);
		parse_scope();
		}
		else{cout << "Syntax Error" << endl;exit(1);}
    }
	else{cout << "Syntax Error" << endl;exit(1);}
}

void Parser::parse_glob_vars(){
    Token token1 = lexer.GetToken();
    if (token1.token_type == ID)
	{
    lexer.UngetToken(token1);
    parse_var_list(true);
	token1 = lexer.GetToken();
	    if(token1.token_type != SEMICOLON)
        {
		cout << "Syntax Error" << endl;exit(1);
        }
    }
	else{lexer.UngetToken(token1);}
}

void Parser::parse_var_list(bool permission)
{
Token token1 = lexer.GetToken();
    if (token1.token_type == ID)
	{
    Token token2 = lexer.GetToken();
        if (token2.token_type == COMMA)
		{
        parse_var_list(permission);
        }
		else{lexer.UngetToken(token2);}
        parse_X(token1,permission);
    }
	else{cout << "Syntax Error" << endl;exit(1);}
}

void Parser::parse_priv_vars()
{
Token token1 = lexer.GetToken();
//cout << token1.lexeme;
    if(token1.token_type == PRIVATE)
	{
    token1 = lexer.GetToken();
        if(token1.token_type == COLON)
		{
        //cout << token1.lexeme;
		parse_var_list(false);
        token1 = lexer.GetToken();
            if(token1.token_type != SEMICOLON)
            {
            cout << "Syntax Error" << endl;exit(1);
            }
        }
		else{cout << "Syntax Error" << endl;exit(1);}
    }
	else{lexer.UngetToken(token1);}
}

void Parser::parse_pub_vars()
{
Token token1 = lexer.GetToken();
    if(token1.token_type == PUBLIC)
	{
    token1 = lexer.GetToken();//cout<<token1.lexeme;
        if(token1.token_type == COLON)
		{
        parse_var_list(true);
        token1 = lexer.GetToken();
            if(token1.token_type != SEMICOLON)
            {
            //cout << "here";
			cout << "Syntax Error" << endl;exit(1);
            }
        }
		else{cout << "Syntax Error" << endl;exit(1);}
    }
	else{lexer.UngetToken(token1);}
}

void Parser::parse_stmt() //::a = main.b
{
Token token1 = lexer.GetToken();
    if(token1.token_type == ID)
	{
    Token token2 = lexer.GetToken();
        if(token2.token_type == EQUAL)
		{
        token2 = lexer.GetToken();
        Token token3 = lexer.GetToken();
            if(token2.token_type == ID) //Add eof here????? Prints too early for syntax error?
			{
                if(token3.token_type == SEMICOLON)
                {
                print_stmt(token1.lexeme, token2.lexeme);
                }
            }
			else{cout << "Syntax Error" << endl;exit(1);}
        }
		else
		{
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        parse_scope();
        }
    }
	else{cout << "Syntax Error" << endl;exit(1);}
}

void Parser::parse_stmt_list()
{
Token token1 = lexer.GetToken();
//cout << token1.lexeme;
    if(token1.token_type == ID)
	{
    Token token2 = lexer.GetToken();
        if(token2.token_type == EQUAL)//termnaimal.nontermanial = <---
		{
        //cout << token2.lexeme;
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        parse_stmt();
        }
		else if(token2.token_type == LBRACE)
		{
        lexer.UngetToken(token2);
        lexer.UngetToken(token1);
        parse_scope();
        }
		else{cout << "Syntax Error" << endl;exit(1);}

        Token token3 = lexer.GetToken(); 
        if(token3.token_type == ID)
		{
        lexer.UngetToken(token3);
        parse_stmt_list();
        }
		else{lexer.UngetToken(token3);}
    }
	else{cout << "Syntax Error" << endl;exit(1);}
}

void Parser::parse_scope()
{
Token token1 = lexer.GetToken();
Token token2 = lexer.GetToken();
    if (token1.token_type == ID)
	{
        if(token2.token_type == LBRACE)
        {
        scopes.push_back(currScope);
        currScope = token1.lexeme;
        parse_pub_vars();
        parse_priv_vars();
        parse_stmt_list();
        check_eof();
        currScope = scopes.back();
        scopes.pop_back();
        token1 = lexer.GetToken();
        }
        if(token1.token_type != RBRACE)
        {
        cout << "Syntax Error" << endl;exit(1);
        }
    }
	else{cout << "Syntax Error" << endl;exit(1);}
}
bool Parser::check_eof()
{
    if(input.EndOfInput() == true)
    {
    return true;
    }
    else
    {
    return false;
    }
}
void Parser::parse_X(Token tokenx, bool permission)
{
struct scopeNode newNode;
newNode.scope = currScope;
newNode.name = tokenx.lexeme;
newNode.permission = permission;
struct scopeList *newList = new struct scopeList;
newList->item = newNode;
newList->next = list;
list = newList;
}

void Parser::print_stmt(string token1, string token2)
{
	cout<<whatScope(token1)<<token1<<" = "<<whatScope(token2)<<token2<<endl;
}

string Parser::whatScope(string id)
{
struct scopeList currNode = *list;
	while(!((currNode.item.name.empty() || currNode.next == NULL) ||(currNode.item.name == id && (currNode.item.scope == currScope || currNode.item.permission))))
	{
    currNode = *currNode.next;
    }
    if(!currNode.item.name.empty() && currNode.item.scope == "::") 
    {
    return "::";
    }
	else if(!currNode.item.name.empty() && currNode.item.name == id ) 
    {
    return currNode.item.scope + ".";
    }
	else{return "?.";}
}