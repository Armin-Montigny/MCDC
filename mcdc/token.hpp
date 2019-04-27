#pragma once
#ifndef TOKEN_HPP
#define TOKEN_HPP
//
// The scanner reads the source code and tokenizes everything.
// So, it will analyze the input, check it, combine characters and return Tokens to the Parser
// The Parser will then match sequences of tokens to handles in the grammar
//
// Tokens will be used together with attributes
// This will help to pass information to the codegenerator, especially for the variables
//

#include "types.hpp"
#include <cstdint>


enum class Token :uint8_t 
{ 
	NONE, 
	ID, 
	OR, 
	XOR, 
	AND, 
	NOT, 
	BOPEN, 
	BCLOSE, 
	END, 
	EXPR, 
	IDNOT 
};


struct TokenWithAttribute
{
	Token token{ Token::NONE };

	// The index of from a normalized input. a=0, b=1 . . .
	uint sourceIndex{ 0U };		

	// Bit Mask for normalized input. Since the alphabet has only 26 letters, a 32 bit variable is enough
	uint sourceMask{ 0U };

	// The original imput symbol
	cchar inputTerminalSymbol{ '.' };
	// And the same in lower case
	cchar inputSymbolLowerCase{ ' ' };
};



// If there is a need to print Tokens . . .
extern const cchar *tokenToCharP(Token t) noexcept;
#endif
