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


// The name of the program is MCDC
//
// That gives a hint on what we would like to achieve. The input is a boolean expression
// given as string. The program performs several calculations and then tries to
// find test sets, that fullfill the criterion for MCDC coverage
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






#include "mcdc.hpp"

#include "coverage.hpp"
#include "ast.hpp"

#include <iostream>
#include <iomanip>

// Add MCDC test pair to internal variable structures
// Partly redundant storage of values
// But for easier access we use different container for the same data
void Mcdc::add(McdcIndependencePair mcdcIndependencePair)
{
	testVector.push_back(mcdcIndependencePair);
	switch (mcdcIndependencePair.mcdcType)
	{
	case  McdcType::UniqueCause:

		testPerMcdcType[McdcType::UniqueCause].insert(mcdcIndependencePair.independencePair.first);
		testPerMcdcType[McdcType::UniqueCause].insert(mcdcIndependencePair.independencePair.second);
		break;
	case McdcType::UniqueCauseMasking:

		testPerMcdcType[McdcType::UniqueCauseMasking].insert(mcdcIndependencePair.independencePair.first);
		testPerMcdcType[McdcType::UniqueCauseMasking].insert(mcdcIndependencePair.independencePair.second);
		break;
	case McdcType::Masking:

		testPerMcdcType[McdcType::Masking].insert(mcdcIndependencePair.independencePair.first);
		testPerMcdcType[McdcType::Masking].insert(mcdcIndependencePair.independencePair.second);
		break;
	case McdcType::NONE:  //fallthrough
	default:
		break;
	}
	independencePairPerCondition[mcdcIndependencePair.influencingCondition].insert(mcdcIndependencePair);

	testPerInfluencingVariable[mcdcIndependencePair.influencingCondition].insert(mcdcIndependencePair.independencePair.first);
	testPerInfluencingVariable[mcdcIndependencePair.influencingCondition].insert(mcdcIndependencePair.independencePair.second);

	conditionSet.insert(mcdcIndependencePair.influencingCondition);
	testSet.insert(mcdcIndependencePair.independencePair.first);
	testSet.insert(mcdcIndependencePair.independencePair.second);
}

// This is a best cost score calculation function and favours
// "Unique cause" over "Unique Cause + Masking" over "Masking"
// As last judgement attribute the number of set bits is used. Fewer is better than more
// Calculate a score for one test value
// calling function needs an sint as returning value, becuase it does want to make comparisons
// by substracting 2 scores
sint Mcdc::calculateScoreForOneTest(uint test) noexcept
{
	// A high resulting score is better than a low score
	sint resultingScore{ 0 };
	
	// Iterate over all independence pairs
	for (const McdcIndependencePair& mcdcIndependencePairSet : testVector)
	{
		// If the test (given as a parameter to this function) is in a found test pair
		if ((mcdcIndependencePairSet.independencePair.first == test) || (mcdcIndependencePairSet.independencePair.second == test))
		{
			// Calculate score, depending on the MCDC type in which the test value fits
			switch (mcdcIndependencePairSet.mcdcType)
			{
			case McdcType::UniqueCause:
				resultingScore += 64;
				break;
			case McdcType::UniqueCauseMasking:
				resultingScore += 32;
				break;
			case McdcType::Masking:
				resultingScore += 16;
				break;
			case McdcType::NONE:
				break;
			}
			// An additional plus is a lower number of set bits in the test value
			resultingScore += (MaxNumberOfBitsForEvaluation-numberOfSetBits(test));
		}
	}
	return resultingScore;
}


// This best cost function can be handed to the coverage problem solver
// Hence the parameters are of type TableCellVector. So a row or a column
// See module coverage for more explanation
sint Mcdc::compareScoreOf2TestValues(const TableCellVector& left, const TableCellVector& right)
{
	const sint sumFirst{ calculateScoreForOneTest(left.cellVectorHeader.index) };
	const sint sumSecond{ calculateScoreForOneTest(right.cellVectorHeader.index) };
	return (sumFirst - sumSecond);
}


