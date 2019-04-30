// --------------------------------------------------------------------------------------------------------------------------------
// License:	BSD-3-Clause
// --------------------------------------------------------------------------------------------------------------------------------
//
// Copyright 2019      Armin Montigny
//
// --------------------------------------------------------------------------------------------------------------------------------
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
// following conditions are met :
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the 
//    following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the 
//    following disclaimer in the documentation and /or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or 
//    promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------------------------------------------------------------


#pragma once
#ifndef CODEGENERATOR_HPP
#define CODEGENERATOR_HPP

// This software implements 2 compiler that take a boolean
// expression as input and produce 2 outputs
//
// 1. An object code that can be run be a virtual machine
// 2. An abstract syntax tree (AST) for calculation of MCDC test pairs
//
//
// The compiler frontent, the scanner and the parser are the same
// The "code generator" makes the difference.
//
// The input of the code generator is the output of the parser.
// The parser, implemented as shift reduce parser, reads a tokrn
// shifts it on the parse stack, and tries to match it with a handle
// given in the grammar. The parsesatck is then reduced according
// to the size of the handle and this code generator will be
// invoked with the handle elements as parameter.
// The grammar, defined here for the boolean expressions, defines
// handles with 1, 2 or 3 elements. Therefore the code generator
// implements 3 overloaded "geneate" functions, that will be used
// for the respective handle.
//
// The first code generator builds an object file with so called
// 3 byte opcodes. There is always an operation, 1 or 2 source
// registers and a target or result register.
// 
// A virtual machine loads the generated object code and executes it.
//
//
// The 2nd code geenrator builds an abstract syntax tree AST. This is 
// implemented as a vector with internal linkage fields. See ast.h
// for further desctriptions
//
// Also the AST can be "executed" to generate results.
//
// Both code generators are derived from a common base clase, because
// they share the same compiler front end.

#include "types.hpp"
#include "token.hpp"
#include "grammar.hpp"
#include "ast.hpp"





// Unlinked Object Code Line. The code genrator generates op codes. This is a operation
// and up to 3 parameters. The parameters will be indices to virtual machine registers
struct OpCodeLine
{
	Token token{ Token::NONE }; // Reuse of Token for operation type
	uint parameter1{ 0U };
	uint parameter2{ 0U };
	uint parameter3{ 0U };
};

// The Object code is a vector of Op Code lines + a symbol table
// The symbol table stores the name of the boolean variable and some helper values
struct ObjectCode 
{
	void clear() noexcept { opCode.clear(); symbolTable.symbol.clear(); }
	void add(const OpCodeLine && ocl) { opCode.push_back(ocl); } // convenience function

	std::vector<OpCodeLine> opCode;
	SymbolTable symbolTable;
};



// Base class for 2 code generators.
// Defines 3 generate functions which will be called from the parser
// The source, the original boolean expression, is just needed for debug info and
// output purposes (No technical need)
class CodeGeneratorBase
{
public: 
	explicit CodeGeneratorBase(std::string& s) : source(s)  {}
	virtual ~CodeGeneratorBase() {}

	CodeGeneratorBase() = delete;
	CodeGeneratorBase(const CodeGeneratorBase&) = delete;
	CodeGeneratorBase(const CodeGeneratorBase&&) = delete;
	CodeGeneratorBase& operator =(const CodeGeneratorBase&) = delete;
	CodeGeneratorBase& operator =(const CodeGeneratorBase&&) = delete;


	virtual void clear() = 0;	// Resets previously generated output
	// The main "generate functions". These will be called by the parser, depending and the size of the handle in the grammar
	virtual TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1) = 0;
	virtual TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1, TokenWithAttribute& tokenWithAttribute2) = 0;
	virtual TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1, TokenWithAttribute& tokenWithAttribute2, TokenWithAttribute& tokenWithAttribute3) = 0;
protected:
	// For debug and output purposes. Not needed to generate the code
	std::string source;
};


// Code Generator for 3 Byte op code. Virtual Registers for the virtual machine will be used to store intermediate results
// Also a symbol table, with all the input symbols will be generated
class CodeGeneratorForVM : public CodeGeneratorBase
{
public:
	explicit CodeGeneratorForVM(ObjectCode& objectCode, std::string& source) :CodeGeneratorBase(source), objectCode(objectCode), availabeRegister() {}
	CodeGeneratorForVM() = delete;
	CodeGeneratorForVM& operator =(const CodeGeneratorForVM&) = delete;
	CodeGeneratorForVM& operator =(const CodeGeneratorForVM&&) = delete;
	CodeGeneratorForVM(const CodeGeneratorForVM&) = delete;
	CodeGeneratorForVM(const CodeGeneratorForVM&&) = delete;
	~CodeGeneratorForVM() {}

#pragma warning(suppress: 26440)
	void clear() override { objectCode.clear(); }
	// Compacts the symbol table. The boolean expression may contain terms like "a+c", without a "b". So it has 2 variables. Not 3
	// This will be reflected with the compact function
	void compactSymbolTable() { objectCode.symbolTable.compact();  }

	TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1) override;
	TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1, TokenWithAttribute& tokenWithAttribute2) override;
	TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1, TokenWithAttribute& tokenWithAttribute2, TokenWithAttribute& tokenWithAttribute3) override;
protected:
	ObjectCode& objectCode;
	std::vector<bool> availabeRegister;

	// Finds avaliable register numbers. Registers will be reused, if they are no longer needed. The following
	// functions will handle the register allocation
	uint getNextAvailableRegister();
	void setRegisterToAvailable(uint index);
};





// Code generator used to build an abstract syntax tree AST for boolean expressions
class CodeGeneratorForAST : public CodeGeneratorBase
{
public:
	explicit CodeGeneratorForAST(VirtualMachineForAST& a, std::string& source) : CodeGeneratorBase(source), ast(a) {}
	CodeGeneratorForAST() = delete;
	CodeGeneratorForAST(const CodeGeneratorForAST&) = delete;
	CodeGeneratorForAST(const CodeGeneratorForAST&&) = delete;
	CodeGeneratorForAST& operator=(const CodeGeneratorForAST&) = delete;
	CodeGeneratorForAST& operator=(const CodeGeneratorForAST&&) = delete;

	~CodeGeneratorForAST() {}

#pragma warning(suppress: 26440)
	void clear() override { ast.clear(); currentIndex = 0U; }

	TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1) override;
	TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1, TokenWithAttribute& tokenWithAttribute2) override;
	TokenWithAttribute generate(const Production& production, TokenWithAttribute& tokenWithAttribute1, TokenWithAttribute& tokenWithAttribute2, TokenWithAttribute& tokenWithAttribute3) override;

protected:
	// Surrounding Container for AST nodes
	VirtualMachineForAST& ast;
	// Counter for nodes in the AST
	uint currentIndex{ 0U };
};

#endif
