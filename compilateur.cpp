//  A compiler from a very simple Pascal-like structured language LL(k)
//  to 64-bit 80x86 Assembly langage
//  Copyright (C) 2019 Pierre Jourlin
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Build with "make compilateur"


#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

char current;				// Current car	
char lookedAhead;			// Looked ahead
int NLookedAhead = 0;

void ReadChar(void) {	
	if(NLookedAhead > 0) { // If char has already been read
		current = lookedAhead;
		NLookedAhead -= 1;
	} else {
		while(cin.get(current) && (current==' '||current=='\t'||current=='\n'));
		cout << "JE LIS : " << current << endl;
	}
}

void LookAhead(void) {
	while(cin.get(current) && (current==' '||current=='\t'||current=='\n'));
	NLookedAhead += 1;
}

void Error(string s){
	cerr<< s << endl;
	exit(-1);
}

// Prototypage entier

void Program(void);
void DeclarationPart(void);
void StatementPart(void);
void Statement(void);
void AssignementStatement(void);

void Expression(void);
void SimpleExpression(void);
void Term(void);
void Factor(void);
void Number(void);

void AdditiveOperator(void);
void MultiplicativeOperator(void);
void RelationalOperator(void);
void Digit(void);
void Letter(void);

// Ecriture

// Program := [DeclarationPart] StatementPart
void Program(void) {
	cout << "PROGRAM" << endl;
	if(current == '[') {
		DeclarationPart();
	}
	StatementPart();
	ReadChar();
}

// DeclarationPart := "[" Letter {"," Letter} "]"
void DeclarationPart(void) {
	cout << "DECLARATION PART" << endl;
	if(current != '[') {
		Error("Mot clé '[' attendu.");
	}
	ReadChar();
	Letter();
	while(current == ',') {
		ReadChar();
		Letter();
	}
	if(current != ']') {
		Error("Mot clé ']' attendu.");
	}
	ReadChar();
}

// StatementPart := Statement {";" Statement} "."
void StatementPart(void) {
	cout << "STATEMENT PART" << endl;
	Statement();
	while(current == ';') {
		ReadChar();
		Statement();
	}
	cout << "JE VAIS CHECK" << endl;
	if(current != '.') {
		Error("Mot clé '.' attendu.");
	}
	cout << "END OF STATEMENT PART" << endl;
	ReadChar();
}

// Statement := AssignementStatement
void Statement(void) {
	cout << "STATEMENT" << endl;
	AssignementStatement();
}

// AssignementStatement := Letter "=" Expression
void AssignementStatement(void) {
	cout << "ASSIGNMENT STATEMENT" << endl;
	Letter();
	if(current != '=') {
		Error("Mot clé '=' attendu.");
	}
	ReadChar();
	Expression();
	cout << "END OF ASSIGNMENT STATEMENT" << endl;
}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
void Expression(void) {
	cout << "EXPRESSION" << endl;
	SimpleExpression();
	if(current=='='||current=='>'||current=='<') {
		RelationalOperator();
		SimpleExpression();
	}
	cout << "END OF EXPRESSION" << endl;
}

// SimpleExpression := Term {AdditiveOperator Term}
void SimpleExpression(void) {
	cout << "SIMPLE EXPRESSION" << endl;
	Term();
	while(current=='+'||current=='-'||current=='|') {
		AdditiveOperator();
		Term();
	}
}

// Term := Factor {MultiplicativeOperator Factor}
void Term(void) {
	cout << "TERM" << endl;
	Factor();
	while(current=='*'||current=='/'||current=='%'||current=='&') {
		MultiplicativeOperator();
		Factor();
	}
}

// Factor := Number | Letter | "(" Expression ")"| "!" Factor
void Factor(void) {
	cout << "FACTOR" << endl;
	if(isdigit(current)) {
		Number();
	} else if(isalpha(current)) {
		Letter();
	} else if(current=='(') {
		ReadChar();
		Expression();
		if(current!=')') {
			Error("Mot clé ')' attendu");
		}
		ReadChar();
	} else if(current == '!') {
		ReadChar();
		Factor();
	} else {
		Error("Facteur attendu");
	}
}

// Number := Digit{Digit}
void Number(void) {
	cout << "NUMBER" << endl;
	Digit();
	while(isdigit(current)) {
		Digit();
	}
}

// AdditiveOperator := "+" | "-" | "||"
void AdditiveOperator(void) {
	cout << "ADDITIVE OPERATOR" << endl;
	if(current=='+'||current=='-') {
		ReadChar();
	} else if(current=='|') {
		LookAhead();
		cout << "YOOOOO" << endl;
		if(lookedAhead == '|') {
			cout << "hahaheaaehae" << endl;
			ReadChar();
		}
	} else {
		Error("Opérateur additif attendu");
	}
}

// MultiplicativeOperator := "*" | "/" | "%" | "&&"
void MultiplicativeOperator(void) {
	cout << "MULTIPLICATIVE OPERATOR" << endl;
	if(current=='*'||current=='/'||current=='%'||current=='&') {
		ReadChar();
	} else {
		Error("Opérateur multiplicatif attendu");
	}
}

// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="
void RelationalOperator(void) {
	cout << "RELATIONAL OPERATOR" << endl;
	if(current=='='||current=='!'||current=='<'||current=='>') {
		ReadChar();
	} else {
		Error("Opérateur relationnel attendu");
	}
}

// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
void Digit(void){
	cout << "DIGIT" << endl;
	if((current<'0')||(current>'9'))
		Error("Chiffre attendu");		   // Digit expected
	else{
		cout << "\tpush $"<<current<<endl;
		ReadChar();
	}
}

// Letter := "a"|...|"z"
void Letter(void) {
	cout << "LETTER" << endl;
	if(current >= 'a' && current <= 'z') {
		ReadChar();
	} else {
		Error("Lettre attendue.");
	}
}


int main(void){	// First version : Source code on standard input and assembly code on standard output
	// Header for gcc assembler / linker
	cout << "\t\t\t# This code was produced by the CERI Compiler"<<endl;
	cout << "\t.text\t\t# The following lines contain the program"<<endl;
	cout << "\t.globl main\t# The main function must be visible from outside"<<endl;
	cout << "main:\t\t\t# The main function body :"<<endl;
	cout << "\tmovq %rsp, %rbp\t# Save the position of the stack's top"<<endl;

	// Let's proceed to the analysis and code production
	ReadChar();
	Program();
	ReadChar();
	// Trailer for the gcc assembler / linker
	cout << "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top"<<endl;
	cout << "\tret\t\t\t# Return from main function"<<endl;
	if(cin.get(current)){
		cerr <<"Caractères en trop à la fin du programme : ["<<current<<"]";
		Error("."); // unexpected characters at the end of program
	}

}
		
			





