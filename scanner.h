#ifndef SCANNER_H
#define SCANNER_H 

#include <cctype>
#include <iostream>
#include <string>


using namespace std; 


class Scanner {
	public:
		Scanner(const std::string& source) : source(source), position(0), line(1) {}

		struct Token {
			std::string type;
			std::string value;
			int line;
		};

		Token getNext(){
			Token tk = nextToken();
			if (tk.type != "COMMENT") {
				return tk;
			}
			return nextToken();
		}

		Token nextToken() {
			Token token;
			char current_char;

			while (position < source.size()) {
				current_char = source[position];

				//handle spaces
				if (std::isspace(current_char)) {
					if (current_char = '\n') {
						line++;
					}
					position++;
					continue;
				}

				if (std::isalpha(current_char) && islower(current_char)) {
					//not a keyword beginning with x
					if (current_char != 'x') {
						position++;
						continue;
					}

					token.type = "IDENTIFIER";
					while (position < source.size() && isalnum(source[position])) {
						if (token.value.size() < 8) {
							token.value += source[position++];
						} else {
							token.line = line;
							return token;
						}
					}

					if (token.value == "xopen" || token.value == "xclose" || token.value == "xdata" ||
						token.value == "xloop" || token.value == "xexit" || token.value == "xin" ||
						token.value == "xout" || token.value == "xthen" || token.value == "xlet" ||
						token.value == "xfunc" || token.value == "xcond") {
							token.type = "KEYWORD";
						}
						token.line = line;
						return token;
				}

				if (isIdentifier(current_char)) {
					scannerError("ERROR: Identifiers must start with 'x'");
					position++;
					continue;
				}

				// comment checking logic
				if (current_char == '$') {
					token.type = "COMMENT";
					position++;
					while (position < source.size() && source[position] != '$') {
						token.value += source[position++];
					}
					if (position < source.size() && source[position] == '$') {
						position++;
					} else {
						scannerError("ERROR: must start and close comments with '$'");
					}
					token.line = line;
					return token;
				}

				//check for operators
				if (isOperator(current_char)) {
					token.type = "OPERATOR";
					token.value += current_char;
					position++;
					if (position < source.size() && isOperator(source[position]) &&
						((token.value == "<") && (source[position] == *"<") ||
						(token.value == ">") && (source[position] == *">"))) {
							token.value += source[position++];
						}
						token.line = line;
						return token;
				}

				// check for integers
				if (std::isdigit(current_char)) {
					token.type = "INTEGER";
					while (position < source.size() && std::isdigit(source[position])) {
						if (token.value.size() < 8) {
							token.value += source[position++];
						} else {
							token.line = line;
							return token;
						}
					}
					token.line = line;
					return token;
				}
				scannerError("ERROR: Unexpected character input: " + current_char);
				position++;
			}
			token.type = "EOF";
			token.line = line;
			return token;
		}

		private:
			const std::string& source;
			size_t position;
			int line;

			bool isIdentifier(char c) {
				return isalnum(c) && islower(c);
			}
			bool isOperator(char c) {
				std::string operators = "=<>~:;+-*/%.(),{}[]";
				return operators.find(c) != std::string::npos;
			}

			void scannerError(const std::string& message) {
				std::cerr << "ERROR: lexical error at line- " << line << ": " << message << std::endl; 
			}
};

#endif 