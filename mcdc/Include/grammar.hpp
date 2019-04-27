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
#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

// Classes and functions to deal with a Parsing for boolean expressions
// 
// The input to this software is a boolean expression given as string. The boolean expression consists
// of terminals/variables/conditions a-z and, in the negated form A-Z. Meaning A ist the same as (NOT a).
// As operators are defined: AND, OR, XOR, NOT, BRACKETS. 
//
// A compiler, sonsisting of a Scanner, Parser and Code Generator, reads the source boolean expression
// and converts it to either an Abstract syntax tree or a 4 byte virtual machine code
//
// The parser calls the sacenner (Lexer) to get tokens. The parser then matches the tokens with the
// grammer (defined in this file= and if there is a matchm reduces the parse stack.
//
// The grammer is defined here
//
// It is a left right Parser with 1 look ahead symbol:  LR(1)
// Additionally the comlexity is reduced by combining a handle with several Look ahaed symbols
// In essence the grammar is LALR(1) compatible
// 
// As terminals we do have the input variables/conditions of the boolean expression
// This is a lower case letter. Or, to indicate negation, an uppercase letter.
// Also the operators of the boolean expressions are terminals. Here we have
// OR, XOR, AND, Brackets and END
// The only none terminal is "expression"
// The productions are listed in the order of their priority.
//
//
// The parser checks the productions in the grammar from top to bottom.
//
// The precedence for binary operators is defined in the grammar and is
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

//
// The parser uses a classical shift match reduce approach.
//
// The grammar shown here is a constexpr array of productions


#include "token.hpp"
#include <array>


// Productions and associated handles

// idx		id	Production:									Handle				Look Ahead
//
// 0.		0	expression ::= ID							ID
// 1.   	1	expression ::= expression OR expression		EXPR OR EXPR		OR
// 2.   		expression ::= expression OR expression		EXPR OR EXPR		)
// 3.   		expression ::= expression OR expression		EXPR OR EXPR		END
// 4.   	2	expression ::= expression XOR expression	EXPR XOR EXPR		OR
// 5.   		expression ::= expression XOR expression	EXPR XOR EXPR		XOR
// 6.   		expression ::= expression XOR expression	EXPR XOR EXPR		)
// 7.   		expression ::= expression XOR expression	EXPR XOR EXPR		END
// 8.   	3	expression ::= expression AND expression	EXPR AND EXPR		OR
// 9.   		expression ::= expression AND expression	EXPR AND EXPR		XOR
// 10.   		expression ::= expression AND expression	EXPR AND EXPR		AND
// 11.   		expression ::= expression AND expression	EXPR AND EXPR		)
// 12.   		expression ::= expression AND expression	EXPR AND EXPR		END
// 13.  	4	expression ::= expression expression		EXPR EXPR			OR
// 14.  		expression ::= expression expression		EXPR EXPR			XOR
// 15.  		expression ::= expression expression		EXPR EXPR			AND
// 16.  		expression ::= expression expression		EXPR EXPR			)
// 17.  		expression ::= expression expression		EXPR EXPR			END
// 18.  	5	expression ::= NOT expression				NOT EXPR
// 19. 		6	expression ::= ( expression	)				BOPEN EXPR BCLOSE
// 20.  	7	expression ::= expression END				EXPR END

// How many elements, terminals and none-terminals can be in a handle
constexpr sint MaxNumberOfTokensInHandle{ 3 };
// Maximum number of lookahead symbol from one production
// Handles and lookahead symbols will be combined
constexpr sint MaxNumberOfTokensForLookAhead{ 5 };
// Overall maximum number of productions for our grammar
constexpr sint MaxNumberOfProductions{ 8 };


// Definition of one production. The grammar consists of MaxNumberOfProductions productions
struct Production
{
	const uint id;							// ID, for informational purposes. Production Identifier
	const cchar* productionAsText;			// The production as text in BNF format
	const Token operationIdentifier;		// For the action and for the code generator

