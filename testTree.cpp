#include "testTree.h"

#include <cstdio>
#include <fstream>
#include <iostream> 
#include <string> 
#include <set>
#include <stack>

using namespace std; 

std::set <string> idList;
bool semanticsError = 0;
static int LabelCntr = 0;
static int VarCntr = 0;
static char Name[20];

string nonTerminal_names[] = {
        "PROGRAM", "VARS", "VARLIST", "EXP", "M", "N", "R", "STATS", "MSTAT", "STAT", "BLOCK", "IN", "OUT", "IF", "LOOP", "ASSIGN", "R0",
};



node * createNode(node_t r) {
	node * root = new node;
	root -> label = r;

	Scanner::Token token1;
	Scanner::Token token2;
	Scanner::Token token3;
	Scanner::Token token4;
	Scanner::Token token5;
	root -> tk1 = token1;
	root -> tk2 = token2;
	root -> tk3 = token3;
	root -> tk4 = token4;
	root -> tk5 = token5;
	root -> child1 = NULL;
	root -> child2 = NULL;
	root -> child3 = NULL;
	root -> child4 = NULL;
	return root;
}

// start at root and visit each node before child node 
void traversePreorder(node *n, int depth) {
	if(n) {
		printNode(n, depth);
		traversePreorder(n -> child1, depth+1);
		traversePreorder(n -> child2, depth+1);
		traversePreorder(n -> child3, depth+1);
		traversePreorder(n -> child4, depth+1);
	}
}

