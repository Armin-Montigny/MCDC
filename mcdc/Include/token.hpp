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
extern const cchar* tokenToCharP(Token t) noexcept;
#endif
