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



// Implementation of Quine&  McCluskey Method for the minimization of boolean expressions

// Quine and&  McCluskey Tabular Deterministic Method to extract Prime Implicants from
// a list of Minterms.
//
// Input to this method is a list of Minterms (A terms consisting of all variables of 
// a boolean expression that will be evealuated to True). Minterms are given with their
// decimal equivalent.
//
//
// A special implementation is implemented to save memory space and time
//
// 1. We will use a separate tag to denote, which variable has been eliminated
// by applying the implication law. Deleted bits will be stored and can be
// used for futher evaluation. For the psoition of a deleted variable a bit will
// be set in the "Deleted" field
//
// 2. There is no need to store all Minterm Numbers for 2 combined Minterms
// as a source and input for next stages. This will save a tremendous amount of space.
// Example. For the following reduction table
//
// #     B#    a b c d    M D      MT Set
// 1     0     0 - -0	  _ 6      0 2 4 6
// 2     0     - 0 - 0    _ 10     0 2 8 10
// 3     0	   - -0 0     _ 12     0 4 8 12
// 4     1     0 - 1 -    _ 5      2 3 6 7
// 5     1     - 0 1 -    _ 9      2 3 10 11
// 6     1     0 1 - -    _ 3      4 5 6 7
// 7     1     - 1 0 -    _ 9      4 5 12 13
// 8     1     1 0 - -    _ 3      8 9 10 11
// 9     1     1 - 0 -    _ 5      8 9 12 13
//
// You will see, the deleted variable and in column D the respective encoding
// And for the minterm Set, you see the 4 entries each, from which the variable list
// has been geenrated. But!  Only the upper and lower value are interesting.
// The middle values can be eliminated. And so we can save a lot of memory and time.
// So the following table is equivalent
//
// #     B#    a b c d    M D      MT Set
// 1     0     0 - -0	  _ 6      0 6
// 2     0     - 0 - 0    _ 10     0 10
// 3     0	   - -0 0     _ 12     0 12
// 4     1     0 - 1 -    _ 5      2 7
// 5     1     - 0 1 -    _ 9      2 11
// 6     1     0 1 - -    _ 3      4 7
// 7     1     - 1 0 -    _ 9      4 13
// 8     1     1 0 - -    _ 3      8 11
// 9     1     1 - 0 -    _ 5      8 13
//
// For illustrational pruposes the values can be shown
//
// As a result of the tabular reduction mode, we will generate a list of Prime Implicants
// Prime Implicnat a minimized Product Terms that cove a minterm.
// They may be one then one prime implcants covering minterms. 
// Further reduction techniques try to find out eithre essential prime implicants
// (Prime implancts that are always needed to make the boolean expression true), or
// try to find a minimum covergae set of prime implicants that make the boolean 
// expression true.





#include "coverage.hpp"

#include "quinemccluskey.hpp"
#include "cloption.hpp"

#include <cmath>
#include <thread>
#include <future>

#include <iostream>
#include <iomanip>
#include <sstream>


// In order to find a minimum set of Prime Implicants we need to solve the set cover or unate covarge problem
// Bevor we can start doing the slection of prime implicant, we must initialze the coverage table
void  QuineMcluskey::initializeCoverageForPrimeImplicants(Coverage& coverageForPrimeImplicants, MintermVector& mv, const SymbolTable& symbolTable)
{
	// Add all minterms as rows
	for (const MinTermType minTerm : mv)
	{
		coverageForPrimeImplicants.addRow(minTerm);
	}
	// Add all prime implicants as columms
	for (PrimeImplicantType primeImplicant : primeImplicantSetResult)
	{
		// Additionally store the prime implicant name in the column header
		std::string primeImplicantString{ primeImplicant.toString(symbolTable) };
		coverageForPrimeImplicants.addColumn(primeImplicant.term, primeImplicantString, primeImplicant);
	}
}

