//
// The scanner reads the source code and tokenizes everything.
// So, it will analyze the input, check it, combine characters and return Tokens to the Parser
// The Parser will then match sequences of tokens to handles in the grammar
//
// Tokens will be used together with attributes
// This will help to pass information to the codegenerator, especially for the variables
//


#include "pch.h"
#include "token.hpp"


// Convert the enum token to a onst char, so that we can output readable text
const cchar *tokenToCharP(Token t) noexcept
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
