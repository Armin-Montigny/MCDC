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
#ifndef QUINEMCCLUSKEY_HPP
#define QUINEMCCLUSKEY_HPP

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



#include "types.hpp"
#include "symboltable.hpp"
#include "coverage.hpp"



// Prime Implicant
// The Prime Implicant consists of a Minterm Number, the decimal equivalent of a Minterm
// A Minterm is a Productterm consisting of all variables. For example for "a+b"  (a || b)
// the Minterms are 1,2,3. 
// The Prime Implicant also consist of a mask, showing which of the variables have been deleted
// During the process of prime implicant extraction.
// FOr each deleted variable, on bit of the the Mask will be set.
// If there are max 4 vaiables in the boolean expression, and the Prime Implicant is ac, then
// obviously b and d have been eliminated, leading to as mask of 1010b (= 10) 
// Since the maximum number of variables (or the list of all variables) in the boolean expression
// is a global property and stored in the symbol table, we will not repeat this information
// in the Prime implicant entry. 
// If you want to show / print the prime implicant you nust use the information from the
// symbol table. The mask will only contain set bits for existing variables. 
struct PrimeImplicantType
{
	MinTermType term;	// Minterm
	MinTermType mask;	// Position od deleted variables

	// Creation of Prime Implicants
	PrimeImplicantType() noexcept : term(null<MinTermType>()), mask(null<MinTermType>()) {}
	PrimeImplicantType(MinTermType t, MinTermType m) noexcept : term(t), mask(m) {}

	// Conevrt Prime Implicant to a string. Use symbol table as described above
	std::string toString(const SymbolTable& symbolTable) const;
};

// The prime imlicants will be stored in a std::set. This needs a compare function. Here we define the Functor for it
struct PrimeImplicantTypeCompare
{
	bool operator() (const PrimeImplicantType& lhs, const PrimeImplicantType& rhs) const noexcept
	{
		return ((lhs.term < rhs.term) || ((lhs.term == rhs.term) && (lhs.mask < rhs.mask)));
	}
};

// Type Definition. Set of Prime Implicants. In such a set all found prime implicants can be stored
using PrimeImplicantSet = std::set<PrimeImplicantType, PrimeImplicantTypeCompare>;






// Implementation of the Quine and McCluskey method
class QuineMcluskey
{
public:
	QuineMcluskey();
	
	std::string getMinimumDisjunctiveNormalForm(MintermVector& mv, const SymbolTable& symbolTable, const std::string& source);

protected:

	// One row in a reduction table (for one bit count and one reuction loop)
	struct TableEntry
	{
		MinTermType mintermLower;	// The minterm and a the same time the minterm range for 2 combined minterms
		MinTermType mintermUpper;	// Upper part for 2 combined minterms
		MinTermType maskForEliminatedBit;	// What bits (in minterm lower) have beem eliminated by implication method
		mutable bool matchFound;	// QM algorithm tries to apply implication law. If 2 fittings terms are found, they will be marked

		// As described above. We are using an optimzed method for showing the source of absobed minterms
		// We can show all minterms (based on program options, and which is not needed for functionality)
		MintermSet matchedMinterm; // List of all source minterms. Functionality wise, only the upper and the lower are needed

		// Selection of constructors
		TableEntry(MinTermType mtl, MinTermType mtu, MinTermType mfe) : mintermLower(mtl), mintermUpper(mtu), maskForEliminatedBit(mfe), matchFound(false), matchedMinterm() {}
		TableEntry(MinTermType mtl, MinTermType mtu, MinTermType mfe, MintermSet& ms) : mintermLower(mtl), mintermUpper(mtu), maskForEliminatedBit(mfe), matchFound(false), matchedMinterm(ms) {}
		TableEntry(MinTermType mtl, MinTermType mtu, MinTermType mfe, MintermSet &&ms) : mintermLower(mtl), mintermUpper(mtu), maskForEliminatedBit(mfe), matchFound(false), matchedMinterm(ms) {}
		TableEntry() noexcept : mintermLower(0U), mintermUpper(0U), maskForEliminatedBit(0U), matchFound(false), matchedMinterm() {}