// There may be many test vectors or test pairs covering one condition
// but only one is needed. We need to find the correct test value/pair
// To solve this problem the set cover or unate covering method is used
// CLass Mcdc has a coverage member
// This function initializes the internal Mcdc coverage member
void  Mcdc::initializeMcdcCoverageMethod()
{
	// Check, where the output goes to and provide the corresponding stream
	const bool predicateForOutputToFile{ (narrow_cast<uint>(testSet.size()) > 100) };
	OutStreamSelection outStreamSelectionMcdc(ProgramOption::pmtpc, predicateForOutputToFile);
	std::ostream& os = outStreamSelectionMcdc();

	// Print MCDC test pairs per variable
	// Header
	os << "\n\nFound Testvectors\n\n";

	// Only if, there are solutions
	if (independencePairPerCondition.size() > 0)
	{
		uint linePrintCounter{ 1U };
		// Print something for every condition
		for (const std::pair<const cchar, McdcIndependencePairSet>& mips : independencePairPerCondition)
		{
			// Get the test pair for this condition
			for (const McdcIndependencePair& mcdcIndependencePairL  : mips.second)
			{
				// Show all relevant information
				os	<< "ID: " << std::setw(3) << linePrintCounter << "  Influencing Condition: '" << mips.first << "'  Pair: " << std::setw(2)
					<< mcdcIndependencePairL.independencePair.first << ", " << std::setw(2) << mcdcIndependencePairL.independencePair.second 
					<< "   " << mcdcTypeToString(mcdcIndependencePairL.mcdcType) << '\n';
				++linePrintCounter;
			}
		}
		printNotCoveredSymbols(os);
		os << "\n\n";
	}


	// Iterate over conditions
	uint rowIndex{ 0 };
	for (const cchar c : conditionSet)
	{
		// Add rows and rw header for condition
		coverage.addRow(rowIndex, std::string(1,c));
		++rowIndex;
	}
	// Iterate over tests
	for (const uint t : testSet)
	{
		// and add test as column and column header
		coverage.addColumn(t,std::to_string(t));
	}

	// Now we want to find out, what tests (one or more) cover which condition
	uint row = 0;
	// Iterate over all tests per variable. So here. Iterate over conditions
	for (const std::pair<const cchar, TestSet>& testSetL : testPerInfluencingVariable)
	{
		// Within that, we iterate over tests in testSet
		for (const uint uiTest : testSetL.second)
		{
			// Find the test in the testSet
			TestSet::iterator ti{ testSet.find(uiTest) };
			if (testSet.end() != ti)
			{
				// If found, mark this row, col combination as initial cover
				const uint col{ narrow_cast<uint>(std::distance(testSet.begin(),ti)) };
				coverage.setCellAsCover(row, col);
			}
		}
		// next condition
		++row;
	}
}


// The software calculatest test vectors for influencing conditions
// For example for "!((a+b)(c+d))" the following pairs will be found

//	ID:  1  Influencing Condition : 'a'  Pair : 1, 9   Unique Cause
//	ID : 2  Influencing Condition : 'a'  Pair : 1, 10   Masking
//	ID : 3  Influencing Condition : 'a'  Pair : 1, 11   Masking
// and many more
// If we have a list with test values like 1 4 5 6 9 
// it will be of course very good to select tests values that are in one test pair
// like 1,9
// This will reduce the number of test values needed
//
// Now check that

bool Mcdc::isCompleteTestPairInCoverageResult(const McdcIndependencePair& mip, const CellVectorHeaderSet& cvhs)
{
	// Test if a complete Test pair is part of the Coverage Result
	// Resulting values that will be returned to function caller
	bool found{ false };					// We found a double match

	// We will check for only in test pairs with the same MCDC type
	const McdcType mcdcType[3]{ McdcType::UniqueCause, McdcType::UniqueCauseMasking, McdcType::Masking };
	// We are iterating over the sequence of MCDC types
	// Because we want to favour "Unique Cause" over "Unique Cause + Masking" over "Masking"
	// If we find something, we will return immediately and this will then have priority
	for (uint mcdcTypeIndex = 0; !found && (mcdcTypeIndex < 3); ++mcdcTypeIndex)
	{
		// Check given mcdcIndependencePair

		// If rwong type, then ignore and goto next
		if (mip.mcdcType != mcdcType[mcdcTypeIndex]) continue;

		// Get left and right of pair
		const uint v1{ mip.independencePair.first };
		const uint v2{ mip.independencePair.second	};
		// And check if they are both in one column of the coverage table
		CellVectorHeaderSet::iterator i1{ std::find_if(cvhs.begin(), cvhs.end(), [v1](const CellVectorHeader & cvh) {return (v1 == cvh.index); }) };
		if (cvhs.end() != i1)
		{	// This is an AND. Search 2 times and both results must be true
			CellVectorHeaderSet::iterator i2{ std::find_if(cvhs.begin(), cvhs.end(), [v2](const CellVectorHeader & cvh) {return (v2 == cvh.index); }) };
			if (cvhs.end() != i2)
			{
				// Yep, result is good
				found = true;		// Set return values
			}
		}
	}
	return found;
}




