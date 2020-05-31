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
#ifndef MINTERMCALCULATOR_HPP
#define MINTERMCALCULATOR_HPP

//
// This class calculates a list with all Minterms from a boolean function
// It is used for extracting the prime implicants with the Quine&  McCluskey method.
//
// The boolena expression is evaluated for all possible input.
// If there are n input variables, there will be 2**n possible combinations of those
// input varaibles. All combinations will be used and the result of the boolean expression
// will be calculated. If the result is true, then the combination of the input variables
// form a minterm. For example, the boolean expression a+b will be evaluated
//
// d is the decimal equivalent for the input combinations, r is the result
//
// d  ab  r
// ---------
// 0  01  0
// 1  00  1
// 2  11  1
// 3  10  1
//
// The boolean function a+b (a or b) has the Minterms: 1,2,3
//
// All Minterms will be calculated and the result will be stored in a vector
// The complexity of the function is O(2**N). Meaning, needed space and calculation
// time grows geometrically. 
//
//  More than 12 variables cannot be used in a reasonable timeframe.
// This programm implements multithreading. But this is basically a meaningless approach
// Even running on 8 threads, you can do 3 variables more. And then?
// So, no further effort on runtime speed optimization
// 
// The resulting Mintermvector and the gathered Symbols will be used by other program
// Parts for further evaluation.

#include "types.hpp"
#include "compiler.hpp"
#include "virtualmachine.hpp"
#include "symboltable.hpp"






// Calculates a vector with minterms from a boolean expression given as string
class MintermCalculator
{
public:
	
	explicit MintermCalculator(std::string& booleanExpression) : objectCode(), compiler(booleanExpression, objectCode), vmfbe(), source(booleanExpression) {}
	MintermCalculator() = delete;
	MintermCalculator(const MintermCalculator&) = delete;
	MintermCalculator(const MintermCalculator&&) = delete;
	MintermCalculator& operator =(const MintermCalculator&) = delete;
	MintermCalculator& operator =(const MintermCalculator&&) = delete;

	// This function is made public, becuase we want to know, if the syntax of the boolean expression is OK.
	bool runCompiler() { objectCode.clear(); compilerResultOK = compiler(); return compilerResultOK; }
	// Calculate all minterms
	MintermVector calculate();
	
	// Return the symbol table. Important to know the number of symbols and for print purposes
	SymbolTable getSymbolTable() { return objectCode.symbolTable; }
	uint getNumberOfBooleanVariables() noexcept { return numberOfBooleanVariables; }

protected:
	// This is used for multithreading
	// we will split the calculation of Minterms in different input Ranges
	// The following type will give the start end the end value for the range
	using MintermRange = std::pair<MinTermType, MinTermType>;
	// And this is the vector with all ranges we will use
	using MintermRangeVector = std::vector<MintermRange>;

	// Depending on available threads and number of variables, ranges can be calculated
	MintermRangeVector calculateMintermRanges(uint numberOfRanges, uint upperBound);
	
	// This is the working horse. Run the virtual machine and calculate results
	MintermVector calculateAsThread(MintermRange mt, ObjectCode objectCode);

	// The resulting object code from the compiler
	// This is the input for the loader of the virtual machine
	ObjectCode objectCode;
	// And the compiler . . .
	CompilerForVM compiler;

	// Virtual machine used to calculate the result of an boolean expression for a given input
	VirtualMachineForBooleanExpressions vmfbe;

	// Number of varaibles in the boolean expression
	uint numberOfBooleanVariables{ 0U };

	// Result of compilation. If false, then the boolean expression has a syntax error
	bool compilerResultOK{ false };

	// Source, source code, or boolean expression
	std::string source;

};

// Show the truth table on the basis of calculated Minterms and the Symboltable
extern void printTruthTable(const std::string& source, MintermVector& mintermVector, const SymbolTable& symbolTable);


#endif