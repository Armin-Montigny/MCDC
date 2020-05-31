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
#ifndef MCDC_HPP
#define MCDC_HPP

// The name of the program is MCDC
//
// That gives a hint on what we would like to achieve. The input is a boolean expression
// given as string. The program performs several calculations and then tries to
// find test sets, that fullfill the criterion for MCDC coveragre
//
// MCDC Coverage is a structural code coverage property. Many othercode coverage
// properties are know. MCDC is interesting, because it can find many problems
// while using only a limited set of test cases.
//
// Therefore MCDC coverage is used in safety critical applications and is recomended
// by ISO 26262 as one criterium to ensure robust code.
//
// There is may literatur available that explains Code Coverage or structural Coverage
// in detail. Please read. This work is based on an FAA document:
//
// DOT/FAA/AR-01/18
// An Investigation of Three Forms of the Modified Condition Decision Coverage(MCDC) Criterion
// U.S. Department of Transportation Federal Aviation Administration
//
// This software first compiles the given source code into an Abstract Syntax Tree (AST) 
// with attributes and the possibility to calculate values in the tree
// It then does a brute force check of all possible pairs of test vectors. Example for 2 variables,
// so for possible values:
// (0,1), (0,2), (0,3), (1,2), (1,3), (2,3)
// As you can see, the number of possible test pairs will grow dramatically. There are
// If n is the number of variables then there are (2**n)(2**n-1)/2 possible test pairs
//
// All this possible test pairs will be avaluated. That means the AST will be evaluated for
// value 1. Trhe result will be stored. Then the AST will be evaluated with the 2nd value.
// Also this result will be stored. With a value for each node and each leave.
//
// MCDC requires that a unique condition (a variable) has an influence on the decision (the result).
// So flipping one variable, will cause a flip in the result.
// This will be checked with a XOR operation of the 2 stored trees. 
// XOR is 1, if the input values are different. In the resulting AST (XOR the first with the second)
// We check, if one variable has influence, by checking that all resulting values from thje leaf to 
// the root of the tree are 1. Meaning, they changed between the first AST and the second.
// There must be only one path with all 1's from the leaf to the root. And only one condition may 
// have changed. Otherwise it is not a unique cause. This is the original definition of MCDC and 
// unfortunately it fails with the simplest boolean expressions, becuase often there are strong 
// or weak coupled conditions. Menaing, if one conditions changes state the other must do this as well. 
// Example: ab+ac has no unique cuase MCDC for condition a.
//
// Smart people investigated and found, that more conditions may change, if they are masked by 
// boolean operations. Masked is always msileading or hard to understand. It basically means
// that any value of a operation has no influence because the other value already deterined the result.
// For an AND operation, if the first input is false, then the rest does not matter. Result will always
// be false, regardless of any value of the second input.
//
// This makes one thing very clear: MCDC must be checked as WHITE box analysis.
// All the black box anaylisys is working by accident and will find unique cause MCDC only.
// With masking, we allow such don't care conditions to change as well. And the special
// case that the influencing and a don't care variable are the same, we call
// Unique case + Masking   MCDC.
// Rest is Masking MCDC
//
// It should be clear that there are many more valid test pairs using all definitions than
// for unique cause only.
//
// Another factor is that in laguages like c or c++ there is "Boolean Short Cut Evalaution"
// That means, evaluation of an expression stops immediately, when the result is known.
//
// In early times smart develops used this even instead of IF ELSE statements and using
// ugly side effects in boolean expressions. In "C" cou can write
// (z != 0) && (y = x/z)
// Such things can only be detected and analysed with whitebox analysis.
// 
// Addditionaly a bollena expression can be in any form or in a minimum DNF (irredundant)
// The result will also be different.
//
// The software at hand can analyse all this cases:
// -All 3 forms of MCDC
// -Boolean shortcut evaluation or not
// -Using given boolean expression or use the minimum irredundant form.
//
//
// The miminum irredundant form will be calculated by the software using the
// Quine and McCluskey tabular method and Petricks method to reduce prime implicant tables.
// 
// After psooible test pairs have been identified the unate coverage or set cover
// algorithm will be used to find the minum number of test sets
//
 
#include "types.hpp"
#include "coverage.hpp"
#include "mintermcalculater.hpp"

#include <utility>
#include <array>

// Type of MCDC for a test pair
// Sequence of this values must not be changed!
enum class McdcType : uint
{
	UniqueCause,
	UniqueCauseMasking,
	Masking,
	NONE
};
// Will be used to print the type of the string. And yes, I know that I am using an array
inline const char* mcdcTypeToString(McdcType mt) 
{
	static const std::vector<const cchar*> McdcTypeTypeString { "Unique Cause", "Unique Cause + Masking", "Masking", "NONE" };
	return McdcTypeTypeString.at(static_cast<uint>(mt));
}



// A pair of test values (decimal equivalent of a boolean value)
using IndependencePair = std::pair<uint, uint>;


