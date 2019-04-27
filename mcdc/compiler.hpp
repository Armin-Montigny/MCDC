#pragma once
#ifndef COMPILER_HPP
#define COMPILER_HPP

// The main purpose of the whole program is to read a boolean expression,
// given as string and then do some evaluations, transformations and
// analysis.
// In order to be able to do this, the given string with a boolean expression
// must be transformed to a form that can be avaluated.
// // It es for exampel necessary to evaluate the boolean expression mathematically.
// So the varaibles will be set to all possible values and the result will be calculated.
// To achieve this, a virtaul machine is used to calculate the result.
// The compiler will scan and parser the boolena expression and then generate op code
// that can be run by the virtual machine.
// 
// A second compiler generates an abstract syntax tree AST for the boolean expression.
// The program does than operations on the AST to finally calculate test vectors
// for MCDC coverage
//
// This compiler is implemented as classical one pass multi stage algorithm
//
// - Minimum Preprocessing
// - Lexcical Analyis, generating Token with Attributes
// - Parser, based on given grammar in LALR(1). Shift Reduce Parser
// - Semenatic Anaylis for symbol table and AST creation
// - Optimizer
// - Code Geenrator
//
//
// This compiler impelementation uses a scanner for 
// - Minimum Preprocessing
// - Lexcical Analyis, generating Token with Attributes
// and a parser for 
// - Parser, based on given grammar in LALR(1). Shift Reduce Parser
// in one common front end. This common front end is defined in a common base class
//
// Since we want to transfrom the source code into 2 other representations
// -3 byte opcode
// or AST
// We will derive 2 compilers from the base compilers with just implementing a
// separate code generator doing
// - Semenatic Anaylis for symbol table and AST creation
// - Optimizer
// - Code Geenrator
//
//
// The compiler defined here has no own functions. Everything is delegated
// to subclasses with the Parser beeing the main working horse.
// There is even no "compiler.cpp" source module. 
// So this is just an abstractction of the compiler concept


#include "scanner.hpp"
#include "parser.hpp"
#include "codegenerator.hpp"

#include <string>


// Compiler front end
class Compiler
{
public:
	explicit Compiler(std::string &sourceCode, CodeGeneratorBase *codeGeneratorForParser) : scanner(sourceCode), parser(scanner, codeGeneratorForParser){}
	Compiler() = delete;
	Compiler& operator =(const Compiler &) = delete;
	virtual ~Compiler() {}

	// Run the compiler, or "compile", or "do the compilation"
	virtual bool operator()() { return parser.parse(); }	// The parser will do the work
protected:
	// Frontend has always a scanner (lexer) and a parser
	Scanner scanner;
	Parser parser;
};




// Compiles data from a string to a program readable by a virtual machine, VM

// This compiler will read a sourceCode (boolean expression) and create an object code
// It has a code generator (to build object code for a VM9 as the backend.
// The constructor sets up the whole functionality

class CompilerForVM : public Compiler
{
public:
	explicit CompilerForVM(std::string &sourceCode, ObjectCode &objectCode) : Compiler(sourceCode, &codeGeneratorForVM), codeGeneratorForVM(objectCode, sourceCode) {}
	CompilerForVM() = delete;
	CompilerForVM& operator =(const CompilerForVM&) = delete;

#pragma warning(suppress: 26443)
	virtual ~CompilerForVM() {}
protected:
	// The code generator makes the differens in the backend. Front end is identical and will be reused from base class
	CodeGeneratorForVM codeGeneratorForVM;
};



// Compiles data from a string to an AST (Abstract Syntax Tree)
class CompilerForAST : public Compiler
{
public:
	explicit CompilerForAST(std::string &sourceCode, VirtualMachineForAST &ast) : Compiler(sourceCode, &codeGeneratorForAST), codeGeneratorForAST(ast, sourceCode), virtualMachineForAST(ast){}
	CompilerForAST() = delete;
	CompilerForAST &operator =(const CompilerForAST&) = delete;
#pragma warning(suppress: 26443)
	virtual ~CompilerForAST() {}
	
	// Run the compiler, or "compile", or "do the compilation"
	virtual bool operator()() { bool rc = parser.parse(); if (rc) { virtualMachineForAST.calculateAstProperties(); } return rc; }	// The parser will do the work

protected:
	// The code generator makes the differens in the backend. Front end is identical and will be reused from base class
	CodeGeneratorForAST codeGeneratorForAST;
	VirtualMachineForAST& virtualMachineForAST;
};



#endif