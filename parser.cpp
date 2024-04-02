#include <iostream> 
#include <string> 
#include <stdio.h>
#include <cstdlib>
#include <cctype>

#include "parser.h"
#include "scanner.h"
#include "testTree.h"


using namespace std; 

Scanner::Token tk;

node *Program(Scanner*);   
node *Vars(Scanner*);
node *VarList(Scanner*);
node *Exp(Scanner*);
node *M(Scanner*);
node *N(Scanner*);
node *R(Scanner*);
node *Stats(Scanner*);
node *Mstat(Scanner*);
node *Stat(Scanner*);
node *Block(Scanner*);
node *In(Scanner*);
node *Out(Scanner*);
node *If(Scanner*);
node *Loop(Scanner*);
node *Assign(Scanner*);
node *R0(Scanner*);
void error(string);

// <program>  -> <vars> xopen <stats> xclose
node *Program(Scanner *scanner){
    node *node = createNode(PROGRAMn);
    node-> child1 = Vars(scanner);

    if (tk.value != "xopen") {
        error("xopen");
    }
    tk = (*scanner).getNext();

    node-> child2 = Stats(scanner);

    if (tk.value != "xclose") {
        error("xclose");
    }
    tk = (*scanner).getNext();

    return node;
}

// <vars> -> empty | xdata <varList>
node *Vars(Scanner *scanner){
    node *node = createNode(VARSn);
    if(tk.value == "xdata"){
        tk = (*scanner).getNext();
        node-> child1 = VarList(scanner);
    } 
    return node;    
}

// <varList> -> identifier : integer ; | identifier : integer <varList> 
node *VarList(Scanner *scanner){
    node *node = createNode(VARLISTn);

    if (tk.type != "IDENTIFIER") {
        error("IDENTIFIER");    
    }
    node-> tk1 = tk;
    tk = (*scanner).getNext();

    if (tk.value != ":") {
        error("COLON");
    }
    node-> tk2 = tk;
    tk = (*scanner).getNext(); 
    
    if (tk.type != "INTEGER") {
        error("INTEGER");
    }
    node-> tk3 = tk;
    tk = (*scanner).getNext();

    if (tk.value == ";"){
        node-> tk4 = tk;
        tk = (*scanner).getNext();
    } else if (tk.type == "IDENTIFIER") {
        node-> child1 = VarList(scanner);
    } else {
        error("; or IDENTIFIER");
    }                          

    return node;
}

// <exp> -> <M> / <exp> | <M> * <exp> | <M>
node *Exp(Scanner *scanner){
    node *node = createNode(EXPn);

    node-> child1 = M(scanner);   // Create <M> 

    if (tk.value == "/" || tk.value == "*"){
        node-> tk1 = tk;
        tk = (*scanner).getNext();
        node-> child2 = Exp(scanner);
    }         

    return node;
}

// <M> -> <N> + <M> | <N> 
node *M(Scanner *scanner){
    node *node = createNode(Mn);

    node-> child1 = N(scanner);  // Create <N>

    if (tk.value == "+"){
        node-> tk1 = tk;
        tk = (*scanner).getNext();
        node-> child2 = M(scanner);
    }

    return node;
}

// <N> -> <R> - <N> | ~ <N> | <R>
node *N(Scanner *scanner){
    node *node = createNode(Nn);

    if (tk.value == "~"){   // ~ <N>
        node-> tk1 = tk;
        tk = (*scanner).getNext();
        node-> child1 = N(scanner);
    } else {                // <R> - <N> || <R> case
        node-> child1 = R(scanner); // This will advance tk, we don't need to 
        if (tk.value == "-"){    // <R> - <N> case
            node-> tk1 = tk;
            tk = (*scanner).getNext();
            node-> child2 = N(scanner); 
        } 
    }

    return node;
}

// <R> -> (<exp>) | identifier | integer
node *R(Scanner *scanner){
    node *node = createNode(Rn);

    if (tk.value == "(") {      
        tk = (*scanner).getNext();    
        node-> child1 = Exp(scanner);
        if(tk.value != ")")
            error(")");        
    } else if (tk.type != "IDENTIFIER" && tk.type != "INTEGER") {
        error("IDENTIFIER OR INTEGER");
        //! come back to
    }else {
        node->tk1 = tk;
    }
        tk = (*scanner).getNext();  
    
    return node;
}

// <stats> -> <stat> <mStat>
node *Stats(Scanner *scanner){
    node *node = createNode(STATSn);
    node-> child1 = Stat(scanner);
    node-> child2 = Mstat(scanner);
    return node;
}

// <mStat> -> empty | <stat> <mStat> 
node *Mstat(Scanner *scanner){   // I think this code is good but too early to test
    node *node = createNode(MSTATn);
    if (tk.value == "xin" || tk.value == "xout" || tk.value == "{" || 
    tk.value == "xcond" || tk.value == "xloop" || tk.value == "xlet"){
        node-> child1 = Stat(scanner);
        node-> child2 = Mstat(scanner);   
    }
    return node;
}