// Same as above. But only one test value needs to be part of test pair
// Of course this is essential, otherwise we can forget that test pair
bool Mcdc::isOnePartOfTestPairInCoverageResult(const McdcIndependencePair& mip, const CellVectorHeaderSet& cvhs)
{
	// Test if a complete Test pair is part of the Coverage Result
	bool found{ false };

	// Please note. The sequence  is impotant here.
	// We will find the first fiiting test pair in the below sequence of types
	const McdcType mcdcType[3]{ McdcType::UniqueCause, McdcType::UniqueCauseMasking, McdcType::Masking };

	// We are iterating over the sequence of MCDC types
	// Because we want to favour "Unique Cause" over "Unique Cause + Masking" over "Masking"
	// If we find something, we will return immediately and this will then have priority
	for (uint mcdcTypeIndex = 0; !found && (mcdcTypeIndex < 3); ++mcdcTypeIndex)
	{
		// If rwong type, then ignore and goto next
		if (mip.mcdcType != mcdcType[mcdcTypeIndex]) continue;

		// Get left and right of pair
		const uint v1{ mip.independencePair.first };
		const uint v2{ mip.independencePair.second };

		// This is an OR. Find either in the one or the other column
		CellVectorHeaderSet::iterator i1{ std::find_if(cvhs.begin(), cvhs.end(), [v1](const CellVectorHeader & cvh) {return (v1 == cvh.index); }) };
		if (cvhs.end() != i1)
		{
			found = true;		// Set return values
		}
		else
		{
			CellVectorHeaderSet::iterator i2{ std::find_if(cvhs.begin(), cvhs.end(), [v2](const CellVectorHeader & cvh) {return (v2 == cvh.index); }) };
			if (cvhs.end() != i2)
			{
				found = true;		// Set return values
			}
		}
	}
	return found;
}

// After MCDC Test pairs have been found by analysing and evaluating the boolen expressions
// abstract syntax tree, we want to identify a minimum test vector that covers all conditions.
// And this with a preference for unique cause MCDC test pairs.
// First we put all found results in a coverage table and reduce it to eliminate redundatend
// test values.
// Then we try to find a minimum set of test pairs and then finally a test vector
// Some heuristic methods are used to come to a desired conclusion