// This is the next part of the initialization for the coverage table for prime implicants
// The covering cells are determined and set
// minTermRowHeader + primeImplicantColumnHeader
// This is a call back function
bool checkCoverForCell(const CellVectorHeader& minTermRowHeader, CellVectorHeader& primeImplicantColumnHeader)
{
	// The Prime Implicant Info has been stored in the any field of the column header´. Get it
	const PrimeImplicantType resultingPrimeImplicant{ std::any_cast<PrimeImplicantType>(primeImplicantColumnHeader.userData) };
	// And, get the minterm
	const MinTermType mtnMinTerm{ minTermRowHeader.index };

	// We want to know, if the minterm is covered by the prime implicant
	bool result{ false };

	// Check, if the prime implicant implies the minterm
	const MinTermNumber mtnPrimeImplicant{ resultingPrimeImplicant.term };
	const MinTermNumber primeImplicantMask{ resultingPrimeImplicant.mask };
	const MinTermNumber primeImplicantMaskNegated{ ~resultingPrimeImplicant.mask };
	const MinTermNumber mtnMinTermMasked{ mtnMinTerm & primeImplicantMaskNegated };
	const MinTermNumber mtnPrimeImplicantMasked{ mtnPrimeImplicant & primeImplicantMaskNegated };

	if (mtnMinTermMasked == mtnPrimeImplicantMasked)
	{
		// If so, then mark it in the prime implicant table
		result = true;
	}
	return result;
}


// This is main result that we want to have from the Quine&  McCluskey method
// Get a minimum possible disjunctive normal form DNF
// Please not, the source parameter is for output purposes
// The main input data is the minterm table
std::string QuineMcluskey::getMinimumDisjunctiveNormalForm(MintermVector& mv, const SymbolTable& symbolTable, const std::string& source)
{
	// Initialize the first level Quine and McCluskey table.
	// Will contain all minterms sorted by number of bits set in the minterm
	initiate(mv);

	// Apply Quine and McCluskey Method
	reduce();

	// Print all resulting reduction tables
	printReductionTable(symbolTable, source);

	// The result of the operation
	std::string minimumDisjunctiveNormalForm;

	// Output control for showing the result of the minimization of the prime implicant table
	const bool predicateForOutputToFile{ (symbolTable.numberOfSymbols() > 5) };
	OutStreamSelection outStreamSelection(ProgramOption::ppirtc, predicateForOutputToFile);
	std::ostream& os{ outStreamSelection() };

	os << "\n\n\n------------------ Try to find Minmum Disjunctive Normal form for boolean expression:\n\n'" << source << "'\n\n";

	// Set up the prime implicant table
	Coverage coverageForPrimeImplicants;
	// Fill in row and column headers for prime implicant table
	initializeCoverageForPrimeImplicants(coverageForPrimeImplicants, mv, symbolTable);
	// Set the callback function that will be called to set the initial coverage
	coverageForPrimeImplicants.setCheckForCoverFunction(checkCoverForCell);
	// And now find the initial coverage (Call the callback function)
	coverageForPrimeImplicants.checkAllCellsForCover();


	// Reduce the primeimplicant table. May have several resulting vectors
	CoverageResult cr{ coverageForPrimeImplicants.reduce(os) };

	// From possible many results, try to find one result	
	// Some heuristic methods . . .
	// 1. Count number of literals in coverage result
	// 2. count number of lowercase literals in coverage result
	// 3. Count NUmber of terms in coverage result

	// To do this, we first need the maximum number of literals in a coverage set over all coverage sets
	// And we need to have the maximum number of terms in the coverage sets

	{
		// Count terms
		uint maxTerms{ narrow_cast<uint>(std::max_element(cr.begin(), cr.end(), [](const CellVectorHeaderSet & cvhsLeft, const CellVectorHeaderSet & cvhsRight) noexcept {return cvhsLeft.size() < cvhsRight.size(); })->size()) };
		std::vector<uint> numberOfLiterals{};				// Count number of literals per possible result
		std::vector<uint> numberOfLowerCaseLiterals{};	// Count number of lowercase literals per possible result

		// Iterate over all possible results in the coverage set
		for (CellVectorHeaderSet& cvhs : cr)
		{
			std::vector<cchar> literals;// Here we count the literals for this possible coverage
			std::vector<cchar> lowerCaseLiterals; // And the same for the lowercase stuff
			// Look at all the prime implicants in the set of prime implicants for this possible solution
			for (const CellVectorHeader& cvh : cvhs)
			{
				// And now look how many characters this prime implicant contains
				for (const cchar c : cvh.textInfo)
				{
					literals.push_back(c);	// Literals over all
					if (c > 'Z')	// Check for lowercase letter
					{
						lowerCaseLiterals.push_back(c);  // And count the pure number of lowercase
					}
				}
			}
			// And now store the results, the sizes, so the number of all literals that we found for this result
			numberOfLiterals.push_back(static_cast<uint>(literals.size()));
			numberOfLowerCaseLiterals.push_back(static_cast<uint>(lowerCaseLiterals.size()));
		}


		// This is the maximumn number of literals in a solution for all coverage sets
		uint maxNumberOfLiterals{ *std::max_element(numberOfLiterals.begin(), numberOfLiterals.end()) };

		// We use some heuristic function, magically defined, to calculate some judgement for a "good" result.
		// Higher number is better
		uint maxResult{ 0 };		// Best result achieved so far
		uint maxResultIndex{ 0 };	// And index in the coverage with the best judgement
		for (uint i = 0; i < cr.size(); ++i)
		{
			const uint result{ (maxNumberOfLiterals - numberOfLiterals[i]) * 5 +   // Few literals result in high score (this is high prio)
				(maxNumberOfLiterals - numberOfLowerCaseLiterals[i]) * 4 +			// Lowercase letters are better than uppercase (Uppercase are negated lowercase letters)
				(maxTerms - narrow_cast<uint>(cr[i].size())) * 8 };					// The number of minterms in the resulting DNF has top prio
			if (result > maxResult)	// Is this the best result so far?
			{
				maxResult = result;	// Remember best result
				maxResultIndex = i;
				// If no best solution then use first solution
			}
		}

		// This is for output only
		// Get the number of Product Terms in the best solution
		const uint NumberOfTermsInBestSolution{ narrow_cast<uint>(cr[maxResultIndex].size()) };
		uint TermCounter{ 0 };

		// Build minimum disjunctive normal form, Minimun DNF, as a string
		for (const CellVectorHeader& cvh : cr[maxResultIndex])
		{
			minimumDisjunctiveNormalForm += cvh.textInfo;
			TermCounter++;
			if (TermCounter < NumberOfTermsInBestSolution)
			{
				minimumDisjunctiveNormalForm += "+";
			}
		}

	}
	os << "\n\n------------------ Minimum DNF:\n\n" << minimumDisjunctiveNormalForm << "\n\n------------------\n\n";
	return minimumDisjunctiveNormalForm;
}