		// for std::unique function
		bool operator == (const TableEntry& other) const noexcept { return (maskForEliminatedBit == other.maskForEliminatedBit) && (mintermLower == other.mintermLower) && (mintermUpper == other.mintermUpper); }
	};

	// Functor for sorting TableEntry in a vector
	struct TableEntryCompare
	{
		bool operator() (const TableEntry& lhs, const TableEntry& rhs) noexcept
		{
			return (lhs.mintermLower < rhs.mintermLower) || ((lhs.mintermLower == rhs.mintermLower) && (lhs.mintermUpper < rhs.mintermUpper));
		}
	};

	// We group rows of TableEntrys by bit counts
	using TableForBitCount = std::vector<TableEntry>;


	// Tried "set" and "unordered_set", but "vector" with "sort" and "unique" is fastest solution
	// And there are many (up to number of variables) tables with TableEntry per bit count
	using BitsAndMinTerms = std::vector<TableForBitCount>;
	// And for each reduction stage there is a table of the above tables
	using ReductionTable = std::vector<BitsAndMinTerms>;

	// The classical Quine&  McCluskey Method notes down all minterms, from which a resulting 
	// minterm (by using the implication law) was generated. This is not necessary. The optimzied
	// Version uses the only lower and upper source minterm numbers
	// For documentation purposes the flag can be used to show everything
	bool processLowerAndUpperMintermOnly{ true };

	// Multi reduction stages reduction tables
	ReductionTable reductionTable;
	// Resulting prime implicants
	PrimeImplicantSet primeImplicantSetResult;


	// Take a list with all minterms and fill initial table. Grouped by number of bits for each minterm entry
	void initiate(MintermVector& mv);
	// Find the prime implicants. Reduce boolean expression
	void reduce();

	// Compare all elements of 2 tables with several TableEntrys (rows). One table with bitcount u and the other table with bitcount l (=u-1)
	void compareTwoEntries(TableForBitCount& upper, TableForBitCount& lower, TableForBitCount& output);

	// Compare a range of tables with each 2 tables with all elements of 2 tables with several TableEntrys (rows). One table with bitcount u and the other table with bitcount l (=u-1)
	void compareFromBitCountUpperToBitCountLower(uint indexReductionTableColumn, uint IndexUpper, uint IndexLower);

	// Check if we found all primeimplcants or if there are tables where we still need to do comparisons
	bool checkIfFurtherEvaluationNecessary(uint indexReductionTableColumn);

	// If tables shall be compare, we cann ignore everything which has no entries
	// These functions find the tables with entries for the lowest and highest number of bit counts
	uint getHighestIndexOfBitCountEntry(uint indexReductionTableColumn);
	uint getLowestIndexOfBitCountEntry(uint indexReductionTableColumn);

	// After a reduction round (Apllying the implication law
	void collectPrimeImplicants(uint indexReductionTableColumn);

	void initializeCoverageForPrimeImplicants(Coverage& coverageForPrimeImplicants, MintermVector& mv, const SymbolTable& symbolTable);



	// Since programm runtime grows geometrically with the number of variables, we will use
	// multi threading to reduce the calculation time
	// Quine&  McCluskey reduction works on presorted tables. Presorted by number of bits in the minterms.
	// SInce we want to apply the absorption law, we compare Minterm entries which just on different
	// Setting of a varibale. On positive and one negated.
	// We can use multi threading to calculate different blocks of Mintemrs with specific number of set bits
	// So a bit count range is for example 0 through 4
	using BitCountRange = std::pair<uint, uint>;

	// A bit count range vector stores all bit counts for all minterms for the whole expression
	// No overlap. NUmber of bits for the minterms of a boolean expression follow the standard binomial distribution
	// We try to follow this approach. For 14 varaibles we will get the following BitCountRanges for 
	// a system iwth 8 threads:  
	// 1,3   3,5   5,6   6,7   7,8   8,9   9,11   11,14 
	// This will help to speed up operations
	using BitCountRangeVector = std::vector<BitCountRange>;
	BitCountRangeVector calculateBoundsForMultiThreading(uint upper, uint lower);

	// Print the calculated reduction tables. SOurce is only for output purposes. Source is not needed for any calculation
	void printReductionTable(const SymbolTable& symbolTable, const std::string& source);
};





#endif