// The MCDC Indepence Pair contains a pair of test value, the type of the MCDC and the condition for this test pair. Simple POD
struct McdcIndependencePair
{
	explicit McdcIndependencePair(McdcType mt, uint v1, uint v2, cchar c) noexcept : mcdcType(mt), influencingCondition(c), independencePair(v1, v2) {}
	McdcIndependencePair() noexcept : mcdcType(McdcType::NONE), influencingCondition(), independencePair() {}

	McdcIndependencePair(const McdcIndependencePair& other)  noexcept { mcdcType = other.mcdcType; influencingCondition = other.influencingCondition; independencePair = other.independencePair; };
	McdcIndependencePair(const McdcIndependencePair&& other) noexcept { mcdcType = other.mcdcType; influencingCondition = other.influencingCondition; independencePair = other.independencePair; };

	McdcIndependencePair& operator =(const McdcIndependencePair& other) noexcept { mcdcType = other.mcdcType; influencingCondition = other.influencingCondition; independencePair = other.independencePair; return *this; }
	McdcIndependencePair& operator =(const McdcIndependencePair&& other) noexcept{ mcdcType = other.mcdcType; influencingCondition = other.influencingCondition; independencePair = other.independencePair; return *this; };

	~McdcIndependencePair() {}

	McdcType mcdcType;
	cchar influencingCondition;
	IndependencePair independencePair;
};




class Mcdc
{
public:
	// Brute force search for MCDC test pairs
	void findMcdcIndependencePairs(VirtualMachineForAST& ast);

	// Select the test set as a result of the brute force search
	void generateTestSets();

protected:

	// SHort and understandable names for internal data structures
	struct McdcIndependencePairComparator	// Sort functor. Needed for the set of McdcIndependencePairs
	{
		bool operator()(const McdcIndependencePair& left, const McdcIndependencePair&right) const noexcept
		{
			return (left.independencePair.first < right.independencePair.first) || ((left.independencePair.first == right.independencePair.first) && (left.independencePair.second < right.independencePair.second));
		}
	};	
	// Set of McdcIndependencePairs
	using McdcIndependencePairSet = std::set<McdcIndependencePair, McdcIndependencePairComparator>;

	// Container for easier access of the same data
	using TestVector = std::vector<McdcIndependencePair>;
	using TestSet = std::set<uint>;
	using IndependencePairPerCondition = std::map<cchar, McdcIndependencePairSet>;
	using TestPerMcdcType = std::map<McdcType, TestSet>;
	using TestPerInfluencingVariable = std::map<cchar, TestSet>;
	using ConditionSet = std::set<cchar>;


	// Main worker horse
	// Check if a Test pair is MCDC
	std::pair<McdcType, cchar> getMcdcType(AST& astInfluencingTree);


	// Print evaluated and calculated "best" test sets
	void printResult(const std::set<TestSet>& allTestSets, std::ostream& os);
	void printNotCoveredSymbols(std::ostream& os);

	// After a new MCDC test pair has been found, we will add it to our interanl list for further processing
	void add(McdcIndependencePair mcdcIndependencePair);


	// Initialise coverage table
	// If we have more than one independance test pair for one condition, we need to select a good one
	// If there are even redundant pairs, we can reduce the list and find the minimum coverage for one condition
	void initializeMcdcCoverageMethod(void);

	// Helps to select "good" test pairs
	//std::pair<bool, McdcIndependencePair> isCompleteTestPairInCoverageResult(const McdcIndependencePairSet& mips, const CellVectorHeaderSet& cvhs);
	//std::pair<bool, McdcIndependencePair> isOnePartOfTestPairInCoverageResult(const McdcIndependencePairSet& mips, const CellVectorHeaderSet& cvhs);
	bool isCompleteTestPairInCoverageResult(const McdcIndependencePair& mip, const CellVectorHeaderSet& cvhs);
	bool isOnePartOfTestPairInCoverageResult(const McdcIndependencePair& mip, const CellVectorHeaderSet& cvhs);


	// Best cost and heuristic function for selecting minimum test pairs
	sint compareScoreOf2TestValues(const TableCellVector& left, const TableCellVector& right);
	sint calculateScoreForOneTest(uint test) noexcept;	// Favours unique cause MCDC over others


	// If there is only one independence test pair per condition, we do not need to find a best solution for a coverage of variables
	bool isMax1IndependencePairPerCondition();

	// Best cost selector function
	McdcIndependencePair findBestResultingIndependencePair(TestVector& resultingIndependencePairPerVariableXPair);


	// All MCDC Indpenden Test Pairs
	TestVector testVector{};

	// Different presentation of same data. Defined to simplify the access and further processing of same data
	IndependencePairPerCondition independencePairPerCondition{};	// Grouped by condition
	TestPerMcdcType testPerMcdcType{};							// Grouped by MCDC type
	TestPerInfluencingVariable testPerInfluencingVariable{};		// Grouped by condition/variable

	// Conditions/variable used in boolean expression
	ConditionSet conditionSet{};
	// Vector of integers (Decimal equivalent of test values for variables)
	TestSet testSet{};


	// Used for calculation of minimum possible test set
	Coverage coverage{};

	// For output display purposes
	VirtualMachineForAST astUsedForMcdcCalculation{};
};


#endif // !MCDC_HPP