// Constructor is initializing the mode of operation depending on comand line option
QuineMcluskey::QuineMcluskey() 
{
	// Show full Quine& McCluskey Reduction Table(will be a bit slower)
	if (programOption.option[ProgramOption::sfqmt].optionSelected)
	{
		// Set internal flag. All originator minterms will be shown in the reduction tables
		// And not only the highes and lowest source minterm
		// This will be slower and consume more memory
		// But it will be more understandable for the user in the printout
		processLowerAndUpperMintermOnly = false;
	}
}


// Fill the first initial Quine&  McCluskey Reduction table
// Fill it with minterm grouped by number of bits in minterm
void QuineMcluskey::initiate(MintermVector& mv)
{
	// We will use a fxed sized vector to store the goups of minterms
	// Depending on the number of max variables
	BitsAndMinTerms bitsAndMinTerms(MAX_NUMBER_OF_BOOLEAN_VARIABLES+1);

	// This is one row of a reduction table
	TableEntry tableEntry;

	// Iterate over all input minterms
	for (const MinTermNumber& mtn : mv)
	{
		// For the selected algorithm, we will use only the lower and upper
		// source minterm number, out of which a new row will be generated
		// For the first initial table, upper and lower are the same 
		tableEntry.mintermLower = tableEntry.mintermUpper = mtn;

		// If the user wants to see every originator minterm
		// then we additionally store it an a dedicated container
		// This has is only used for printout
		if (!processLowerAndUpperMintermOnly)
		{
			tableEntry.matchedMinterm.clear();
			tableEntry.matchedMinterm.insert(mtn);
		}

		// Get the number of bits for this minterm
		const NumberOfBitsCountType nb{ numberOfSetBits(mtn) };
		
		// And add this table row to the grouped-by-number-of-bits-table
		bitsAndMinTerms[nb].push_back(tableEntry);
	}

	// After that, add it to the vector conatining all reduction tables that will be filled in several loops
	reductionTable.push_back(std::move(bitsAndMinTerms));
}



