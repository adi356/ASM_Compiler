#include "compiler.h"

using namespace std;

static int LabelCntr = 0;
static int VarCntr = 0;
static char Name[20];

static char *newName(nameType what) {
	if (what==VAR) { 
		sprintf(Name,"T%d",VarCntr++); 
	} else { 
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
 
