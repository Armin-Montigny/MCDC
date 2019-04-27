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
// The scanner reads the source code and tokenizes everything.
// So, it will analyze the input, check it, combine characters and return Tokens to the Parser
// The Parser will then match sequences of tokens to handles in the grammar
//
// Tokens will be used together with attributes
// This will help to pass information to the codegenerator, especially for the variables
//



#include "token.hpp"


// Convert the enum token to a onst char, so that we can output readable text
const cchar* tokenToCharP(Token t) noexcept
{
	switch (t)
	{
	case Token::NONE:
		return "NONE";
		break;
	case Token::ID:
		return "ID";
		break;
	case Token::IDNOT:
		return "IDNOT";
		break;
	case Token::OR:
		return "OR";
		break;
	case Token::XOR:
		return "XOR";
		break;
	case Token::AND:
		return "AND";
		break;
	case Token::NOT:
		return "NOT";
		break;
	case Token::BOPEN:
		return "BOPEN";
		break;
	case Token::BCLOSE:
		return "BCLOSE";
		break;
	case Token::END:
		return "END";
		break;
	case Token::EXPR:
		return "EXPR";
		break;
	default:
		return "Error";
		break;
#pragma warning(suppress: 4062)
#pragma warning(suppress: 4061)
	}
}