	const bool hasLookAhead;				// Is look ahead needed to identify handle
	const uint numberofLookAheadTerminals;	// How many different handles, identified by lookahead, do we have
	const std::array<Token, MaxNumberOfTokensForLookAhead> lookAheadTerminalSymbol;	// Lookahead symbols

	const uint numberOfTokensInHandle;		// How many Tokens do we have in one handle. Needs to be combined with lookahead
	const std::array<Token, MaxNumberOfTokensInHandle> handle;					// Tokens in the handle

	Production() = delete;
	Production& operator =(const Production& ) = delete;
};

// The grammar is an arry of pruductions
using Grammar = std::array<Production, MaxNumberOfProductions>;


// And now the grammar. Defined as a constant / constexpr
constexpr Grammar grammarForBooleanExpressions
{ 							// Start of initializing the "production" variable, like sint sint a{0}
	{						// Start of array defintion for many productions

		{					// The array element. One Production 
			0U, 				// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= ID",
			Token::ID,

			false,			// Is a look ahead used?
			0U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::NONE, Token::NONE, Token::NONE, Token::NONE, Token::NONE
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			1U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::ID, Token::NONE, Token::NONE
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		},					// End of one array element, meaning one production

		{					// The array element. One Production 
			1U, 				// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= expression OR expression",
			Token::OR,

			true,			// Is a look ahead used?
			3U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::OR, Token::BCLOSE, Token::END, Token::NONE, Token::NONE
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			3U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::EXPR, Token::OR, Token::EXPR
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		},					// End of one array element, meaning one production

		{					// The array element. One Production 
			2U, 				// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= expression XOR expression",
			Token::XOR,

			true,			// Is a look ahead used?
			4U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::OR, Token::XOR, Token::BCLOSE, Token::END, Token::NONE
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			3U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::EXPR, Token::XOR, Token::EXPR
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		},					// End of one array element, meaning one production

		{					// The array element. One Production 
			3U, 				// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= expression AND expression",
			Token::AND,

			true,			// Is a look ahead used?
			5U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::OR, Token::XOR, Token::AND, Token::BCLOSE, Token::END
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			3U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::EXPR, Token::AND, Token::EXPR
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		},					// End of one array element, meaning one production

		{					// The array element. One Production 
			4U, 			// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= expression expression",
			Token::AND,

			true,			// Is a look ahead used?
			5U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::OR, Token::XOR, Token::AND, Token::BCLOSE, Token::END
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			2U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::EXPR, Token::EXPR, Token::NONE
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		},					// End of one array element, meaning one production

		{					// The array element. One Production 
			5U,				// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= NOT expression",
			Token::NOT,

			false,			// Is a look ahead used?
			0U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::NONE, Token::NONE, Token::NONE, Token::NONE, Token::NONE
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			2U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::EXPR, Token::NOT, Token::NONE
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		},					// End of one array element, meaning one production


		{					// The array element. One Production 
			6U, 			// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= ( expression )",
			Token::BCLOSE,

			false,			// Is a look ahead used?
			0U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::NONE, Token::NONE, Token::NONE, Token::NONE, Token::NONE
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			3U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::BCLOSE, Token::EXPR, Token::BOPEN
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		},					// End of one array element, meaning one production

		{					// The array element. One Production 
			7U, 				// ID of production, just for information
							// Desciption of production, just for information
			"expression ::= expression END",
			Token::END,

			false,			// Is a look ahead used?
			0U,				// In case of needed lookahead, how many tokens do we have
			{				// Start of initializing the Token list of look ahead symbols
				{			// Begin of the array of lookahead Tokens
					Token::NONE, Token::NONE, Token::NONE, Token::NONE, Token::NONE
				}			// End of array of lookahead Tokens
			},				// End of initializing the Token list of look ahead symbols

			2U,				// Number of Tokens in handle
			{				// Start of initializing the Token list for the handle
				{			// Begin of the array 
					Token::END, Token::EXPR, Token::NONE
				}			// End of array of Tokens for the handle
			}				// Start of initializing the Token list for the handle
		}					// End of one array element, meaning one production

	} // End of the main array
};	// End of initializing the "production" variable




#endif
