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
//


#include "parser.hpp"

#include <iostream>
#include <iomanip>


// Shift reduce parser
bool Parser::parse()
{

	bool result{ true }; // Result of the parsing process. True is OK

	// Reset the scanner. Start the scanning the source string from the beginning
	scanner.reset();
	// And reinitilize the code generator
	codeGeneratorForVM->clear();

	// Interface to scanner
	TokenWithAttribute tokenWithAttribute;

	bool doParse{ true };
	while (doParse)
	{
		// Call scanner/Lexer and get the next token
		tokenWithAttribute = scanner.getNextToken();
		if (Token::NONE == tokenWithAttribute.token)
		{
			// Error in input string
			result = false;
			doParse = false;
		}
		else
		{
			// So, we found a new Valid Token
			// ..............................................................	-- > Shift
			parseStack.push_back(tokenWithAttribute);    

			// Debug output, if selected
			if (programOption.option[ProgramOption::ppst].optionSelected) printParseStack("After Shift");

			// Try to match  the parse stack top with a handle in the grammar
			// After a match, reduce and try to match again
			bool doMatchAndReduce{ true };
			while (doMatchAndReduce)
			{
				// ..........................................................	-- > Match
				const auto[matchFound, indexOfMatchedProduction] = match();  
				if (matchFound)
				{
					// Debug output, if selected
					if (programOption.option[ProgramOption::ppst].optionSelected) std::cout << "Match found. Start to reduce, Number of production: " << indexOfMatchedProduction << '\n';
					
					// ......................................................	-- > Reduce
					reduce(indexOfMatchedProduction);
					// ......................................................	-- > Generate code

					// Debug output, if selected
					if (programOption.option[ProgramOption::ppst].optionSelected) printParseStack("After reduce");
				}
				// As long as a match is found, try to match again . . .
				doMatchAndReduce = matchFound;
			}
		}

		// We read the end token on the parse stack position 0
		// So everything is reduced and END is there
		// This is a normal end of parse
		if (Token::END == parseStack[0].token)
		{
			// Normal end
			doParse = false;
		}
		// If there is an END and not on parse stack position 0, then we have an error
		// because not everything could be matched to the grammar and reduced
		else if (Token::END == tokenWithAttribute.token)
		{
			// Syntax Error
			std::cerr << "*** Error: Snytax Error in boolean expression\n\n";
			result = false;
			doParse = false;
		}
	}
	return result;
}


// Match the top of the parse stack against a handle in the grammar
std::pair<bool, uint>  Parser::match()
{
	// Return values
	bool matchFound{ false };
	uint indexOfMatchedProduction{ 0 };

	// Get the top of the stack
	const sint sizeOfStack{ narrow_cast<sint>(parseStack.size()) };
	const sint topOfStack{ sizeOfStack - 1 };

	// Now we want to compare the parse stack with the handle in the productions
	// So we will iterate through all productions and do compare with the elements on the stack

	for (const Production& production : grammar)
	{
		// The handle and the optional lookahead have a length. 
		// We can only compare a handle to stack elements, if there are enough elements on the stack
		// Get the minimum nneded number of stack elements from the production

		const uint lengthOfHandleIncludingLookahead{ (production.hasLookAhead ? 1U : 0U) + production.numberOfTokensInHandle };
		//std::cout << "Match  top Of Stack: " << topOfStack << "    Length of Handle: " << lengthOfHandleIncludingLookahead << "    Production : " << production.id  << '\n';		

		if (static_cast<uint>(sizeOfStack) >= lengthOfHandleIncludingLookahead)
		{
			//std::cout << "SIZE OK " << topOfStack << "    Length of Handle: " << lengthOfHandleIncludingLookahead << "    Production : " << production.id  << '\n';		
			// index of stack Element to compare
			sint stackElementToCompare{ topOfStack };
			// We check for good lookahead
			// If there is no lookahead, then we still assume a OK comparison
			// If there is a look ahead, then the initial assumption is that lookahead has no match
			bool lookAheadOk{ !production.hasLookAhead };
			// OK, we can start to compare. 
			// We will first concentrate on the lookahead symbols and the to top stack element
			Token stackTokenToCompare{ parseStack[static_cast<uint>(stackElementToCompare)].token };

			// If there is a look ahead at all
			if (production.hasLookAhead)
			{
				// Iterate over all look ahead tokens
				for (const Token tokenLookahead : production.lookAheadTerminalSymbol)
				{
					// Token lookahead maybe Token::none and never compare to true. 
					// This does not matter since we "or" everything together
					// So we are checking for one good result
					lookAheadOk = lookAheadOk || (stackTokenToCompare == tokenLookahead);
				}
				// Lookahead consumed one byte of the stack
				--stackElementToCompare;
			}

			// Either there was no lookahead or the look ahead matched 
			if (lookAheadOk)
			{
				// Now assume that we have a match, but only if there are tokens in the handle
				matchFound = true;
				// Then compare the stack with the handle
				// Iterate over all elements in the handle
				uint indexInHandle{ 0 };
				const uint maxTokenInHandle{ production.numberOfTokensInHandle };

				do
				{
					// Token on the stack
					stackTokenToCompare = parseStack[static_cast<uint>(stackElementToCompare)].token;
					const Token	handleTokenToCompare{ production.handle[indexInHandle] };

					// We want to do a complete match, if one does not fit, then result is false
					matchFound = matchFound && (stackTokenToCompare == handleTokenToCompare);

					--stackElementToCompare;
					++indexInHandle;
				} while ((0 <= stackElementToCompare) && (indexInHandle < maxTokenInHandle) && matchFound);
			}
		}
		if (matchFound)
		{
			indexOfMatchedProduction = production.id;
			break;
		}
	}

	return std::make_pair(std::move(matchFound), std::move(indexOfMatchedProduction));
}


