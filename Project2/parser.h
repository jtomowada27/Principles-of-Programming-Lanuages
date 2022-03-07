#ifndef __PARSER__H_
#define __PARSER__H_
#include <vector>
#include "lexer.h"

using namespace std;

struct scopeNode{
	string name;
	string scope;
	bool permission;
};
struct scopeList{
	scopeNode item;
	scopeList* next = NULL;
};
class Parser{
   public:
	void parse_program();
	struct scopeList* list;
   private:
	LexicalAnalyzer lexer;
	vector<string> scopes;
	string currScope;

	bool check_eof();
	void parse_glob_vars();
	void parse_var_list(bool);
	void parse_pub_vars();
	void parse_priv_vars();
	void parse_scope();
	void parse_stmt_list();
	void parse_stmt();
	void print_stmt(string, string);
	void parse_X(Token,bool);
	void printList();
	string whatScope(string);
};
#endif
