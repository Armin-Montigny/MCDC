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



#include "mintermcalculater.hpp"
#include "cloption.hpp"

#include <future>
#include <iostream>
#include <bitset>
#include <iomanip>



// Calculating Minterms is slow
// And the complexity is O(2^n). Meaning Rowing expeonentially
// SO, for biiger number of boolean variables, we use multi threading
// One tread calculates a range of Minterms
// The ranges for the trheads are claculated in this function
// numberOfRanges is equal to number of threads. Upper bound is 2^number of variables
MintermCalculator::MintermRangeVector MintermCalculator::calculateMintermRanges(uint numberOfRanges, uint upperBound)
{
	// Resulting minterm ranges (not overlapping)
	MintermRangeVector mrv;
	// If there are less values to calculate then threads available, use only one range
	if (upperBound < numberOfRanges)
	{
		// One range from 0 to upper bound
		mrv.emplace_back(MintermRange(0, upperBound));
	}
	else
	{
		// More values to calculate than available threads
		// Used to calculate the next begin of range
		const uint rangeValue = { upperBound / numberOfRanges };
		// We of course always start with 0
		uint startValue{ 0 };

		for (uint ui = 0; ui < numberOfRanges; ++ui)
		{
			// Calculate next end of range value
			uint endValue{ startValue + rangeValue };
			// If this is already over  the edge
			if (endValue > (upperBound))
			{
				// Limit the upper bound
				endValue = upperBound;
			}
			// Store resulting range
			mrv.emplace_back(MintermRange(startValue, endValue));
			// And set next start value
			startValue = endValue;
		}
	}
	// Return a vector with ranges
	return mrv;
}


// Calculate all minterms for a boolean functions
// Means set all possible input values for a boolean function
// If the result of the calculation is true, then we found a minterm and store it
// Calculations with a false result(Maxterm) are not stored.
// Since claculation is slow and gets slower with more variable
// We will use multitasking wit big boolean functions
MintermVector MintermCalculator::calculate()
{
	// Here we will store the result, so all minterms for the boolean function
	MintermVector mintermVector;
	
	// Compile the source code to object code
	if (!compilerResultOK)
	{
		compilerResultOK = compiler();
	}
	if (compilerResultOK)
	{
		// CHeck how many variables are in and how many calculation loops we need to do
		numberOfBooleanVariables = objectCode.symbolTable.numberOfSymbols();
		const uint maxEvaluations{ 1U << numberOfBooleanVariables };

		// If we have more than 10 boolean variables, then we will use multí threading to calculate the minterms
		if (numberOfBooleanVariables > 10)
		{
			// Claculate the ranges to be calculated by each thread
			MintermRangeVector mrv{ calculateMintermRanges(numberOfThreads, maxEvaluations) };

			// Futures for asynchronous functions. Wait for the thread to finish ang get the result
			std::future<MintermVector> futures[numberOfThreads];

			// Start calculation threads
			for (uint i = 0; i < numberOfThreads; ++i)
			{
				futures[i] = std::async(std::launch::async,& MintermCalculator::calculateAsThread, this, mrv[i], objectCode);
			}
			// Wait for the thread to end and read the result
			for (uint i = 0; i < numberOfThreads; ++i)
			{
				MintermVector mvTemp{ futures[i].get() };
				// Store all results from all threads in one common function result
				mintermVector.insert(mintermVector.end(), std::make_move_iterator(mvTemp.begin()), std::make_move_iterator(mvTemp.end()));
			}
		}
		else
		{
			// No multitasking
			// Load the code into a virtual machine and
			vmfbe.load(objectCode, source);
			// execute the program for all posiible input values
			for (uint ui = 0; ui < maxEvaluations; ++ui)
			{
				// If the result evaluates to true
				if (vmfbe.run(ui))
				{
					// then store this minterm
					mintermVector.push_back(ui);
				}
			}
		}
	}
	return mintermVector;
}


// Calculate minterms in a thread
MintermVector MintermCalculator::calculateAsThread(MintermRange mt, ObjectCode objectCodeParameter)
{
	// Resulting vector with minterms
	MintermVector mv;

	// Local virtual machine
	VirtualMachineForBooleanExpressions vm;
	// Load the object code into the virtual machine
	vm.load(objectCodeParameter, source);
	// And run the virtual machine for the range assigned to the thread
	for (uint ui = mt.first; ui < mt.second; ++ui)
	{
		// In case of true result
		if (vm.run(ui))
		{
			// Store the minterm
			mv.push_back(ui);
		}
	}
	// Return the local minterm vector, so that it can be combined with other thread results
	return mv;
}


// Print a comlete truth table. 
// So not only the minterm, but the result for a boolean expression for all possible inputs
// Source is a minterm vector. The variable source is the source string, the boolean expression.It is for output purposes only
// the symbol table is important, to give bits names

void printTruthTable(const std::string& source, MintermVector& mintermVector, const SymbolTable& symbolTable)
{
	// So many letter/symbols/variables/conditions/terminals were in the source string, the boolean expression
	const uint symbolCount{ symbolTable.numberOfSymbols() };
	// So many line we will show
	const uint32 maxEvaluations{ bitMask[symbolCount] };
	// The width of a string containg the decimal equivalent of the biggest number
	// Needed for formatting
	const MinTermType maxLengthDecimalEquivalent{ static_cast<uint>(std::log10(maxEvaluations)) + 1 };

	// Determine where the output is streamed to
	const bool predicateForOutputToFile{ (symbolCount > 4) };
	OutStreamSelection outStreamSelection(ProgramOption::pttc, predicateForOutputToFile) ;
	std::ostream& os{ outStreamSelection() };

	// Header Line
	os << "------------------ Truth table for boolean expression:\n\n'"<<source<<"'\n\n";

	for (uint i = 0; i < (1 + maxLengthDecimalEquivalent + 1); ++i) { os << ' '; }
	for (const cchar c : symbolTable.symbol) { os << c; }
	os << '\n';
	for (uint i = 0; i < (1 + maxLengthDecimalEquivalent + 1 + symbolCount + 2); ++i) { os << '-'; }
	os << "+--\n";

	// Now we ant to show the real data
	std::bitset<26> b;	// bitset for converting to binary string
	for (uint i = 0; i < maxEvaluations; ++i)
	{
		b = i; // Assign running value to bitset
		// Check if the running value is in the minterm list
		// If so, then result is 1 , else 0
		uint result{ (std::find(mintermVector.begin(), mintermVector.end(), i) != mintermVector.end()) ? 1U : 0U };
		// Print decimal equivalent of running variable, the bitset and the result
		os << std::setw(static_cast<std::streamsize>(maxLengthDecimalEquivalent) + 1) << i << " " << b.to_string().substr(26 - symbolCount) << "    " << result << '\n';
	}
	os << "\n\n";

}