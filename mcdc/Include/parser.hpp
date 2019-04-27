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
#ifndef PARSER_HPP
#define PARSER_HPP

//
// Implementation of a Shift Reduce Parser for boolean expressions
//
// This Software takes a string with a boolean expression as an input
// and transorms it to other representations, to be able to vealuate it
//
// The Parser, defined in this module, analyzes the given source (boolean expression)
// and generates code for a virtual machine or for an Abstract Syntax Tree AST
// or whatever, using a codegenerator.
//
//  First the source is tokenized via a scanner, then the Parser analyzes the Tokens
// and tries to match it wit the productions in the grammar. If a match is found,
// then the code generator is called.
//
// So, the algorithm is a classical shift match reduce parser.
//
// The whole chain consist of:
//
// Scanner --> Parser --> Code Generator --> Virtual Machine
//
// The parser tkaes a source string as input and generates an object file and a symbol table
//
// Operation:
// 
// The Parser calls the scanner to get the next token. The token will be shifted on the parse stack.
// Then, the top elements (one or more) of the parse stack will be compared to handles and
// the look ahead symnol in the grammar. If there is no match, the next token will be read
// and pushed on the stack. If there is a match, then the gode generator is called and the parse stack
// will be reduced. All matching elements on the parse stack (without the look ahead symbol) will be
// removed/replace by the non-terminal, the left hand side for/of this production. 
// This will be done until the end of the source string has been detected or a syntax error has been found.
//
// Tokens have attributes, specifically the input terminal symbols


#include "scanner.hpp"
#include "codegenerator.hpp"
#include "grammar.hpp"

#include <utility>



// The parse stack is a vector of Tokens (with attributes). Since we want o work on more elements as
// the stack top, a std::vector is used and not a std::stack
using ParseStack = std::vector<TokenWithAttribute>;


class Parser
{
public:
	// The parser needs to know about the scanner and the code geenrator
	// The Grammar is a runtime constant
	explicit Parser(Scanner& scannerForParser, CodeGeneratorBase* codeGeneratorForParser) noexcept : grammar(grammarForBooleanExpressions), scanner(scannerForParser), codeGeneratorForVM(codeGeneratorForParser), parseStack() {}
	Parser() = delete;
	Parser& operator =(const Parser&) = delete;

	// Main interface. Do the parse. Returns false on error
	bool parse();

protected:
	// Yes I know. These are references
	const Grammar& grammar;
	Scanner& scanner;
	CodeGeneratorBase* codeGeneratorForVM;

	// Match function compares top of parse stack with handles of the grammar
	std::pair<bool, uint> match();
	// Calls Code generator and reduces the parse stack, be erasing/replacing handle elements with a none terminal
	void reduce(uint positionOfProduction);

	ParseStack parseStack;

	// For debugging or learning purposes
	void printParseStack(std::string title);
};




#endif
