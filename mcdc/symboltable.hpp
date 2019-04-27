#pragma once
#include "types.hpp"

// Symboltable for boolean expressions
// 
// Will contain all conditions/variables/terminals/letters (Interchangeable terms) of a boolean expressions
//
// So can contain letters a-z and A-Z
// 
// Since boolean expressions must have a limited number of varaibles, there may be letters between a-z in it.
// But with gaps. So for example "(a+b)k" contains a,b,k.  For calculation purposes it only contains
// 3 variables. This we will use in the normalized input. It will map all letters to a gapless sequence.

#include <set>
#include <map>


using SymbolType = std::set<cchar>;
struct SymbolTable
{
	// All input symbols, so possible something betwenn a-z and A-Z
	SymbolType symbol;

	// Mapping of input symbols to a consecutive list
	std::map<uint, uint> normalizedInput;

	// Get the number of overall symbols. Lowercase character and uppercase character will be counted only once
	const uint numberOfSymbols() const noexcept { return narrow_cast<uint>(symbol.size()); }

	// Create Normalize Input from symbol
	void compact();
};