// Parameters symbolTable and mintermVector or only needed for output functions
void Mcdc::generateTestSets()
{
	// Dependent on complexity, find correct output stream 
	const ulong notDroppedElements{ coverage.countNotDroppedTableElements() };
	const bool predicateForOutputToFile{ (notDroppedElements > 30UL) };
	OutStreamSelection outStreamSelection(ProgramOption::pmcsc, predicateForOutputToFile);
	std::ostream& os{ outStreamSelection() };


	CoverageResult resultingCoverageSets;	// Result of coverage analysis. Contains on or more coverage sets
	std::set<TestSet> allTestSets;			// Selected test sets based on the above

	// For speed optimization.
	// If each condition has only exactly one test pair, then no reduction is necessary
	// Otherwise we will use the set cover ( unate covering solving method
	if (isMax1IndependencePairPerCondition())
	{
		// Simply get the test pairs in the desired form
		resultingCoverageSets = coverage.getCoverageResultWithoutReduction();
	}
	else
	{
		// Reduce the covergae table and find one or more minimum coverage sets
		// Set a best cost function for eliminating the desired redundant columns
		coverage.setBestCostFunctionForColumn(std::bind(&Mcdc::compareScoreOf2TestValues, this, std::placeholders::_1, std::placeholders::_2));
		// Do the Reduction, including petricks method
		resultingCoverageSets = coverage.reduce(os);
	}


	// Show the result to the user
	os << "\n\nTest Sets\n\n";

	// Check all Coverage sets
	for (uint i = 0; i < resultingCoverageSets.size(); ++i)
	{
		os << "\n-------- For Coverage set " << std::setw(3) << (i + 1) << "    ----------------------------------------------------\n\n";

		McdcIndependencePair resultingIndependencePair; // Found test pair for for this coverage set 
		TestSet testSetForOneVariable;					// And the found test set for one condition for the coverage set under evaluation
		//
		// Strategy:
		// If we have a test pair that has both test values beeing a part of the coverage set (independencePairFull),
		// then we favour this over a test pair that contains only one member of the test vector (independencePairPart)
		// (None is not allowed, then we continue the seacrh
		//
		// One variable may have more possible solutions in either the group "independencePairFull" or in the group of "independencePairPart"
		//
		// If there is more than one possible solution in one group, then we need to select only one.
		// The heuristics is here, to chose the test pair with the smallest number of different set bits

		// Check the resulting independece pairs grouped by influencing condition
		// So, for each influencing condidtion
		for (const std::pair<const cchar, McdcIndependencePairSet>& ippc : independencePairPerCondition)
		{
			bool found{ false };
			bool foundCompleteTestPair{ false };
			bool foundPartTestPair{ false };
			// Find a test pair for each variable
			TestVector resultingIndependencePairPerVariableFullPair{};
			TestVector resultingIndependencePairPerVariableHalfPair{};


			// Because there may be more than one test pair per condition, we want to check all 
			// possible test pairs per condition and then select the "best"
			// Go through all possible test pairs for that condition
			for (const McdcIndependencePair& mip : ippc.second)
			{
				// First check, if a complete test pair is part of one found coverage sets
				// This is a preferred solution. We will reduce the number of test values with that approach
				if (isCompleteTestPairInCoverageResult(mip, resultingCoverageSets[i]))
				{
					// Store this pair as a result
					resultingIndependencePairPerVariableFullPair.push_back(mip);
					foundCompleteTestPair = true;
				}
				else	// we did not find a test pair, containing both values, therefore search for a test pair containing at least one value
				{		// This must be in an else branch, because, if we would have already detected a value in the if branch
						//We would detect it here again
					if (isOnePartOfTestPairInCoverageResult(mip, resultingCoverageSets[i]))
					{
						// Found in this evaluation cycle
						resultingIndependencePairPerVariableHalfPair.push_back(mip);
						foundPartTestPair = true;
					}
				}
			}
			// Now we want to do some heuristics
			// If a test pair has both its value in the test vector, then we prefer this solution
			if (foundCompleteTestPair)
			{
				// There maybe more than on solution. If so, we want to find the "best"
				resultingIndependencePair = findBestResultingIndependencePair(resultingIndependencePairPerVariableFullPair);
				found = true;
			}
			else if (foundPartTestPair)
			{
				// If a test pair has both its value in the test vector, then we prefer this solution
				// But there was none.
				// Only 1 value of the pair was in the test vector. Better than nothing, take that
				// There maybe more than on solution. If so, we want to find the "best"
				resultingIndependencePair = findBestResultingIndependencePair(resultingIndependencePairPerVariableHalfPair);
				found = true;
			}
			// else nothing found. Do nothing

			if (found)
			{
				// Add first and second part of the found pair to the result
				testSetForOneVariable.insert(resultingIndependencePair.independencePair.first);
				testSetForOneVariable.insert(resultingIndependencePair.independencePair.second);

				// Show result to user. Show the test pair for this condition and for this coverage set
				os << "Test Pair for Condition '" << ippc.first << "':  " << std::setw(3) << resultingIndependencePair.independencePair.first
					<< ' ' << std::setw(3) << resultingIndependencePair.independencePair.second << "   (" << mcdcTypeToString(resultingIndependencePair.mcdcType) << ")\n";
			}
			// else nothing
			
		}
		// else, ignore, store nothing and check next
		printNotCoveredSymbols(os);

		// After we have found and shown test pairs for all conditions for this coverage set
		// We will show a vector of test values (derived from the test pairs)
		os << "\nResulting Test Vector:  ";
		for (const uint t : testSetForOneVariable)
		{
			os << t << ' ';
		}
		os << "\n\n";
		// And save the complete result in a vector grouped by all coverage sets
		allTestSets.insert(testSetForOneVariable);


	}

	// We went through all coverage sets
	// Now we want to show the recomended test set to the user
	printResult(allTestSets, os);

	// If it has not yet been printed to cout, then print it anyway
	if (!outStreamSelection.hasStdOut())
	{
		printResult(allTestSets, std::cout);
	}

	// Done
	// Main Purpose of whole software program is finsihed now
}

McdcIndependencePair Mcdc::findBestResultingIndependencePair(TestVector& resultingIndependencePairPerVariableXPair)
{
	uint bestSelectionIndex{ 0 };
	uint bestCostSum{ 0 };
	auto deltaBitCount = [](uint first, uint second) { return static_cast<uint>(MaxNumberOfBitsForEvaluation - std::abs(static_cast<int>(numberOfSetBits(first)) - static_cast<int>(numberOfSetBits(second)))); };
	
	//numberOfSetBits
	
	for (uint i = 0; i < narrow_cast<uint>(resultingIndependencePairPerVariableXPair.size()); ++i)
	{
		uint sum
		{ 
			(static_cast<uint>(McdcType::NONE) - static_cast<uint>(resultingIndependencePairPerVariableXPair[i].mcdcType)) * MaxNumberOfBitsForEvaluation +
			deltaBitCount(resultingIndependencePairPerVariableXPair[i].independencePair.first, resultingIndependencePairPerVariableXPair[i].independencePair.second)
		};
		if (sum > bestCostSum)
		{
			bestCostSum = sum;
			bestSelectionIndex = i;
		}
	}
	return resultingIndependencePairPerVariableXPair[bestSelectionIndex];
}