// One of the main functions that implement the essence of the Quine&  McCluskey algorithm
// Rows with terms containing all the same number of set bits will be compared
// with other rows conatining one set bit more.
// The goups of tables with a different numer of set bits are given as parameter
// This, becuase we want to use multithreading and let different threads do comparisons
// for different tables with different number of set bits
// This function compares 2 subtables and produces one resulting table
void QuineMcluskey::compareTwoEntries(TableForBitCount& upper, TableForBitCount& lower, TableForBitCount& output)
{
	
	// Compare alle table entries, all rows with a number of bits, with the other table entries, rows with number of bits+1
	// Compare all rows of table with bit count + 1
	for (const TableEntry& teUpper : upper)
	{
		// To all other rows bit bit count
		for (const TableEntry& telower : lower)
		{
			// The next 4 lines are implement the algorithm 
			// abc+aBc == ac(b+B) == ac(true) == ac
			// So, 2 terms, with the same variables and a difference in just one bit (b--B), can be reduced
			// to one term, with the different bit eliminated.

			// The mask (The so far eliminated Bit Positions) must be equal
			if (teUpper.maskForEliminatedBit == telower.maskForEliminatedBit)
			{
				// XOR for the 2 terms, will find the different set bits between 2 terms
				const MinTermType termDifference{ (teUpper.mintermLower ^ telower.mintermLower) };
				// If there is only a difference in one bit. So is there only one bit set in the difference we just calculated
				const bool differenceInOnlyOneBit{ (!(termDifference & (termDifference - 1))) };  // Taken from "Hackers Delight"
				// So, we found 2 terms, where one variable can be eliminated
				if (differenceInOnlyOneBit)
				{
					// According to the algorithm, we will mark the 2 terms as "done"
					// We found a difference in exactly one bit. Mark both terms
					teUpper.matchFound = true;
					telower.matchFound = true;

					// Depending on the comand line selection . . .
					// Functionality wise, only the upper and the lower term from the source terms are important
					// Standard QWuine and McCluskey copy all source terms to the next table.
					// This is not necessary
					// Anyway, for the user it may be simpler to see everything. We give the possibility
					// And implement the algorithm here accordingly
					if (processLowerAndUpperMintermOnly)
					{
						// Create a new tableEntry for the next reduction Table column
						output.push_back(TableEntry(telower.mintermLower, teUpper.mintermUpper, termDifference + telower.maskForEliminatedBit));
					}
					else
					{
						// Store all the source values, from which we combined the new one
						MintermSet mintermSetForMatches;
						mintermSetForMatches.insert(telower.matchedMinterm.begin(), telower.matchedMinterm.end());
						mintermSetForMatches.insert(teUpper.matchedMinterm.begin(), teUpper.matchedMinterm.end());
						
						// Create a new tableEntry for the next Reduction Table column
						output.push_back(TableEntry(telower.mintermLower, teUpper.mintermUpper, termDifference + telower.maskForEliminatedBit, std::move(mintermSetForMatches)));
					}
				}
			}
		}
	}
	// Definition of Functor. For comparing and finally sorting the rows in the resulting tables
	TableEntryCompare tableEntryCompare;
	// Sort the new resulting table
	std::sort(output.begin(), output.end(), tableEntryCompare);
	// And erase duplicates
	output.erase(std::unique(output.begin(), output.end()), output.end());
}