// <stat> -> <in>|<out>|<block>|<if>|<loop>|<assign>
node *Stat(Scanner *scanner){
    node *node = createNode(STATn);      

    if (tk.value == "xin") {
        tk = (*scanner).getNext();
        node-> child1 = In(scanner);
    } else if (tk.value == "xout"){
        tk = (*scanner).getNext();
        node-> child1 = Out(scanner);
    } else if (tk.value == "{"){
        tk = (*scanner).getNext();
        node-> child1 = Block(scanner);
    } else if (tk.value == "xcond"){
        tk = (*scanner).getNext();
        node-> child1 = If(scanner);
    } else if (tk.value == "xloop"){
        tk = (*scanner).getNext();
        node-> child1 = Loop(scanner);
    } else if (tk.value == "xlet"){
        tk = (*scanner).getNext();
        node-> child1 = Assign(scanner);
    } else {
        error("xin or xout or { or xcond or xloop or xlet");
    }  

    return node;
}

// <block> -> { <vars> <stats> }
node *Block(Scanner *scanner){
    node *node = createNode(BLOCKn);

    node-> child1 = Vars(scanner);
    node-> child2 = Stats(scanner);

    if(tk.value != "}") {
        error("}");
    }
    tk = (*scanner).getNext();

    return node;
}

// <in> -> xin >> identifier; 
node *In(Scanner *scanner){
    node *node = createNode(INn);

    if (tk.value != ">>") {
        error(">>");    
    }
    tk = (*scanner).getNext();
    
    if (tk.type != "IDENTIFIER") {
        error("IDENTIFIER");    
    }
    node-> tk1 = tk;
    tk = (*scanner).getNext();

    if (tk.value != ";") {
        error(";");  
    }
    tk = (*scanner).getNext();

    return node;
}

// <out> -> xout << <exp>;
node *Out(Scanner *scanner){
    node *node = createNode(OUTn);

    if (tk.value != "<<") {
        error("<<"); 
    }
    tk = (*scanner).getNext();

    node-> child1 = Exp(scanner);    // <EXP>

    if (tk.value != ";") {
        error(";");       
    }  
    tk = (*scanner).getNext();
    
    return node;
}

// <if> -> xcond [ <exp> <RO>  <exp> ] <stat>
node *If(Scanner *scanner){
    node *node = createNode(IFn);

    if (tk.value != "[") {
        error("["); 
    }
    tk = (*scanner).getNext();

    node-> child1 = Exp(scanner);
    node-> child2 = R0(scanner);
    node-> child3 = Exp(scanner);

    if (tk.value != "]") {
        error("]"); 
    }
    tk = (*scanner).getNext();

    node-> child4 = Stat(scanner);

    return node;
}

// <loop> -> xloop [ <exp> <RO> <exp> ] <stat>
node *Loop(Scanner *scanner){
    node *node = createNode(LOOPn);

    if (tk.value != "[") {
        error("["); 
    }
    tk = (*scanner).getNext();

    node-> child1 = Exp(scanner);
    node-> child2 = R0(scanner);
    node-> child3 = Exp(scanner);

    if (tk.value != "]") {
        error("]"); 
    }
    tk = (*scanner).getNext();

    node-> child4 = Stat(scanner);

    return node;
}

// <assign> -> xlet identifier <exp>;  
node *Assign(Scanner *scanner){
    node *node = createNode(ASSIGNn);

    if (tk.type != "IDENTIFIER") {
       error("IDENTIFIER");    
    }
    node-> tk1 = tk;
    tk = (*scanner).getNext();   

    node-> child1 = Exp(scanner);

    if (tk.value != ";") {
        error(";"); 
    }
    tk = (*scanner).getNext();

    return node;
}

// <R0> -> <<(one token) | >> (one token) | < | > | = | % 
node *R0(Scanner *scanner){
    node *node = createNode(R0n);
    
    if (tk.value == "<<" || tk.value == ">>"){
        node-> tk1 = tk;
        tk = (*scanner).getNext();        
        return node;
    } 

    if (tk.value != "<" && tk.value != ">" && tk.value != "=" && tk.value != "%") {
        error("< or > or = or %");
    }
    node-> tk1 = tk;
    tk = (*scanner).getNext();

    return node;
}

//actual parser
node *parser(string input) {   
    Scanner scanner(input);      
    tk = scanner.getNext();

    node *root;
    root = Program(&scanner);   

    if (tk.type != "EOF"){
        error("EOF");
    }
    return root;
}

void error(string expected) {
	cout << "ERROR: line " << tk.line << ": " << expected << " tk expected";
	cout << tk.value << " token was received instead";
	exit(1);
}