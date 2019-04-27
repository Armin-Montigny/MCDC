#include "pch.h"

#include "parser.hpp"

#include <iostream>
#include <iomanip>



bool Parser::parse()
{
	bool result = true;
	// std::cout << "Starting Parse\n";
	scanner.reset();
	codeGeneratorForVM->clear();

	const bool endWasReduced = false;
	bool doParse = true;

	TokenWithAttribute tokenWithAttribute;

	while (doParse)
	{
		tokenWithAttribute = scanner.getNextToken();
/*
		{
			static sint i = 0;
			std::cout << "*******           Get Next Token Parser run " << i++ << "  Token: " << std::setw(7) << tokenToCharP(tokenWithAttribute.token) << "    Token Source Index: " << tokenWithAttribute.sourceIndex << "  inputTerminalSymbol: " << tokenWithAttribute.inputTerminalSymbol << '\n';
		}
		*/
		if (Token::NONE == tokenWithAttribute.token)
		{
			// Error in input string
			result = false;
			doParse = false;
		}
		else
		{
			bool doMatchAndReduce = true;
			// Valid Token
			parseStack.push_back(tokenWithAttribute);    // ****** --> Shift
			if (programOption.option[ProgramOption::ppst].optionSelected) printParseStack("After Shift");

			while (doMatchAndReduce)
			{
				const auto[matchFound, indexOfMatchedProduction] = match();   // ****** --> Match
				if (matchFound)
				{
					if (programOption.option[ProgramOption::ppst].optionSelected) std::cout << "Match found. Start to reduce, Number of production: " << indexOfMatchedProduction << '\n';
					
					// ****** --> Generate Code
					reduce(indexOfMatchedProduction);				// ****** --> Reduce
					if (programOption.option[ProgramOption::ppst].optionSelected) printParseStack("After reduce");
				}
				doMatchAndReduce = matchFound;
			}
		}

		if (Token::END == parseStack[0].token)
		{
			// Normal end
			doParse = false;
		}
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

	for (const Production &production : grammar)
	{
		// The handle and the optional lookahead have a length. 
		// We can only compare a handle to stack elements, if there are enough elements on the stack
		// Get the minimum nneded number of stack elements from the production
		const uint lengthOfHandleIncludingLookahead = (production.hasLookAhead ? 1U : 0U) + production.numberOfTokensInHandle;


		//std::cout << "Match  top Of Stack: " << topOfStack << "    Length of Handle: " << lengthOfHandleIncludingLookahead << "    Production : " << production.id  << '\n';		


		if (static_cast<uint>(sizeOfStack) >= lengthOfHandleIncludingLookahead)
		{
			//std::cout << "SIZE OK " << topOfStack << "    Length of Handle: " << lengthOfHandleIncludingLookahead << "    Production : " << production.id  << '\n';		


			// index of stack Element to compare
			sint stackElementToCompare = (topOfStack);
			// We check for good lookahead
			// If there is no lookahead, then we still assume a OK comparison
			// If there is a look ahead, then the initial assumption is that lookahead has no match
			bool lookAheadOk = !production.hasLookAhead;
			// OK, we can start to compare. 
			// We will first concentrate on the lookahead symbols and the to top stack element
			Token stackTokenToCompare = parseStack[static_cast<uint>(stackElementToCompare)].token;



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
				uint indexInHandle = 0;
				const uint maxTokenInHandle = production.numberOfTokensInHandle;

				do
					//for (Token tokenInHandle : production.handle)
				{
					// Token on the stack
					stackTokenToCompare = parseStack[static_cast<uint>(stackElementToCompare)].token;
					const Token	handleTokenToCompare = production.handle[indexInHandle];

					// We want to do a complete match, if one does not fit, then result is false
					matchFound = matchFound && (/*(Token::NONE == handleTokenToCompare) ||*/ (stackTokenToCompare == handleTokenToCompare));

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



void Parser::reduce(uint positionOfProduction)
{
	TokenWithAttribute newTokenWithAttribute;
	uint stackPointer = narrow_cast<uint>(parseStack.size()) - 1;
	// Get reference to production
	const Production &production = grammar[positionOfProduction];

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
		//ast.add(production.operationIdentifier, parseStack, stackPointer, production.numberOfTokensInHandle);
		// Replace stack top with new token
		parseStack[stackPointer] = newTokenWithAttribute;
		break;
	case 2:
		// Call generator with 2 parameter
		newTokenWithAttribute = codeGeneratorForVM->generate(production, parseStack[stackPointer], parseStack[stackPointer - 1]);
		//ast.add(production.operationIdentifier, parseStack, stackPointer, production.numberOfTokensInHandle);

		// Delete stack top
		// Replace stack top -1 with new token
		parseStack[stackPointer - 1] = newTokenWithAttribute;
		parseStack.erase(parseStack.begin() + stackPointer);

		break;
	case 3:
		// Call code generator with 3 parameter
		newTokenWithAttribute = codeGeneratorForVM->generate(production, parseStack[stackPointer], parseStack[stackPointer - 1], parseStack[stackPointer - 2]);
		//ast.add(production.operationIdentifier, parseStack, stackPointer, production.numberOfTokensInHandle);


		// Delete stack top
		// Delete stack top - 1
		// Replace stack top -2 with new token
		parseStack[stackPointer - 2] = newTokenWithAttribute;

		parseStack.erase(parseStack.begin() + (static_cast<sll>(stackPointer) - 1), parseStack.begin() + (static_cast<sll>(stackPointer) + 1));

		break;
	}
}


void Parser::printParseStack(std::string title)
{
	const uint stackSize = narrow_cast<uint>(parseStack.size());
	std::cout << title << "  - Parse Stack Dump.  Size: " << stackSize << '\n';
	for (uint i = 0; i < stackSize; i++)
	{
		std::cout << "---- Stack Dump: " << i << "      Token: " << std::setw(7) << tokenToCharP(parseStack[i].token) << "      Token Source Index: " << parseStack[i].sourceIndex << "      inputTerminalSymbol: " << parseStack[i].inputTerminalSymbol << '\n';
	}
}


