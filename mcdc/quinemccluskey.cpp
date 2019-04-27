
// Implementation of Quine & McCluskey Method for the minimization of boolean expressions

// Quine and & McCluskey Tabular Deterministic Method to extract Prime Implicants from
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



#include "pch.h"

#include "coverage.hpp"

#include "quinemccluskey.hpp"
#include "cloption.hpp"

#include <cmath>
#include <thread>
#include <future>

#include <iostream>
#include <iomanip>
#include <sstream>

void  QuineMcluskey::initializeCoverageForPrimeImplicants(Coverage& coverageForPrimeImplicants, MintermVector& mv, const SymbolTable &symbolTable)
{
	for (const MinTermType minTerm : mv)
	{
		coverageForPrimeImplicants.addRow(minTerm);
	}
	for (PrimeImplicantType primeImplicant : primeImplicantSetResult)
	{
		std::string primeImplicantString = primeImplicant.toString(symbolTable);
		coverageForPrimeImplicants.addColumn(primeImplicant.term, primeImplicantString, primeImplicant);
	}
}

namespace CellVectorHeaderSort
{
	bool comp(const CellVectorHeader &left, const CellVectorHeader &right) noexcept
	{
		return left.textInfo < right.textInfo;
	}
}

std::string QuineMcluskey::getMinimumDisjunctiveNormalForm(MintermVector &mv, const SymbolTable &symbolTable, const std::string& source)
{
	initiate(mv);
	reduce();
	printReductionTable(symbolTable, source);

	std::string minimumDisjunctiveNormalForm;
	const bool predicateForOutputToFile = (symbolTable.numberOfSymbols() > 5);
	OutStreamSelection outStreamSelection(ProgramOption::ppirtc, predicateForOutputToFile);
	std::ostream& os = outStreamSelection();

	os << "\n\n\n------------------ Try to find Minmum Disjunctive Normal form for boolean expression:\n\n'" << source << "'\n\n";

	Coverage coverageForPrimeImplicants;
	initializeCoverageForPrimeImplicants(coverageForPrimeImplicants, mv, symbolTable);
	coverageForPrimeImplicants.setCheckForCoverFunction(checkCoverForCell);
	coverageForPrimeImplicants.checkAllCellsForCover();
	CoverageResult cr = coverageForPrimeImplicants.reduce(os, true);

	// Some heuristic methods . . .
	// 1. Count number of literals in coverage result
	// 2. count number of lowercase literals in coverage result
	// 3. Count NUmber of terms in coverage result

	// To to this, we first need the maximum number of literals in a coverage set over all coverage sets
	// And we need to have the maximum number of terms in the coverage sets

	{
		uint maxTerms = narrow_cast<uint>(std::max_element(cr.begin(), cr.end(), [](const CellVectorHeaderSet &cvhsLeft, const CellVectorHeaderSet &cvhsRight) noexcept {return cvhsLeft.size() < cvhsRight.size(); })->size());
		std::vector<uint> numberOfLiterals;
		std::vector<uint> numberOfLowerCaseLiterals;

		for (CellVectorHeaderSet &cvhs : cr)
		{
			std::vector<cchar> literals;
			std::vector<cchar> lowerCaseLiterals;
			for (const CellVectorHeader &cvh : cvhs)
			{
				for (const cchar c : cvh.textInfo)
				{
					literals.push_back(c);
					if (c > 'Z')
					{
						lowerCaseLiterals.push_back(c);
					}
				}
			}
			numberOfLiterals.push_back(static_cast<uint>(literals.size()));
			numberOfLowerCaseLiterals.push_back(static_cast<uint>(lowerCaseLiterals.size()));
		}


		// If no best solution then use first soluiton
		uint maxNumberOfLiterals = *std::max_element(numberOfLiterals.begin(), numberOfLiterals.end());
		uint maxResult{ 0 };
		uint maxResultIndex{ 0 };
		for (uint i = 0; i < cr.size(); ++i)
		{
			const uint result = (maxNumberOfLiterals - numberOfLiterals[i]) * 5 +
				(maxNumberOfLiterals - numberOfLowerCaseLiterals[i]) * 4 +
				(maxTerms - narrow_cast<uint>(cr[i].size())) * 8;
			if (result > maxResult)
			{
				maxResult = result;
				maxResultIndex = i;
			}
		}


		const uint NumberOfTermsInBestSolution = narrow_cast<uint>(cr[maxResultIndex].size());
		uint TermCounter{ 0 };

		// Build minimum disjunctive normal form, Minimun DNF
		for (const CellVectorHeader &cvh : cr[maxResultIndex])
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



//minTermRowHeader
//primeImplicantColumnHeader
bool checkCoverForCell(const CellVectorHeader &minTermRowHeader, CellVectorHeader &primeImplicantColumnHeader)
{
	const PrimeImplicantType resultingPrimeImplicant = std::any_cast<PrimeImplicantType>(primeImplicantColumnHeader.userData);
	const MinTermType mtnMinTerm = minTermRowHeader.index;

	bool result{ false };

	const MinTermNumber mtnPrimeImplicant = resultingPrimeImplicant.term;
	const MinTermNumber primeImplicantMask = resultingPrimeImplicant.mask;
	const MinTermNumber primeImplicantMaskNegated = ~resultingPrimeImplicant.mask;
	const MinTermNumber mtnMinTermMasked = mtnMinTerm & primeImplicantMaskNegated;
	const MinTermNumber mtnPrimeImplicantMasked = mtnPrimeImplicant & primeImplicantMaskNegated;

	if (mtnMinTermMasked == mtnPrimeImplicantMasked)
	{
		result = true;
	}
	return result;
}









QuineMcluskey::QuineMcluskey() noexcept
{
	if (programOption.option[ProgramOption::sfqmt].optionSelected)
	{
		processLowerAndUpperMintermOnly = false;
	}
}


void QuineMcluskey::initiate(MintermVector &mv)
{

	BitsAndMinTerms bitsAndMinTerms(MAX_NUMBER_OF_BOOLEAN_VARIABLES+1);
	TableEntry tableEntry;

	for (const MinTermNumber &mtn : mv)
	{
		tableEntry.mintermLower = tableEntry.mintermUpper = mtn;
		if (!processLowerAndUpperMintermOnly)
		{
			tableEntry.matchedMinterm.clear();
			tableEntry.matchedMinterm.insert(mtn);
		}
		const NumberOfBitsCountType nb = numberOfSetBits(mtn);

		
		bitsAndMinTerms[nb].push_back(tableEntry);
	}
	reductionTable.push_back(std::move(bitsAndMinTerms));
}




void QuineMcluskey::compareTwoEntries(TableForBitCount& upper, TableForBitCount& lower, TableForBitCount& output)
{
	
	// Compare alle table entries with the other table entries
	for (const TableEntry &teUpper : upper)
	{
		for (const TableEntry &telower : lower)
		{
			// The mask (The so far eliminated Bit Positions) must be equal
			if (teUpper.maskForEliminatedBit == telower.maskForEliminatedBit)
			{
				const MinTermType termDifference = (teUpper.mintermLower ^ telower.mintermLower);
				// If there is only a difference in one bit
				if (!(termDifference & (termDifference - 1)))
				{
					// We found a difference in exactly one bit. Mark both terms
					teUpper.matchFound = true;
					telower.matchFound = true;

					if (processLowerAndUpperMintermOnly)
					{
						// Create a new tableEntry for the next Reduction Table column
						output.push_back(TableEntry(telower.mintermLower, teUpper.mintermUpper, termDifference + telower.maskForEliminatedBit));
					}
					else
					{
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
	TableEntryCompare tableEntryCompare;
	std::sort(output.begin(), output.end(), tableEntryCompare);
	output.erase(std::unique(output.begin(), output.end()), output.end());
}

void QuineMcluskey::compareFromBitCountUpperToBitCountLower(uint indexReductionTableColumn, uint indexUpper, uint indexLower)
{
	for (uint ui = indexUpper; ui > indexLower; --ui)
	{
		compareTwoEntries(reductionTable[indexReductionTableColumn][ui], reductionTable[indexReductionTableColumn][static_cast<ull>(ui) - 1], reductionTable[static_cast<ull>(indexReductionTableColumn) + 1][static_cast<ull>(ui) - 1]);
	}
}



void QuineMcluskey::reduce()
{
	uint currentReductionTableColumn{ 0 };
	bool doIterate{ false };
	do
	{
		reductionTable.emplace_back(BitsAndMinTerms(33));

		const uint upper = getHighestIndexOfBitCountEntry(currentReductionTableColumn);
		const uint lower = getLowestIndexOfBitCountEntry(currentReductionTableColumn);

		BitCountRangeVector bcrv = calculateBoundsForMultiThreading(upper, lower);
		if (bcrv.size() > 0)
		{
			std::future<void> futures[numberOfThreads];
			for (uint ui = 0; ui < bcrv.size(); ++ui)
			{
				uint u = bcrv[ui].first;
				uint l = bcrv[ui].second;

				futures[ui] = std::async(&QuineMcluskey::compareFromBitCountUpperToBitCountLower, this, currentReductionTableColumn, u, l);
			}
			for (uint ui = 0; ui < bcrv.size(); ++ui)
			{
				futures[ui].wait();
			}
		}
		else
		{
			compareFromBitCountUpperToBitCountLower(currentReductionTableColumn, upper, lower);
		}
		collectPrimeImplicants(currentReductionTableColumn);

		++currentReductionTableColumn;
		// Check if done or if further iteration necessary
		doIterate = checkIfFurtherEvaluationNecessary(currentReductionTableColumn);
	} while (doIterate);

	//return primeImplicantSetResult;
}


void QuineMcluskey::collectPrimeImplicants(uint indexReductionTableColumn)
{
	for (const TableForBitCount &tfbc : reductionTable[indexReductionTableColumn])
	{
		if (tfbc.size()) for (TableEntry te : tfbc)
		{
			if (!te.matchFound)
			{
				primeImplicantSetResult.insert(std::move(PrimeImplicantType(te.mintermLower, te.maskForEliminatedBit)));
			}
		}
	}
}


bool QuineMcluskey::checkIfFurtherEvaluationNecessary(uint indexReductionTableColumn)
{
	bool result = false;
	for (const TableForBitCount &tfbc : reductionTable[indexReductionTableColumn])
	{
		if (tfbc.size())
		{
			result = true; break;
		}
	}
	return result;
}


uint QuineMcluskey::getHighestIndexOfBitCountEntry(uint indexReductionTableColumn)
{
	uint result{ 0 };

	for (uint ui = 0; ui < reductionTable[indexReductionTableColumn].size(); ++ui)
	{
		if (reductionTable[indexReductionTableColumn][ui].size() > 0)
		{
			result = ui;
		}
	}

	return result;
}


uint QuineMcluskey::getLowestIndexOfBitCountEntry(uint indexReductionTableColumn)
{
	uint result{ 0 };
	for (uint ui = 0; ui < reductionTable[indexReductionTableColumn].size(); ++ui)
	{
		if (reductionTable[indexReductionTableColumn][ui].size() > 0)
		{
			result = ui;
			break;
		}
	}
	return result;
}

QuineMcluskey::BitCountRangeVector QuineMcluskey::calculateBoundsForMultiThreading(uint upper, uint lower)
{
	BitCountRangeVector result;
	const uint middle = static_cast<uint>(std::ceil((upper - lower) / 2 + 1));

	sint delta = 1;
	sint currentForDirectionLower = static_cast<sint>(middle);
	sint nextForDirectionLower = static_cast<sint>(middle) - delta;

	sint currentForDirectionUpper = static_cast<sint>(middle) + delta;
	sint nextForDirectionUpper = static_cast<sint>(middle);

	const uint maxLoopCount = (numberOfThreads >> 1);
	const uint lastLoopCount = maxLoopCount - 1;

	for (uint i = 0; i < maxLoopCount; ++i)
	{
		if (currentForDirectionLower > narrow_cast<sint>(lower))
		{
			if (nextForDirectionLower < narrow_cast<sint>(lower))
			{
				nextForDirectionLower = narrow_cast<sint>(lower);
			}
			if (i == lastLoopCount)
			{
				result.push_back(BitCountRange(currentForDirectionLower, lower));
			}
			else
			{
				result.push_back(BitCountRange(currentForDirectionLower, nextForDirectionLower));
			}
		}
		if (nextForDirectionUpper < narrow_cast<sint>(upper))
		{
			if (currentForDirectionUpper > narrow_cast<sint>(upper))
			{
				currentForDirectionUpper = narrow_cast<sint>(upper);
			}
			if (i == lastLoopCount)
			{
				result.push_back(BitCountRange(upper, nextForDirectionUpper));
			}
			else
			{
				result.push_back(BitCountRange(currentForDirectionUpper, nextForDirectionUpper));
			}
		}

		currentForDirectionLower = nextForDirectionLower;
		nextForDirectionLower -= delta;

		nextForDirectionUpper = currentForDirectionUpper;
		currentForDirectionUpper += delta;

		delta = delta << 1;

	}
	return result;
}



std::string PrimeImplicantType::toString(const SymbolTable &symbolTable) const
{
	std::ostringstream result;
	const uint symbolCount = symbolTable.numberOfSymbols();
	MinTermNumber maskSelector = bitMask[symbolCount - 1];
	std::set<cchar>::iterator symbolIterator = symbolTable.symbol.begin();

	for (uint ui = 0; ui < symbolCount; ++ui)
	{
		if (!(maskSelector & mask))
		{
			const MinTermNumber mt = term;
			const MinTermNumber mtMasked = mt & maskSelector;
			result << (mtMasked ? *symbolIterator : static_cast<cchar>((*symbolIterator) - ('a' - 'A')));
		}
		maskSelector = maskSelector >> 1;
		++symbolIterator;
	}
	return result.str();
}



void QuineMcluskey::printReductionTable(const SymbolTable &symbolTable, const std::string& source)
{
	const uint maxNumberOfBits = narrow_cast<uint>(symbolTable.symbol.size());
	const bool predicateForOutputToFile = (maxNumberOfBits > 6);
	OutStreamSelection outStreamSelection(ProgramOption::pqmtc, predicateForOutputToFile);
	std::ostream& os = outStreamSelection();

	os << "------------------ Print Quine McCluskey Reduction tables for boolean expression\n\n'" << source << "'\n\n";
	for (uint currentTableIndex = 0; currentTableIndex < reductionTable.size()-1; ++currentTableIndex)
	{
		os << "\nReduction Loop " << currentTableIndex + 1 << "-----------------------------\n\n";
		os << "#     B#    ";
		for (const cchar c : symbolTable.symbol)
		{
			os << c << ' ';
		}
		os << "   M D      MT Set\n";
		uint counter{ 1 };

		const uint upper = getHighestIndexOfBitCountEntry(currentTableIndex);
		const uint lower = getLowestIndexOfBitCountEntry(currentTableIndex);

		for (uint bitCount = lower; bitCount <= upper; ++bitCount)
		{
			for (TableEntry &te : reductionTable[currentTableIndex][bitCount])
			{
				os << std::left << std::setw(6)<< counter++<< std::setw(4) << narrow_cast<uint>(bitCount) << "  ";

				const MinTermNumber mtn = te.mintermLower;
				const MinTermNumber deletedPos = te.maskForEliminatedBit;
				MinTermNumber bitMaskLocal =bitMask[maxNumberOfBits - 1];

				for (uint bitIndex = 0; bitIndex < maxNumberOfBits; ++bitIndex)
				{
					cchar output = '0';
					if (deletedPos & bitMaskLocal)
					{
						output = '-';
					}
					else
					{
						if (mtn & bitMaskLocal)
						{
							output = '1';
						}
					}
					bitMaskLocal = bitMaskLocal >> 1;
					os << output << ' ';
				}
				if (currentTableIndex)
				{
					os << "   " << (te.matchFound ? 'X' : '_') << ' ' << std::setw(4) << deletedPos << "   ";
				}
				else
				{
					os << "   " << (te.matchFound ? 'X' : '_') <<  "        ";
				}

				if (processLowerAndUpperMintermOnly)
				{
					os << te.mintermLower << ' ' << te.mintermUpper << '\n';
				}
				else
				{
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
	os << "------------------ Prime Implicants  after Quine McCluskey Minimization for boolean expression\n'" << source << "'\n\n";

	for (const PrimeImplicantType& resultingPrimeImplicant : primeImplicantSetResult)
	{
		os << resultingPrimeImplicant.toString(symbolTable) << "  ";
	}
	os << "\n\n";
}
















