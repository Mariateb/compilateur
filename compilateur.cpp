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


char[] letters = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'];
char[] digits = ['0','1','2','3','4','5','6','7','8','9'];

// Vérifications
bool isDigit(char s) {
	for(int i = 0; i < 10; i ++) {
		if(s == digits[i]) {
			return true;
		}
	}
	return false;
}
bool isLetter(char s) {
	for(int i = 0; i < 26; i ++) {
		if(s == letters[i]) {
			return true;
		}
	}
	return false;
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
	if(current == '[') {
		DeclarationPart();
	}
	StatementPart();
	ReadChar();
}

// DeclarationPart := "[" Letter {"," Letter} "]"
void DeclarationPart(void) {
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
	Statement();
	while(current == ';') {
		ReadChar();
		Statement();
	}
	if(current != '.') {
		Error("Mot clé '.' attendu.");
	}
	ReadChar();
}

// Statement := AssignementStatement
void Statement(void) {
	AssignementStatement();
	ReadChar();
}

// AssignementStatement := Letter "=" Expression
void AssignementStatement(void) {
	Letter();
	if(current != '=') {
		Error("Mot clé '=' attendu.");
	}
	ReadChar();
	Expression();
}

// Expression := SimpleExpression [RelationalOperator SimpleExpression]
void Expression(void) {
	SimpleExpression();
	if(current=='='||current=='>'||current=='<') {
		RelationnalOperator();
		SimpleExpression();
	}
}

// SimpleExpression := Term {AdditiveOperator Term}
void SimpleExpression(void) {
	Term();
	while(current=='+'||current=='-'||current='|') {
		AdditiveOperator();
		Term();
	}
}

// Term := Factor {MultiplicativeOperator Factor}
void Term(void) {
	Factor();
	while(current=="*"||current=="/"||current=="%"||current=="&") {
		MultiplicativeOperator();
		Factor();
	}
}

// Factor := Number | Letter | "(" Expression ")"| "!" Factor
void Factor(void) {
	if(isDigit(current)) {
		Number();
	} else if(isLetter(current)) {
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


// AdditiveOperator := "+" | "-" | "||"
// MultiplicativeOperator := "*" | "/" | "%" | "&&"
// RelationalOperator := "==" | "!=" | "<" | ">" | "<=" | ">="  
// Digit := "0"|"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
// Letter := "a"|...|"z"



void AdditiveOperator(void){
	if(current=='+'||current=='-'||"|")
		ReadChar();
	else
		Error("Opérateur additif attendu");	   // Additive operator expected
}
		
void Digit(void){
	if((current<'0')||(current>'9'))
		Error("Chiffre attendu");		   // Digit expected
	else{
		cout << "\tpush $"<<current<<endl;
		ReadChar();
	}
}

void Term(void){
	if(current=='('){
		ReadChar();
		ArithmeticExpression();
		if(current!=')')
			Error("')' était attendu");		// ")" expected
		else
			ReadChar();
	}
	else 
		if (current>='0' && current <='9')
			Digit();
	     	else
			Error("'(' ou chiffre attendu");
}

void ArithmeticExpression(void){
	char adop;
	Term();
	while(current=='+'||current=='-'){
		adop=current;		// Save operator in local variable
		AdditiveOperator();
		Term();
		cout << "\tpop %rbx"<<endl;	// get first operand
		cout << "\tpop %rax"<<endl;	// get second operand
		if(adop=='+')
			cout << "\taddq	%rbx, %rax"<<endl;	// add both operands
		else
			cout << "\tsubq	%rbx, %rax"<<endl;	// substract both operands
		cout << "\tpush %rax"<<endl;			// store result
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
	ArithmeticExpression();
	ReadChar();
	// Trailer for the gcc assembler / linker
	cout << "\tmovq %rbp, %rsp\t\t# Restore the position of the stack's top"<<endl;
	cout << "\tret\t\t\t# Return from main function"<<endl;
	if(cin.get(current)){
		cerr <<"Caractères en trop à la fin du programme : ["<<current<<"]";
		Error("."); // unexpected characters at the end of program
	}

}
		
			





