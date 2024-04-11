#include <iostream> 
#include <fstream> 
#include <iomanip>
#include <string>
#include <set>
#include <cctype>

#include "scanner.h"
#include "testTree.h" 
#include "parser.h" 
#include "staticSemantics.h"
#include "compiler.h"

using namespace std; 

int main(int argc, char *argv[]) {
	std::string outputFileName;
	std::ifstream inputFile;
	std::string input;
	
	if (argc > 2) {    
        std::cerr << "Too many arguments" << std::endl;
        return 1;
    } else if (argc == 1) {        
        std::cout << "Needs to read from keyboard..." << std::endl;
        std::string line;

        while (std::getline(std::cin, line)) {
            input += line + '\n';  
        } 
        outputFileName = "a.asm";
    } else {
        outputFileName = std::string(argv[1]) + ".asm";   
        std::string filename = std::string(argv[1]) + ".f23"; 
        inputFile.open(filename);
        if (!inputFile) {
            std::cerr << "Can't open file..." << std::endl;  
            return 1;
        } else {
            std::cout << "File opened successfully!" << std::endl;  

	        std:: string line;       
            while (std::getline(inputFile, line)) {
                input += line + '\n';   
            }     
        }
    }

	std::cout << "\nSource Program:\n" << input << "\nParse Tree Output: \n";
	node *root = parser(input);
	traversePreorder(root, 0);
	bool semanticsError = staticSemantics(root, 0);
    if(semanticsError) {
        cout << "SEMANTICS ERROR returned.\n";
        return 0;
    }
    FILE *outFile = fopen(outputFileName.c_str(), "w"); 
    if(!outFile){
        cout << ".asm file creation failed";
        return 0;
    }
    recGen(root, outFile);          
    fclose(outFile);        

    outFile = fopen(outputFileName.c_str(), "r");    
    if (!outFile) {
        std::cout << "Failed to reopen file for reading" << std::endl;
        return 1;
    }

    int character;
    while ((character = fgetc(outFile)) != EOF)     
        putchar(character);    
    fclose(outFile);       
    return 0;
}