// Compare a range of sub table (grouped by number of set bits) for a complete reduction table
// Also here is the intention to enable mlti threading
void QuineMcluskey::compareFromBitCountUpperToBitCountLower(uint indexReductionTableColumn, uint indexUpper, uint indexLower)
{
	// For the given main reduction table
	// Do something with subtables with a number-of-set-bits-upper to number-of-set-bits-upper
	// And store the result in a new reduction table for the next reduction round
	// So, do for given range
	for (uint ui = indexUpper; ui > indexLower; --ui)
	{
		compareTwoEntries(	reductionTable[indexReductionTableColumn][ui],								// Subtable with set bit count x
							reductionTable[indexReductionTableColumn][static_cast<uint>(ui) - 1],		// Subtable with set bit count x-1

							// Write to other (next) reduction table at the lower bit count index
							// It is alwyas the lower bit cout index, because if you comapre something
							// with 4 bits sets and with 3 bits set, then the result is of youres something with 3 bits set
							reductionTable[static_cast<size_t>(indexReductionTableColumn) + 1U][static_cast<uint>(ui) - 1]);
	}
}


// Overall reduction algorithm until prime implicants are found
void QuineMcluskey::reduce()
{
	// We start with the main and initial reduction table 0
	uint currentReductionTableColumn{ 0 };
	// Iterate, until all prime implicants are found
	bool doIterateUntilAllPrimeImplcantsAreFound{ false };
	do
	{
		// Already now create a new, next stage, reduction table
		reductionTable.emplace_back(BitsAndMinTerms(MAX_NUMBER_OF_BOOLEAN_VARIABLES + 1));

		// We simple have defined the table with a fixed number. Without knowing 
		// how many bist are set at all. We will now find out the table index, where a bit is set
		// And here, the highest index and the lowest index
		const uint upper{ getHighestIndexOfBitCountEntry(currentReductionTableColumn) };
		const uint lower{ getLowestIndexOfBitCountEntry(currentReductionTableColumn) };
		// We want to calculate ranges of "bit-set-numbers" that we can give to multiple threads
		BitCountRangeVector bcrv{ calculateBoundsForMultiThreading(upper, lower) };
		// If we will do multi threading
		if (bcrv.size() > 0)
		{
			// Define futures for the threads. No return value necessary. We just wnt to wait until thread is finished
			std::future<void> futures[numberOfThreads];
			// Get the ranges
			for (uint ui = 0; ui < bcrv.size(); ++ui)
			{
				uint u{ bcrv[ui].first };
				uint l{ bcrv[ui].second };
				// And call calculation function in a thread
				futures[ui] = std::async(&QuineMcluskey::compareFromBitCountUpperToBitCountLower, this, currentReductionTableColumn, u, l);
			}
			// Wait until all threads are finished
			for (uint ui = 0; ui < bcrv.size(); ++ui)
			{
				futures[ui].wait();
			}
		}
		else
		{
			// No multi threading. Calculate result in one thread
			compareFromBitCountUpperToBitCountLower(currentReductionTableColumn, upper, lower);
		}

		// Collect prime implicants after this round. Prime implicants have not been marked (as used for combination)
		collectPrimeImplicants(currentReductionTableColumn);

		// Goto next reduction round
		++currentReductionTableColumn;
		// Check if all done or if further iteration necessary
		doIterateUntilAllPrimeImplcantsAreFound = checkIfFurtherEvaluationNecessary(currentReductionTableColumn);

	} while (doIterateUntilAllPrimeImplcantsAreFound);
}


// -----------------------------------------
// Helper funcions



// Collect the prime implicnats
// Afte a reduction round has been made, and we tried to combine rows withd different number of bits set
// Some rows my not have been combine and ar hance not marked as such
// Then, it is a prime implicant
void QuineMcluskey::collectPrimeImplicants(uint indexReductionTableColumn)
{
	// Iterate over the a reduction table for this stage
	for (const TableForBitCount& tfbc : reductionTable[indexReductionTableColumn])
	{
		// If there are elements then iterate over rows in the subtables (gouped by bit numbers)
		if (tfbc.size()) for (TableEntry te : tfbc)
		{
			// And if the row is not marked as combined with a other row
			if (!te.matchFound)
			{
				// The it is a prime implicnat and we save it in the set of prime implicants
				primeImplicantSetResult.insert(std::move(PrimeImplicantType(te.mintermLower, te.maskForEliminatedBit)));
			}
		}
	}
}