// Visits nodes and prints parse tree
// TODO: need to figure out how to connect similar levels of tree
void printNode(node *n, int depth) { 
	string space = "";

	for (int i = 0; i < depth*2; i++) {
		space=space + " ";
	}

	cout << space << nonTerminal_names[n -> label] << ": ";

	if(n -> tk1.value != "") {
		cout << n->tk1.value << " ";
	}
	if(n -> tk2.value != "") {
		cout << n->tk2.value << " ";
	}
	if(n -> tk3.value != "") {
		cout << n->tk3.value << " ";
	}
	if(n -> tk4.value != "") {
		cout << n->tk4.value << " ";
	}
	if(n -> tk5.value != "") {
		cout << n->tk5.value << " ";
	}
	cout << endl;
}

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
		if (it != idList.end()) {      	// Found in idList, throw detailed error msg       
        	std::cout << "SEMANTICAL ERROR: redefinition of " << tk.value;
			std::cout << " on line " << tk.line << "\n";
		} else {                       	// Not found in idList, add to idList		
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
		}
	}
 }

 static char *newName(nameType what) {
	if (what==VAR) { 
		sprintf(Name,"T%d",VarCntr++); 
	} else { // creating new Label
		sprintf(Name,"L%d",LabelCntr++); 
	}
	return(Name);
 }

  void recGen(node *n, FILE *out) {
    string label, label2, argR;
    if (n == NULL) {
        return;
    }
    switch(n->label) {
        case PROGRAMn:
            cout << "\nASM OUTPUT:\n";
            recGen(n->child2, out);
            fprintf(out, "\nSTOP\n");
            recGen(n->child1, out);
            for (int i = 0; i < VarCntr; i++) {
                fprintf(out, "T%d 0\n", i);
            }
            return;
        case VARSn:
            recGen(n->child1, out);
            break;
        case VARLISTn:
            recGen(n->child1, out);
            fprintf(out, "%s %s\n", n->tk1.value.c_str(), n->tk3.value.c_str());
            break;
        case EXPn:
            recGen(n->child1, out);
            recGen(n->child2, out);
            if (n->tk1.value == "*") {
                argR = newName(VAR);
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nSTORE %s", argR.c_str());
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nMULT %s", argR.c_str());
                fprintf(out, "\nPUSH\nSTACKW 0");
            } else if (n->tk1.value == "/") {
                argR = newName(VAR);
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nSTORE %s", argR.c_str());
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nDIV %s", argR.c_str());
                fprintf(out, "\nPUSH\nSTACKW 0");
            }
            break;
        case Mn:
            recGen(n->child1, out);
            recGen(n->child2, out);
            if (n->tk1.value == "+") {
                argR = newName(VAR);
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nSTORE %s", argR.c_str());
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nADD %s", argR.c_str());
                fprintf(out, "\nPUSH\nSTACKW 0");
            }
            break;
        case Nn: 
            recGen(n->child1, out);
            recGen(n->child2, out);
            if(n->tk1.value == "-") {
                argR = newName(VAR);
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nSTORE %s", argR.c_str());
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nSUB %s", argR.c_str());
                fprintf(out, "\nPUSH\nSTACKW 0");
            } else if (n->tk1.value == "~") {
                fprintf(out, "\nSTACKR 0\nPOP");
                fprintf(out, "\nMULT -1");
                fprintf(out, "\nPUSH\nSTACKW 0");
            }
            break;
        case Rn:
            if(n->tk1.type == "IDENTIFIER") {
                fprintf(out, "\nLOAD %s", n->tk1.value.c_str());
                fprintf(out, "\nPUSH\nSTACKW 0");
            } else if (n->tk1.type == "INTEGER") {
                fprintf(out, "\nLOAD %s", n->tk1.value.c_str());
                fprintf(out, "\nPUSH\nSTACKW 0");
            }
            recGen(n->child1, out);
            break;
        case STATSn:
            recGen(n->child1, out);
            recGen(n->child2, out);
            break;
        case MSTATn:
            recGen(n->child1, out);
            recGen(n->child2, out);
            break;
        case STATn:
            recGen(n->child1, out);
            break;
        case BLOCKn:
            recGen(n->child1, out);
            recGen(n->child2, out);
            break;
        case INn:
            fprintf(out, "READ %s", n->tk1.value.c_str());
            break;
        case OUTn:
            argR = newName(VAR);
            recGen(n->child1, out);
            fprintf(out, "\nSTACKR 0");
            fprintf(out, "\nPOP");
            fprintf(out, "\nSTORE %s", argR.c_str());
            fprintf(out, "\nWRITE %s", argR.c_str());
            break;
        case IFn:
            recGen(n->child3, out);
            argR = newName(VAR);
            fprintf(out, "\nSTORE %s", argR.c_str());
            recGen(n->child1, out);
            fprintf(out, "\nSUB %s", argR.c_str());
            label = newName(LABEL);

            if (n->child2->tk1.value == "%") {      
                fprintf(out,"\nBRZERO %s",label.c_str());
            } else if (n->child2->tk1.value == "<") {      
                fprintf(out,"\nBRZPOS %s",label.c_str());
            } else if (n->child2->tk1.value == ">") {      
                fprintf(out,"\nBRZNEG %s",label.c_str());
            } else if (n->child2->tk1.value == "<<") {     
                fprintf(out,"\nBRPOS %s",label.c_str());
            } else if (n->child2->tk1.value == ">>") {    
                fprintf(out,"\nBRNEG %s",label.c_str());
            } else if (n->child2->tk1.value == "=") {
                fprintf(out,"\nBRPOS %s",label.c_str());   
                fprintf(out,"\nBRNEG %s",label.c_str());
            }

            recGen(n->child4, out);
            fprintf(out, "\n%s: NOOP", label.c_str());
            break;
        case LOOPn:
            argR = newName(VAR);
            label = newName(LABEL);
            label2 = newName(LABEL);
            fprintf(out, "\n%s: NOOP", label.c_str());
            recGen(n->child3, out);
            fprintf(out, "\nSTORE %s", argR.c_str());
            recGen(n->child1, out);
            fprintf(out, "\nSUB %s", argR.c_str());

            if (n->child2->tk1.value == "%") { 
                fprintf(out,"\nBRZERO %s",label2.c_str());
            } else if (n->child2->tk1.value == "<") {      
                fprintf(out,"\nBRZPOS %s",label2.c_str());
            } else if (n->child2->tk1.value == ">") {      
                fprintf(out,"\nBRZNEG %s",label2.c_str());
            } else if (n->child2->tk1.value == "<<") {     
                fprintf(out,"\nBRPOS %s",label2.c_str());
            } else if (n->child2->tk1.value == ">>") {    
                fprintf(out,"\nBRNEG %s",label2.c_str());
            } else if (n->child2->tk1.value == "=") {
                fprintf(out,"\nBRPOS %s",label2.c_str());   
            }
            recGen(n->child4, out);
            fprintf(out, "\nBR %s\n", label.c_str());
            fprintf(out, "\n%s: NOOP", label2.c_str());
            break;
        case ASSIGNn:
            recGen(n->child1, out);
            fprintf(out, "\nSTACKR 0");
            fprintf(out, "\nSTORE %s", n->tk1.value.c_str());
            break;
        case R0n:
            recGen(n->child1, out);
            fprintf(out, " %s ", n->tk1.value.c_str());
            break;
	   }   
}
 
