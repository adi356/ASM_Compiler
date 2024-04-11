#include "staticSemantics.h"
#include "nonTerms.h"

#include <cstdio>
#include <fstream>
#include <iostream> 
#include <string> 
#include <set>
#include <stack>

using namespace std;

bool semanticsError = 0;
// needed for variable declarations
std::set <string> idList;

//added staticSemantics for P3
bool staticSemantics(node *n, int depth) {
	if(n) {
		
		insert(n->tk1, nonTerminal_names[n->label]);
		insert(n->tk2, nonTerminal_names[n->label]);
		insert(n->tk3, nonTerminal_names[n->label]);
		insert(n->tk4, nonTerminal_names[n->label]);
		insert(n->tk5, nonTerminal_names[n->label]);

		verify(n->tk1, nonTerminal_names[n->label]);
		verify(n->tk2, nonTerminal_names[n->label]);
		verify(n->tk3, nonTerminal_names[n->label]);
		verify(n->tk4, nonTerminal_names[n->label]);
		verify(n->tk5, nonTerminal_names[n->label]);

		staticSemantics(n->child1, depth+1);
		staticSemantics(n->child2, depth+1);
		staticSemantics(n->child3, depth+1);
		staticSemantics(n->child4, depth+1);
		
		return semanticsError;
	}
}

// added insert for P3
void insert(Scanner::Token tk, string label){
	if(tk.value != "" && tk.type == "IDENTIFIER" && label == "VARLIST"){
		auto it = idList.find(tk.value);
		if (it != idList.end()) {      	       
        	std::cout << "SEMANTICAL ERROR: redefinition of " << tk.value;
			std::cout << " on line " << tk.line << "\n";
		} else {                       		
			idList.insert(tk.value);
		}
	}	
}


// added verify for P3
 void verify(Scanner::Token tk, string label) {
	if(tk.value != "" && tk.type == "IDENTIFIER" && label != "VARLIST") {
		auto it = idList.find(tk.value);
		if (it == idList.end()) {
			std::cout << "SEMANTICS ERROR: use without declaration " << tk.value;
			std::cout << " on line " << tk.line << "\n";
            // comment out for debugging
            semanticsError = 1;
		}
	}
 }