void Mcdc::printNotCoveredSymbols(std::ostream& os)
{
	os << '\n';
	for (cchar symbol : astUsedForMcdcCalculation.symbolTable.symbol)
	{
		if (!independencePairPerCondition.empty() && (independencePairPerCondition.count(symbol) == 0))
		{
			os << "*** No Test Pair for Condition  '" << symbol << "'\n";
		}
	}
}

// Show the result of all calculations to the user
// The last 3 parameters for for output only

// Example output

// -------------------------------------------------------------------------- -
// Testvector: Recommended Result : 1 4 5 6 9
//
//	1 : a = 0  b = 0  c = 0  d = 1    (1)
//	4 : a = 0  b = 1  c = 0  d = 0    (1)
//  5 : a = 0  b = 1  c = 0  d = 1    (0)
//	6 : a = 0  b = 1  c = 1  d = 0    (0)
//	9 : a = 1  b = 0  c = 0  d = 1    (0)
//

void Mcdc::printResult(const std::set<TestSet>& allTestSets, std::ostream& os)
{
	// If there is only one solution
	// Meaning the coverage set had only one member
	// If so, then this is the final result
	if (1 == allTestSets.size())
	{
		os << "\n\n\n---------------------------------------------------------------------------\nTestvector: Final Result: ";
		// Show the list of test values
		for (const uint t : *allTestSets.begin())
		{
			os << t << ' ';
		}
		os << "\n\n";

		// As an additional service to the user, we will translate the test value to the settings for the conditions
		for (const uint t : *allTestSets.begin())
		{
			os << std::setw(5) << t << ":  ";
			SymbolType::size_type v{ astUsedForMcdcCalculation.symbolTable.symbol.size() - 1 };
			for (const cchar c : astUsedForMcdcCalculation.symbolTable.symbol)
			{
				os << c << '=' << ((0U !=  (t&  bitMask[v])) ? '1' : '0') << "  ";
				--v;
			}
			const uint resultingDecision = astUsedForMcdcCalculation.evaluateTree(t) ? 1U : 0U;
			os << "  (" << resultingDecision << ")\n";
		}
		os << "\n\n";
	}
	else
	{
		// There were more than one possible solutions
		// We will find the minimum soze solution and if there are several eqaul sized minimums
		// then we will take the first

		uint min{ UINT_MAX };
		TestSet minTestSet;
		// Look for the mimimum size, the test vector with the least values
		for (const TestSet& ts : allTestSets)
		{
			if (ts.size() < min)
			{
				min = narrow_cast<uint>(ts.size());
				minTestSet = ts;
			}
		}


		// And now the same as above
		os << "\n\n\n---------------------------------------------------------------------------\nTestvector: Recommended Result: ";
		// Show the list of test values
		for (const uint t : minTestSet)
		{
			os << t << ' ';
		}
		os << "\n\n";

		// As an additional service to the user, we will translate the test value to the settings for the conditions
		for (const uint t : minTestSet)
		{
			os << std::setw(5) <<t << ":  ";
			SymbolType::size_type v{ astUsedForMcdcCalculation.symbolTable.symbol.size() - 1 };
			for (const cchar c : astUsedForMcdcCalculation.symbolTable.symbol)
			{
				os << c << '=' << ((0U != (t&  bitMask[v])) ? '1' : '0') << "  ";
				--v;
			}
			const uint resultingDecision = astUsedForMcdcCalculation.evaluateTree(t) ? 1U : 0U;
			os << "  (" << resultingDecision << ")\n";

		}
		os << "\n\n";
	}
}



// Find out, if for every condition there is excatly one covering test
// In that case, we do not need to rund the set cover proble / unate covering solver
bool Mcdc::isMax1IndependencePairPerCondition()
{
	bool thereIsExcatlyOneTestPairPerCondition{ true };		// Assume positive result
	for (const std::pair<cchar, McdcIndependencePairSet>& ippc : independencePairPerCondition)
	{
		if (ippc.second.size() > 1)
		{
			// As soon as there is on conditions with more test tests than one
			thereIsExcatlyOneTestPairPerCondition = false;
			break; // Stop any further searching
		}
	}
	return thereIsExcatlyOneTestPairPerCondition;
}