// Reduce
// After the top of the parse stack has been positively compared against 
// a handle in the grammar, we no the index of the production in the grammar
// and can generate code and reduce the parse stack. Meaning, Eliminiating
// matched elements and put a non terminal (in our case EXPR) into the stack
void Parser::reduce(uint positionOfProduction)
{
	TokenWithAttribute newTokenWithAttribute;	// Result of codegenerator. New stack element
	uint stackPointer = { narrow_cast<uint>(parseStack.size()) - 1 };

	// Get reference to production
	const Production& production{ grammar[positionOfProduction] };

	if (production.hasLookAhead)
	{
		--stackPointer;
	}
	//std::cout << "Reduce,  Production: " << positionOfProduction << "    Stack Pointer: " << stackPointer << "    Number of Tokens in Handle: " << production.numberOfTokensInHandle << '\n';
	switch (production.numberOfTokensInHandle)
	{
	case 1:
		// Call generator with 1 parameter
		newTokenWithAttribute = codeGeneratorForVM->generate(production, parseStack[stackPointer]);
		// Replace stack top with new token
		parseStack[stackPointer] = newTokenWithAttribute;
		break;
	case 2:
		// Call generator with 2 parameter
		newTokenWithAttribute = codeGeneratorForVM->generate(production, parseStack[stackPointer], parseStack[stackPointer - 1]);
		// Delete stack top
		// Replace stack top -1 with new token
		parseStack[stackPointer - 1] = newTokenWithAttribute;
		// Erase elements from the stack, that have been reduced
		parseStack.erase(parseStack.begin() + stackPointer);

		break;
	case 3:
		// Call code generator with 3 parameter
		newTokenWithAttribute = codeGeneratorForVM->generate(production, parseStack[stackPointer], parseStack[stackPointer - 1], parseStack[stackPointer - 2]);
		// Delete stack top
		// Delete stack top - 1
		// Replace stack top -2 with new token
		parseStack[stackPointer - 2] = newTokenWithAttribute;
		// Erase elements from the stack, that have been reduced
		parseStack.erase(parseStack.begin() + (static_cast<sll>(stackPointer) - 1), parseStack.begin() + (static_cast<sll>(stackPointer) + 1));
		break;
	}
}

// Debug print of the parse stack. Will slways go to std::cout
void Parser::printParseStack(std::string title)
{
	const uint stackSize = narrow_cast<uint>(parseStack.size());
	// Print always the complete parse stacl
	std::cout << title << "  - Parse Stack Dump.  Size: " << stackSize << '\n';
	for (uint i = 0; i < stackSize; i++)
	{
		std::cout << "---- Stack Dump: " << i << "      Token: " << std::setw(7) << tokenToCharP(parseStack[i].token) << "      Token Source Index: " << parseStack[i].sourceIndex << "      inputTerminalSymbol: " << parseStack[i].inputTerminalSymbol << '\n';
	}
}