// Check, if we found all prime implicants or if we need to continue searching
bool QuineMcluskey::checkIfFurtherEvaluationNecessary(uint indexReductionTableColumn)
{
	bool result{ false };	// We initially assume that no further evealuation is necessary
	// Go through all sub table (gouped by number of bits) for this reduction table
	for (const TableForBitCount& tfbc : reductionTable[indexReductionTableColumn])
	{
		// If any subtable contains elements
		if (tfbc.size())
		{	
			// The we need to continue the search
			result = true; break;	// No further searching necessary
		}
	}
	return result;
}

// A reduction table always consits of sub tables that are grouped by number of bits
// Te reduction table always contains subtables for all possible number of set bits
// Mostly they are not all used
// To save time and memeory, we will not evaluate empty tables

// This function returns the highest index of the subtable that contains elements
uint QuineMcluskey::getHighestIndexOfBitCountEntry(uint indexReductionTableColumn)
{
	uint result{ 0 };
	// Iterate over all sub tables
	for (uint ui = 0; ui < reductionTable[indexReductionTableColumn].size(); ++ui)
	{
		// If subtable contains elements, then remember the index
		// If in the next run, we find a table which also contains
		// elements, we overwrite the previous value and store the new one.
		// With that, we will get the last table that contains date
		if (reductionTable[indexReductionTableColumn][ui].size() > 0)
		{
			result = ui;
		}
	}
	return result;
}

// This function returns the lowest index of the subtable that contains elements
uint QuineMcluskey::getLowestIndexOfBitCountEntry(uint indexReductionTableColumn)
{
	uint result{ 0 };
	// Iterate over sub tables, starting with 0, until we find a sub table that has elements

	for (uint ui = 0; ui < reductionTable[indexReductionTableColumn].size(); ++ui)
	{
		// If sub table has elements, then this is the first. And the result is the lowest index
		if (reductionTable[indexReductionTableColumn][ui].size() > 0)
		{
			result = ui;
			break;	// Important. Must stop search
		}
	}
	return result;
}


// This is a little bit hard to understand. Lets give it a try
// Sínce comparing rows bit certain number of set bits is a very time and memory consuming
// operation, we will uses multithrading to reduce calculation time
// We need to compare ranges with number of set bits. For example we need to
// compare a subtable that has 4 bits with a subtable that has 3 bits. And so one
// Mathimatically it is the case, that for a certain number of bits there
// are more possible minterms (rows) then for the other. We want to
// distribute the work to the threads in a somehow balanced way.
// Looking at the mathematics, we see the the binimial distrbution applies
// Number of possible minterms with a given number of set bits is (n over k)
// This function simulates that distribution and generates ranges for set
// bits that can then be worked on by separate threads

