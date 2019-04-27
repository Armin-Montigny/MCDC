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