// Brute Force Evaluation
// Try to find test pairs that fullfill the requirement of MCDC
// A bbolean expressions is given. From this we will construct an abstract syntax tree.
// Then we run a dnested loop and create test vectors for each test value with the other
// Then number of combinations is (k(k-1)/2) with k being 2^(number of conditions)
// After 2 AST have been evaluated we perform a tree xor to find the difference between 2 trees
// And finally to check, if it is a valid MCDC test pair
void Mcdc::findMcdcIndependencePairs(VirtualMachineForAST& ast)
{

	sint counter{ 0 };

	sint counterUniqueCauseMCDC{ 0 };
	sint counterUniqueCauseMaskingMCDC{ 0 };
	sint counterMaskingMCDC{ 0 };
	std::set<uint> testSetUniqueCauseMCDC{};
	std::set<uint> testSetUniqueCauseMaskingMCDC{};
	std::set<uint> testSetMaskingMCDC{};

	// Store a local copy
	astUsedForMcdcCalculation = ast;


	// Number of different conditions in the given AST
	const uint maxConditions{ ast.maxConditionsInTree() };

	{	// We want to close the file before calling the next function
		// Therfore we open a new block
		
		// Determine the desired output stream
		const bool predicateForOutputToFile{ (maxConditions > 3) };
		OutStreamSelection outStreamSelection(ProgramOption::pmastc, predicateForOutputToFile);
		std::ostream& osMcdc{ outStreamSelection() };
		osMcdc << "\n\n\n\n-------------------------------------------------- Searching for MCDC Test pairs \n\n\n";


		// We will use this for calculating all AST for all possible test values
		VirtualMachineForAST astForEvaluation{ ast };
		// The we will tree XOR the 2 calculated AST to find out, if this is an influencing set
		VirtualMachineForAST astInfluenceSet{ ast };

		// Calculate maximum number of possible tests. We will compare everything with the other 
		const uint maxLoop{ static_cast<uint>(1U << static_cast<uint>(maxConditions)) };
		// Number of nodes in the AST	
		const uint astSize{ narrow_cast<uint>(astInfluenceSet.ast.size()) };

		// We will calculate results for the AST for all possible test values
		std::vector<VirtualMachineForAST> astPreEvaluated;
		astPreEvaluated.reserve(maxLoop);	// Wow, what a waste of memory

		// For all possible test values, depending on the number of conditions
		for (uint i = 0U; i < maxLoop; ++i)
		{
			static_cast<void>(astForEvaluation.evaluateTree(i));		// EValuate the result
			astPreEvaluated.push_back(astForEvaluation);	// And store it for later reference
		}


		// Check all possible test pairs, if they have an influencing condition
		for (uint outer = 0U; outer < (maxLoop - 1); ++outer)
		{
			for (uint inner = outer + 1; inner < maxLoop; inner++)
			{
				// Perform TREE XOR. To be able to check, what conditions and what operators changed values
				// Tree exor means: we perfrom an XOR for every node in the AST
				// If we have a not evaluated node, then the result is 0
				// With that, we can check, what values (nodes) changed between 2 test pairs
				// The result of this activity is the "influence tree"
				// It is not important, if the source values were 0.
				// If the complete path, starting from the condition to the root, is all true,
				// than the condition has an influence.
				// If only one condition changes (only one condition is true) and this is an influencing condition
				// then we have unique cause MCDC. If more conditions changed, but only one of them is in an
				// influencing path, then we have masking MCDC
				// For strongly coupled conditions (meaningm the same condition is in the AST more than one)
				// Unique cause is per definition not possible.
				// If only one of the multiple same conditions is part of the influencing path, 
				// then we talk about Unique+Maskin MCDC
				for (uint i = 0; i < astSize; ++i)
				{
					astInfluenceSet.ast[i].value = (astPreEvaluated[outer].ast[i].value != astPreEvaluated[inner].ast[i].value) &&
						!astPreEvaluated[outer].ast[i].notEvaluated &&
						!astPreEvaluated[inner].ast[i].notEvaluated;
				}
	
				// Now check the influence tree. And determine the type of MCDC (if any) and the influcening condition for this test pair
				const auto [mcdcType, influencingCondition] = getMcdcType(astInfluenceSet.ast);


				// Now, depending on the found MCDC type or if MCDC at all
				// Inform the user about what we found
				// And save the test values for further processing
				switch (mcdcType)
				{
				case McdcType::UniqueCause:
					if (!programOption.option[ProgramOption::dnpast].optionSelected)
					{
						osMcdc << "\n----------------------- Found   Unique Cause          MCDC Test pair for condition: " << influencingCondition << "     Test Pair: " << outer << ' ' << inner << '\n';
					}
					testSetUniqueCauseMCDC.insert(outer);
					testSetUniqueCauseMCDC.insert(inner);
					++counterUniqueCauseMCDC;
					break;
				case McdcType::UniqueCauseMasking:
					if (!programOption.option[ProgramOption::dnpast].optionSelected)
					{
						osMcdc << "\n----------------------- Found   Unique Cause+Masking  MCDC Test pair for condition: " << influencingCondition << "     Test Pair: " << outer << ' ' << inner << '\n';
					}
					testSetUniqueCauseMaskingMCDC.insert(outer);
					testSetUniqueCauseMaskingMCDC.insert(inner);
					++counterUniqueCauseMaskingMCDC;
					break;
				case McdcType::Masking:
					if (!programOption.option[ProgramOption::dnpast].optionSelected)
					{
						osMcdc << "\n----------------------- Found   Masking               MCDC Test pair for condition: " << influencingCondition << "     Test Pair: " << outer << ' ' << inner << '\n';
					}
					testSetMaskingMCDC.insert(outer);
					testSetMaskingMCDC.insert(inner);
					++counterMaskingMCDC;
					break;
				case McdcType::NONE: //fallthrough
				default:
					if (!programOption.option[ProgramOption::dnpast].optionSelected)
					{
						osMcdc << "....................... Could not be identified as MCDC of any type.  Test Pair: " << outer << ' ' << inner << '\n';
					}
					break;
				}

				// If we have a valid MCDC Test Pair
				if (McdcType::NONE != mcdcType)
				{
					// Then we add it to all internal containers
					add(McdcIndependencePair(mcdcType, outer, inner, influencingCondition));
					// And show 3 ASTs
					if (!outStreamSelection.isNull() && !programOption.option[ProgramOption::dnpast].optionSelected)
					{
						++counter;
						osMcdc << "\n-------------------------------------- AST for value: " << outer << "\n\n";
						astPreEvaluated[outer].printTree(osMcdc);
						osMcdc << "\n-------------------------------------- AST for value: " << inner << "\n\n";
						astPreEvaluated[inner].printTree(osMcdc);
						osMcdc << "\n-------------------------------------- AST for influencing condition check\n\n";
						astInfluenceSet.printTree(osMcdc);
						osMcdc << "\n\n";
					}
				}
				else if (!outStreamSelection.isNull() && programOption.option[ProgramOption::paast].optionSelected && !programOption.option[ProgramOption::dnpast].optionSelected)
				{
					// It is alsa possible to show all ASTs regardless if they are MCDC pair or not. Controlled via command line
					// That is the case here
					osMcdc << "\n-------------------------------------- AST for value: " << outer << "\n\n";
					astPreEvaluated[outer].printTree(osMcdc);
					osMcdc << "\n-------------------------------------- AST for value: " << inner << "\n\n";
					astPreEvaluated[inner].printTree(osMcdc);
					osMcdc << "\n-------------------------------------- AST for influencing condition check\n\n";
					astInfluenceSet.printTree(osMcdc);
					osMcdc << "\n\n";
				}

			}
		}

		// So far we showed independence pairs. Now we ant to show the test values in a list
		if (testSetUniqueCauseMCDC.size() > 0)
		{
			osMcdc << "\n\n----------------------\nUnique Cause MCDC Test Set\n";
			for (const uint ts : testSetUniqueCauseMCDC)
			{
				osMcdc << ts << ' ';
			}
			osMcdc << '\n';
		}
		if (testSetUniqueCauseMaskingMCDC.size() > 0)
		{
			osMcdc << "\n\n----------------------\nUnique Cause + Masking MCDC Test Set\n";
			for (const uint ts : testSetUniqueCauseMaskingMCDC)
			{
				osMcdc << ts << ' ';
			}
			osMcdc << '\n';
		}
		if (testSetMaskingMCDC.size() > 0)
		{
			osMcdc << "\n\n----------------------\nMasking MCDC Test Set\n";
			for (const uint ts : testSetMaskingMCDC)
			{
				osMcdc << ts << ' ';
			}
			osMcdc << '\n';
		}
	}

	// After we found all test values and pairs, we want to select minimum
	// necessary test sets
	// For that we build a coverage table and reduce it
	initializeMcdcCoverageMethod();
}