// Upper and lower ar max and min indices for sub tables
// The result is a vector of pairs with upper/lower values
QuineMcluskey::BitCountRangeVector QuineMcluskey::calculateBoundsForMultiThreading(uint upper, uint lower)
{
	BitCountRangeVector result;		// Here will store the resulting ranges

	// For the claculation we atrt in the middle of the given overall range
	const uint middle{ static_cast<uint>(std::ceil((upper - lower) / 2 + 1)) };

	// We start with a delta of one, so the next range will be only on more or less than the current value
	sint delta{ 1 };
	// We will start with the middle and then go up and down
	sint currentForDirectionLower{ static_cast<sint>(middle) };
	sint nextForDirectionLower{ static_cast<sint>(middle) - delta };

	sint currentForDirectionUpper{ static_cast<sint>(middle) + delta };
	sint nextForDirectionUpper{ static_cast<sint>(middle) };

	//Depending on the number of available threads we will define ranges for up and for down
	const uint maxLoopCount{ (numberOfThreads >> 1) };
	const uint lastLoopCount{ maxLoopCount - 1 };


	for (uint i = 0; i < maxLoopCount; ++i)
	{
		// Direction for lower indices
		// If we did not hit the lower boundary
		if (currentForDirectionLower > narrow_cast<sint>(lower))
		{
			// Limit to the lower boundary. Ther must be nothing smaller than "lower"
			if (nextForDirectionLower < narrow_cast<sint>(lower))
			{
				nextForDirectionLower = narrow_cast<sint>(lower);
			}
			// If this is the last possibility to set the range
			if (i == lastLoopCount)
			{
				// Then the range is from the crrent value to the lower boundary
				result.push_back(BitCountRange(currentForDirectionLower, lower));
			}
			else
			{
				// ELse the range is from the current value to the next calculated value
				result.push_back(BitCountRange(currentForDirectionLower, nextForDirectionLower));
			}
		}

		// Direction for higher indices
		// If we did not hit the upper boundary
		if (nextForDirectionUpper < narrow_cast<sint>(upper))
		{
			// Limit claculated value to upper boundary
			if (currentForDirectionUpper > narrow_cast<sint>(upper))
			{
				currentForDirectionUpper = narrow_cast<sint>(upper);
			}
			// If this is the last loop run,
			if (i == lastLoopCount)
			{
				// Then the range is from the current element to the upper boundary
				result.push_back(BitCountRange(upper, nextForDirectionUpper));
			}
			else
			{
				// ELse the range is from the current value to the next calculated value
				result.push_back(BitCountRange(currentForDirectionUpper, nextForDirectionUpper));
			}
		}

		// Clculate the next values
		currentForDirectionLower = nextForDirectionLower;
		nextForDirectionLower -= delta;

		nextForDirectionUpper = currentForDirectionUpper;
		currentForDirectionUpper += delta;
		// And in the next round the delta will be bigger.
		// Times 2
		// This reproduces the effect of the binmial distribution
		delta = delta << 1;
	}
	return result;
}



// -----------------------------------------
// print data


// Convert a prime implicant to a string
//The symbol table is needed to do the translation
std::string PrimeImplicantType::toString(const SymbolTable& symbolTable) const
{
	std::ostringstream result;
	// Maximum number of symbols in the symbol table
	const uint symbolCount{ symbolTable.numberOfSymbols() };
	// We will check, which bit is set or not. For the set bits we will print a symbol
	MinTermNumber maskSelector{ bitMask[symbolCount - 1] };

	// We will iterate over all symbols
	std::set<cchar>::iterator symbolIterator{ symbolTable.symbol.begin() };

	// And for the number of symbols
	for (uint ui = 0; ui < symbolCount; ++ui)
	{
		// Apply mask to symbol iterator (which has also a mask)
		// Is there a symbol at this position?
		if (!(maskSelector&  mask))
		{
			const MinTermNumber mt{ term };
			const MinTermNumber mtMasked{ mt & maskSelector }; // This will result in true, if there is a positive variable at this position 
			// Distihuish between lower case and upper case.
			result << (mtMasked ? *symbolIterator : static_cast<cchar>((*symbolIterator) - ('a' - 'A')));
		}
		// Check next bit position
		maskSelector = maskSelector >> 1;
		// And evaluate next symbol
		++symbolIterator;
	}
	return result.str();
}


