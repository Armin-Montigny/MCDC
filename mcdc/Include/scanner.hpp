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
#ifndef SCANNER_HPP
#define SCANNER_HPP

// Scanner (Lexer) for boolean expression
//
// The Scanner (Lexer) is a part of the compiler. It reads the source string and combines 
// input source characters to tokens. The Tokens are returned to the Parser
//
// The input source string can consist of
//
// 1. Conditions/BooleanVariables/Literals/Letters. These terms are used synonymously
//
// Allowed are all Latin characters a-z and A-Z
//
// Capital letters are the negated form of lowercase letters
// So:   A   is equal to   !a
//
// Please note: Even so all letters of the alphabet are allowed, the maximum number of 
// different letters is limited to a smaller number
//
// 2. White Spaces.
//
// White spaces are allowed and will be ignored.
// White spaces are not allowed in multi character operators
//
// 3. Operators
//
//   OR Operator:  '|' or  '||'  or   '+'
//	XOR Operator:  '^' 
//  AND Operator:  '&' or  ' &&'  or   '*'  or  concatenation. 
//					"ab" means a AND b. 
//					Also valid for brackets (a+b)(c+d) --> (a OR b) AND (c OR d)
//					Even "(a)(b)" works and means a AND b
//  NOT Operator:  '!'  or  '~'
//  Brackets:		'(' or ')'   Change operator precedence or group sub expressions
//
//
// The precedence for binary operators is defined in the grammar and is:
// Binary Operator Precedence, left associative:  Brackets AND XOR OR
// Unary, right associative:  NOT
// --> NOT > Brackets > AND > XOR > OR
//
// "a+b^c!(d+e)"
// will result in a syntax tree of the form:
//
// 0            ID a(0)
// 1
// 2 OR 0, 4 (0)            ID b(0)
// 3
// 4            XOR 2, 6 (0)           ID c(0)
// 5
// 6                       AND 4, 8 (0)                      ID d(0)
// 7                                             OR 6, 8 (0)
// 8                                  NOT 7 (0)             ID e(0)
// 9

// Because of simplicity of the "language" boolean expression, the scanner is handcoded




#include "token.hpp"

#include <string>

class Scanner
{
public:
	// The source code is a boolean expression
	Scanner(const std::string& bExp) : sourceCode(bExp) { reset(); }

	// Main Interface function. Called by the parser
	TokenWithAttribute getNextToken();

	// Reset iterator for source string to be abble to do the next parse
	void reset() noexcept { currentElement = sourceCode.begin(); }

protected:
	std::string sourceCode;
	// Current evealuated character in source code
	std::string::iterator currentElement;
};



#endif