// Get tree xored AST and check, if we have at all, anf if what MCDC type
// It is not important, if the source values were 0.
// If the complete path, starting from the condition to the root, is all true,
// than the condition has an influence.
// If only one condition changes (only one condition is true) and this is an influencing condition
// then we have unique cause MCDC. If more conditions changed, but only one of them is in an
// influencing path, then we have masking MCDC
// For strongly coupled conditions (meaningm the same condition is in the AST more than one)
// Unique cause is per definition not possible.
// If only one of the multiple same conditions is part of the influencing path, 
// then we talk about Unique+Masking MCDC

std::pair<McdcType, cchar> Mcdc::getMcdcType(AST& astInfluencingTree)
{
	// For function result
	McdcType resultingMcdcType{ McdcType::NONE };
	cchar lastInfluencingSymbol{ ' ' };
	constexpr uint MaxIndexForTerminal{ static_cast<uint>('z') + 1U };

	// We need to count, how often a condition is present in the influencing tree
	uint countOfOneModfiedPerVariable[MaxIndexForTerminal];
	std::memset(&countOfOneModfiedPerVariable[0], 0, sizeof(countOfOneModfiedPerVariable));

	// For Masking we need to count all modified conditions
	uint modifiedCondition{ 0U };
	// And, how often an influencing symbol is present
	uint influencingSymbolCounter{ 0U };

	bool oneInfluencingPathFound{ false }; // Check if at least one influencing path is available
	const uint astSize{ narrow_cast<uint> (astInfluencingTree.size()) };

	// Check the complete influencing tree
	for (uint i = 0; i < astSize; ++i)
	{
		// Reference to current node
		const AstNode& astNode{ astInfluencingTree[i] };

		// Is this a leaf, meaning do we have a condition here
		// A condition would be the staring point for a path analysis
		if ((NumberOfChildren::zero == astNode.numberOfChildren) && astNode.value)
		{
			// Yes, Leaf. Read the condition. Only lowercase is important for counting
			cchar inputTerminalSymbol{ astNode.tokenWithAttribute.inputSymbolLowerCase };

			// Count the number of that condition that have value true. 
			// That means the variable has been changed from on test to the other
			++countOfOneModfiedPerVariable[inputTerminalSymbol];

			// Count overall number of modifed conditions, regardless, what condition is is
			++modifiedCondition;

			// Check if the condition is influencing
			// That means all true in a path up to the root node
			uint indexNodeUnderEvaluation{ i };
			bool pathIsTrue{ true };
			do
			{
				// All values along the path must be true. If not, we can terminate this loo´p
				pathIsTrue = pathIsTrue && astInfluencingTree[indexNodeUnderEvaluation].value;
				// Set next node to check to the parent of this node. So go up the path to the root
				indexNodeUnderEvaluation = astInfluencingTree[indexNodeUnderEvaluation].parentID;

				// COntinue until we reached the root node and as long as all elements in the path are true
			} while (pathIsTrue && (indexNodeUnderEvaluation != ASTNoLinkedElement));

			// OK, we found a comlte true path. So this condition is influencing
			if (pathIsTrue)
			{
				// Count all influencing conditions
				++influencingSymbolCounter;

				// And remember the name of the condition
				// If there is only one influencing variable in the whole tree, then this it is
				// This is important to detect the type of MCDC
				lastInfluencingSymbol = inputTerminalSymbol;
			}
			// So, note, if at least one influencing path could be found
			oneInfluencingPathFound = oneInfluencingPathFound || pathIsTrue;
		}

	}

	// There was at least one influencing path. So it could be MCDC
	if (oneInfluencingPathFound)
	{
		// Sum up all modified conditions from the per condition count
		uint overallModifiedVariables{ 0 };
		for (uint i = 0; i < MaxIndexForTerminal; ++i)
		{
			overallModifiedVariables += countOfOneModfiedPerVariable[i];
		}


		// Evaluation:

		// Precondition for MCDC is:

		// There must be only one influencing variable --> Masking MCDC
		// If additionally only one variable changed value --> Unique Cause MCDC

		// For strongly coupled variables (e.g Variable appears mor than one time in a boolean expression)
		// masking is allowed for that variable. That means the strongly coupled variable may have
		// several truth values in the influence set

		// Check for MCDC

		// There must be only one influencing variable
		if (1U == influencingSymbolCounter)
		{
			// Unique cause MCDC. Exactly one influencing condition / exactly one modified condition
			if (1 == modifiedCondition)
			{
				resultingMcdcType = McdcType::UniqueCause;

			}
			// Unique Cause + Masking MCDC
			// Only one influencing condition. But more modified condition. and thos all of the same name.
			else if ((1 < modifiedCondition) && (1 < countOfOneModfiedPerVariable[lastInfluencingSymbol]) && (countOfOneModfiedPerVariable[lastInfluencingSymbol] == overallModifiedVariables))
			{
				resultingMcdcType = McdcType::UniqueCauseMasking;
			}
			// Masking
			// Only one influencing condition. But more different modified conditions
			else if (1 < modifiedCondition)
			{
				resultingMcdcType = McdcType::Masking;
			}
		}
	}
	// Now we kno the type and the influencing condition
	return std::make_pair(resultingMcdcType, lastInfluencingSymbol);
}