// Print all Reduction tables, so show the complete history of the Q&M reduction process
void QuineMcluskey::printReductionTable(const SymbolTable& symbolTable, const std::string& source)
{
	// Tetermin, to which stream the output should go
	// Showing tables for more than 6 variables is too much data
	const uint maxNumberOfBits{ narrow_cast<uint>(symbolTable.symbol.size()) };
	const uint maxNumberOfBitsMinusOne{ maxNumberOfBits - 1 };
	const bool predicateForOutputToFile{ (maxNumberOfBits > 6) };
	OutStreamSelection outStreamSelection(ProgramOption::pqmtc, predicateForOutputToFile);
	std::ostream& os{ outStreamSelection() };

	// Header
	os << "------------------ Print Quine McCluskey Reduction tables for boolean expression\n\n'" << source << "'\n\n";

	// We will ptint table below each other. The classical approach by printing different
	// reduction steps side by side consumes too much space
	// So go through all but the last reduction tables (The last is alwyas empty)
	for (uint currentTableIndex = 0; currentTableIndex < reductionTable.size()-1; ++currentTableIndex)
	{
		os << "\nReduction Loop " << currentTableIndex + 1 << "-----------------------------\n\n";
		
		// Print Column headers
		os << "#     B#    ";
		for (const cchar c : symbolTable.symbol)
		{
			os << c << ' ';
		}
		os << "   M D      MT Set\n";

		// Counter for the lines in a sub table
		uint counter{ 1 };

		// We do not want to print empty tables
		const uint upper{ getHighestIndexOfBitCountEntry(currentTableIndex) };
		const uint lower{ getLowestIndexOfBitCountEntry(currentTableIndex) };

		// So, for all the sub tables grouped by the number of set bits
		for (uint bitCount = lower; bitCount <= upper; ++bitCount)
		{
			// For each row in this sub table
			for (TableEntry& te : reductionTable[currentTableIndex][bitCount])
			{
				// Print running number and bit count
				os << std::left << std::setw(6)<< counter++<< std::setw(4) << narrow_cast<uint>(bitCount) << "  ";

				// Get the minterm and the mask. So, see, what avriables are in there
				const MinTermNumber mtn{ te.mintermLower };
				const MinTermNumber deletedPos{ te.maskForEliminatedBit };

				// This will be used to iterate over the single literals in the minterms
				// Start with this vale und will be shifted right
				MinTermNumber bitMaskLocal{ bitMask[maxNumberOfBitsMinusOne] };
				
				// Now check all bits in the term
				for (uint bitIndex = 0; bitIndex < maxNumberOfBits; ++bitIndex)
				{
					// Defualt is 0
					cchar output{ '0' };
					// If the varaible has been deleted
					if (deletedPos&  bitMaskLocal)
					{
						output = '-';	// indicate deleted variable via dash
					}
					else
					{
						// If ist is posutively set
						if (mtn&  bitMaskLocal)
						{
							output = '1';	// Shwo a 1
						}
						// else, use 0 from the varaibale initiaization
					}
					bitMaskLocal = bitMaskLocal >> 1; // Next bitmask
					os << output << ' ';	// print separator
				}


				// Are we not in the first table?
				if (currentTableIndex)
				{
					// Then show, if a combination could be done and what bit has been deleted
					os << "   " << (te.matchFound ? 'X' : '_') << ' ' << std::setw(4) << deletedPos << "   ";
				}
				else
				{
					// In the first initial table there is no deleted pos. There is just the minterm 
					os << "   " << (te.matchFound ? 'X' : '_') <<  "        ";
				}


				// For the selected algorithm, we will use only the lower and upper
				// source minterm number, out of which a new row will be generated
				// For the first initial table, upper and lower are the same 
				// If the user wants to see every originator minterm
				// then we additionally store it an a dedicated container
				// And thsi we will show also to the user and print it
				if (processLowerAndUpperMintermOnly)
				{
					// Show only lower and upper value as the originator of the new combined value
					os << te.mintermLower << ' ' << te.mintermUpper << '\n';
				}
				else
				{
					// Show all minterms, from whicht his row has been created
					for (const MinTermType m : te.matchedMinterm)
					{
						os << m << ' ';
					}
					os << '\n';
				}
			}
		}
		os << "\n\n";
	}

	// Then show all prime implicants
	os << "------------------ Prime Implicants  after Quine McCluskey Minimization for boolean expression\n'" << source << "'\n\n";

	for (const PrimeImplicantType& resultingPrimeImplicant : primeImplicantSetResult)
	{
		os << resultingPrimeImplicant.toString(symbolTable) << "  ";
	}
	os << "\n\n";
}






