// mcdc.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include "pch.h"


#include "compiler.hpp"
#include "virtualmachine.hpp"
#include "mintermcalculater.hpp"
#include "quinemccluskey.hpp"

#include "petrick.hpp"
#include "mcdc.hpp"
#include "coverage.hpp"
#include "ast.hpp"


#include <bitset>
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <cctype>

#include <functional>
#include <utility>

void Mcdc::add(McdcIndependencePair mcdcIndependencePair)
{
	testVector.push_back(mcdcIndependencePair);
	switch (mcdcIndependencePair.mcdcType)
	{
	case  McdcType::UniqueCause:
		//mcdcIndependencePairUniqueCause.push_back(mcdcIndependencePair);
		testPerMcdcType[McdcType::UniqueCause].insert(mcdcIndependencePair.independencePair.first);
		testPerMcdcType[McdcType::UniqueCause].insert(mcdcIndependencePair.independencePair.second);
		break;
	case McdcType::UniqueCauseMasking:
		//mcdcIndependencePairUniqueCauseMasking.push_back(mcdcIndependencePair);
		testPerMcdcType[McdcType::UniqueCauseMasking].insert(mcdcIndependencePair.independencePair.first);
		testPerMcdcType[McdcType::UniqueCauseMasking].insert(mcdcIndependencePair.independencePair.second);
		break;
	case McdcType::Masking:
		//mcdcIndependencePairMasking.push_back(mcdcIndependencePair);
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

sint Mcdc::calculateSumForOneTest(uint test)
{
	sint resultingSum{ 0 };
	for (auto a : independencePairPerCondition)
	{
		// Iterate over all independence pairs
		for (auto b : a.second)
		{
			if ((b.independencePair.first == test) || (b.independencePair.second == test))
			{
				switch (b.mcdcType)
				{
				case McdcType::UniqueCause:
					resultingSum += 64;
					break;
				case McdcType::UniqueCauseMasking:
					resultingSum += 32;
					break;
				case McdcType::Masking:
					resultingSum += 16;
					break;
				case McdcType::NONE:
					break;
				}
				resultingSum += (MaxNumberOfBitsForEvaluation-numberOfSetBits(test));
			}
		}
	}
	return resultingSum;
}


sint Mcdc::compare2TestValues(const TableCellVector &left, const TableCellVector &right)
{
	const sint sumFirst{ calculateSumForOneTest(left.cellVectorHeader.index) };
	const sint sumSecond{ calculateSumForOneTest(right.cellVectorHeader.index) };
	return (sumFirst - sumSecond);
}



void  Mcdc::checkForCoverage()
{
	

	
	const bool predicateForOutputToFile = (narrow_cast<uint>(testSet.size()) > 100);

	OutStreamSelection outStreamSelectionMcdc(ProgramOption::pmtpc, predicateForOutputToFile);
	std::ostream& os = outStreamSelectionMcdc();

	os << "\n\nFound Testvectors\n\n";

	if (independencePairPerCondition.size() > 0)
	{
		sint cc = 1;
		for (auto a : independencePairPerCondition)
		{
			for (auto b : a.second)
			{
				os << "ID: " << std::setw(3) << cc << "  Influencing Condition: '" << a.first << "'  Pair: " << std::setw(2) << b.independencePair.first << ", " << std::setw(2) << b.independencePair.second << "   " << mcdcTypeToString(b.mcdcType) << '\n';
				++cc;
			}
		}
		os << "\n\n";
	}

	// Iterate over conditions
	uint rowIndex{ 0 };
	for (const cchar c : conditionSet)
	{
		coverage.addRow(rowIndex, std::string(1,c));
		++rowIndex;
	}
	for (const uint t : testSet)
	{
		coverage.addColumn(t,std::to_string(t));
	}

	uint row = 0;
	for (const auto tv : testPerInfluencingVariable)
	{
		for (const uint t : tv.second)
		{
			TestSet::iterator ti = testSet.find(t);
			if (testSet.end() != ti)
			{
				const uint col = narrow_cast<uint>(std::distance(testSet.begin(),ti));
				coverage.setCellAsCover(row, col);
			}
		}
		++row;
	}
}


	std::pair<bool, McdcIndependencePair> Mcdc::isCompleteTestPairInCoverageResult(McdcIndependencePairSet& mips, CellVectorHeaderSet& cvhs)
	{
		// Test if a complete Test pair is part of the Coverage Result
		bool found = false;
		McdcIndependencePair mipResult;

		McdcType mcdcType[3]{ McdcType::UniqueCause, McdcType::UniqueCauseMasking, McdcType::Masking };
		for (uint mcdcTypeIndex = 0; !found && (mcdcTypeIndex < 3); ++mcdcTypeIndex)
		{
			for (const McdcIndependencePair& mip : mips)
			{
				if (mip.mcdcType != mcdcType[mcdcTypeIndex]) continue;
				const uint v1 = mip.independencePair.first;
				const uint v2 = mip.independencePair.second;
				CellVectorHeaderSet::iterator i1 = std::find_if(cvhs.begin(), cvhs.end(), [v1](const CellVectorHeader& cvh) {return (v1 == cvh.index); });
				if (cvhs.end() != i1)
				{
					CellVectorHeaderSet::iterator i2 = std::find_if(cvhs.begin(), cvhs.end(), [v2](const CellVectorHeader& cvh) {return (v2 == cvh.index); });
					if (cvhs.end() != i2)
					{
						found = true;
						mipResult = mip;
						break;
					}
				}
			}
		}
		return std::make_pair(found, mipResult);
	}





	std::pair<bool, McdcIndependencePair> Mcdc::isOnePartIfTestPairInCoverageResult(McdcIndependencePairSet& mips, CellVectorHeaderSet& cvhs)
	{
		// Test if a complete Test pair is part of the Coverage Result
		bool found = false;
		McdcIndependencePair mipResult;

		McdcType mcdcType[3]{ McdcType::UniqueCause, McdcType::UniqueCauseMasking, McdcType::Masking };
		for (uint mcdcTypeIndex = 0; !found && (mcdcTypeIndex < 3); ++mcdcTypeIndex)
		{
			for (const McdcIndependencePair& mip : mips)
			{
				if (mip.mcdcType != mcdcType[mcdcTypeIndex]) continue;
				const uint v1 = mip.independencePair.first;
				const uint v2 = mip.independencePair.second;
				CellVectorHeaderSet::iterator i1 = std::find_if(cvhs.begin(), cvhs.end(), [v1](const CellVectorHeader& cvh) {return (v1 == cvh.index); });
				if (cvhs.end() != i1)
				{
					found = true;
					mipResult = mip;
					break;
				}
				else
				{
					CellVectorHeaderSet::iterator i2 = std::find_if(cvhs.begin(), cvhs.end(), [v2](const CellVectorHeader& cvh) {return (v2 == cvh.index); });
					if (cvhs.end() != i2)
					{
						found = true;
						mipResult = mip;
						break;
					}
				}
			}
		}
		return std::make_pair(found, mipResult);
	}

void Mcdc::generateTestSets(SymbolTable &symbolTable, MintermVector &mv)
{
	CoverageResult cr;
	std::set<TestSet> allTestSets;

	const ulong notDroppedElements{ coverage.countNotDroppedTableElements() };
	const bool predicateForOutputToFile = (notDroppedElements > 30UL);

	OutStreamSelection outStreamSelection(ProgramOption::pmcsc, predicateForOutputToFile);
	std::ostream& os = outStreamSelection();


	if (isMax1IndependencePairPerCondition())
	{
		cr = coverage.getCoverageResultWithoutReduction();
	}
	else
	{
		coverage.setBestCostFunctionForColumn(std::bind(&Mcdc::compare2TestValues, this, std::placeholders::_1, std::placeholders::_2));
		cr = coverage.reduce(os, true);
	}





	os << "\n\nTest Sets\n\n";



	// Check all Coverage sets
	for (uint i = 0; i < cr.size(); ++i)
	{
		os << "\n-------- For Coverage set " << std::setw(3) << (i + 1) << "    ----------------------------------------------------\n\n";
		bool found = false;
		// Find a test pair for each varaible
		McdcIndependencePair resultingIndependencePair;

		TestSet testSetForOneVariable;

		for (auto &ippc : independencePairPerCondition)
		{

			const auto[ completeTestPairAvailable,  independencePairFull] = isCompleteTestPairInCoverageResult(ippc.second, cr[i]);
			
			if (completeTestPairAvailable)
			{
				resultingIndependencePair = std::move(independencePairFull);
				found = true;
			}
			else
			{
				const auto[partTestPairAvailable, independencePairPart] = isOnePartIfTestPairInCoverageResult(ippc.second, cr[i]);
				
				if (partTestPairAvailable)
				{
					resultingIndependencePair = std::move(independencePairPart);
					found = true;
				}
			}
			if (found)
			{
				testSetForOneVariable.insert(resultingIndependencePair.independencePair.first);
				testSetForOneVariable.insert(resultingIndependencePair.independencePair.second);

				os << "Test Pair for Condition '" << ippc.first << "':  " << std::setw(3) << resultingIndependencePair.independencePair.first
					<< ' ' << std::setw(3) << resultingIndependencePair.independencePair.second << "   (" << mcdcTypeToString(resultingIndependencePair.mcdcType) << ")\n";
			}
		}

		os << "\nResulting Test Vector:  ";
		for (const uint t : testSetForOneVariable)
		{
			os << t << ' ';
		}
		os << "\n\n";

		allTestSets.insert(testSetForOneVariable);


	}
		printResult(allTestSets, os, symbolTable, mv);
		if (!outStreamSelection.hasStdOut())
		{
			printResult(allTestSets, std::cout, symbolTable, mv);
		}


}
void Mcdc::printResult(std::set<TestSet> &allTestSets, std::ostream& os, SymbolTable &symbolTable, MintermVector &mv)
{



	if (1 == allTestSets.size())
	{
		os << "\n\n\n---------------------------------------------------------------------------\nTestvector: Final Result: ";
		for (const uint t : *allTestSets.begin())
		{
			os << t << ' ';
		}
		os << "\n\n";
		for (const uint t : *allTestSets.begin())
		{
			os << std::setw(5) << t << ":  ";
			SymbolType::size_type v = symbolTable.symbol.size() - 1;
			for (const cchar c : symbolTable.symbol)
			{
				os << c << '=' << ((0U !=  (t & bitMask[v])) ? '1' : '0') << "  ";
				--v;
			}
			uint count = narrow_cast<uint>(std::count(mv.begin(), mv.end(), t));
			os << "  (" << count << ")\n";

		}
		os << "\n\n";
	}
	else
	{
		uint min = static_cast<uint>(-1);
		TestSet minTestSet;
		for (const TestSet &ts : allTestSets)
		{
			if (ts.size() < min)
			{
				min = narrow_cast<uint>(ts.size());
				minTestSet = ts;
			}
		}


		os << "\n\n\n---------------------------------------------------------------------------\nTestvector: Recommended Result: ";
		for (const uint t : minTestSet)
		{
			os << t << ' ';
		}
		os << "\n\n";

		for (const uint t : minTestSet)
		{
			os << std::setw(5) <<t << ":  ";
			SymbolType::size_type v = symbolTable.symbol.size() - 1;
			for (const cchar c : symbolTable.symbol)
			{
				os << c << '=' << ((0U != (t & bitMask[v])) ? '1' : '0') << "  ";
				--v;
			}
			uint count = narrow_cast<uint>(std::count(mv.begin(), mv.end(), t)); 
			os << "  (" << count << ")\n";

		}
		os << "\n\n";

	}

}




bool Mcdc::isMax1IndependencePairPerCondition()
{
	bool result = true;
	for (auto &ippc : independencePairPerCondition)
	{
		if (ippc.second.size() > 1)
		{
			result = false;
			break;
		}
	}
	return result;
}

void Mcdc::findMcdcIndependencePairs(VirtualMachineForAST &ast)
{
	{
		const uint maxConditions = ast.maxConditionsInTree();
		const bool predicateForOutputToFile = (maxConditions > 3);
		OutStreamSelection outStreamSelection(ProgramOption::pmastc, predicateForOutputToFile);
		std::ostream& osMcdc = outStreamSelection();
		osMcdc << "\n\n\n\n-------------------------------------------------- Searching for MCDC Test pairs \n\n\n";


		VirtualMachineForAST astOuter{ ast };
		VirtualMachineForAST astInner{ ast };
		VirtualMachineForAST astInfluenceSet{ ast };

		const uint maxLoop = static_cast<uint>(1U << static_cast<uint>(maxConditions));


		sint counter = 0;

		sint counterUniqueCauseMCDC = 0;
		sint counterUniqueCauseMaskingMCDC = 0;
		sint counterMaskingMCDC = 0;
		std::set<uint> testSetUniqueCauseMCDC;
		std::set<uint> testSetUniqueCauseMaskingMCDC;
		std::set<uint> testSetMaskingMCDC;

		const sint astSize = narrow_cast<sint>(astInfluenceSet.ast.size());




	#define PREEVALUATION

	#ifdef PREEVALUATION


		// Preevaluated "inner"

		constexpr sint MaxPreEvaluated = 32768;

		const sint maxPreEvaluated = std::min(MaxPreEvaluated, static_cast<sint>(maxLoop));
		const sint maxPreEvaluatedBegin = static_cast<int>(maxLoop) - maxPreEvaluated + 1;
		std::vector<VirtualMachineForAST> astInnerPreEvaluated;
		astInnerPreEvaluated.reserve(MaxPreEvaluated);



		for (uint i = static_cast<uint>(maxPreEvaluatedBegin); i < maxLoop; ++i)
		{
			astInner.evaluateTree(i);
			astInnerPreEvaluated.push_back(astInner);
		}
	#endif		


		// Brute Force Test all permutations
		for (uint outer = 0; outer < static_cast<uint>(maxLoop - 1); ++outer)
		{
			// Calculate Result for value outer
			astOuter.evaluateTree(outer);

			for (uint inner = outer + 1; inner < static_cast<uint>(maxLoop); inner++)
			{



	#ifdef PREEVALUATION

				AST *astCalculated{ null<AST *>()};
				if (inner >= static_cast<uint>(maxPreEvaluatedBegin))
				{
					astCalculated = &(astInnerPreEvaluated[static_cast<ull>(inner) - static_cast<ull>(maxPreEvaluatedBegin)].ast);
				}
				else
				{
					// Calculate Result for value outer
					astInner.evaluateTree(inner);
					astCalculated = &astInner.ast;
				}

				// Perform TREE XOR. To be able to check, what conditions and what operators changed values
				for (uint i = 0; i < static_cast<uint>(astSize); ++i)
				{
					astInfluenceSet.ast[i].value = (astOuter.ast[i].value != (*astCalculated)[i].value) && !astOuter.ast[i].notEvaluated && !(*astCalculated)[i].notEvaluated;
				}


	#else


				astInner.evaluateTree(inner);
				// Perform TREE XOR. To be able to check, what conditions and what operators changed values
				for (sint i = 0; i < astSize; ++i)
				{
					astInfluenceSet.ast[i].value = (astOuter.ast[i].value != astInner.ast[i].value) && !astOuter.ast[i].notEvaluated && !astInner.ast[i].notEvaluated;
				}


	#endif





				// Now check the influence tree. And determine the type of MCDC (if any) for this test pair
				const auto[mcdcType, influencingCondition] = getMcdcType(astInfluenceSet.ast);


				switch (mcdcType)
				{
				case McdcType::UniqueCause:
					osMcdc << "\n----------------------- Found   Unique Cause          MCDC Test pair for condition: " << influencingCondition << "     Test Pair: " << outer << ' ' << inner << '\n';
					testSetUniqueCauseMCDC.insert(outer);
					testSetUniqueCauseMCDC.insert(inner);
					++counterUniqueCauseMCDC;
					break;
				case McdcType::UniqueCauseMasking:
					osMcdc << "\n----------------------- Found   Unique Cause+Masking  MCDC Test pair for condition: " << influencingCondition << "     Test Pair: " << outer << ' ' << inner << '\n';
					testSetUniqueCauseMaskingMCDC.insert(outer);
					testSetUniqueCauseMaskingMCDC.insert(inner);
					++counterUniqueCauseMaskingMCDC;
					break;
				case McdcType::Masking:
					osMcdc << "\n----------------------- Found   Masking               MCDC Test pair for condition: " << influencingCondition << "     Test Pair: " << outer << ' ' << inner << '\n';
					testSetMaskingMCDC.insert(outer);
					testSetMaskingMCDC.insert(inner);
					++counterMaskingMCDC;
					break;
				case McdcType::NONE: //fallthrough
				default:
					osMcdc << "....................... Could not be identified as MCDC of any type.  Test Pair: " << outer << ' ' << inner << '\n';
					break;
				}



				if (McdcType::NONE != mcdcType)
				{
					add(McdcIndependencePair(mcdcType, outer, inner, influencingCondition));
					if (!outStreamSelection.isNull())
					{
						++counter;
						osMcdc << "\n-------------------------------------- AST for value: " << outer << "\n\n";
						astOuter.printTree(osMcdc);
						osMcdc << "\n-------------------------------------- AST for value: " << inner << "\n\n";
	#ifdef PREEVALUATION
						astInnerPreEvaluated[static_cast<ull>(inner) - static_cast<ull>(maxPreEvaluatedBegin)].printTree(osMcdc);
	#else
						astInner.printTree(osMcdc);
	#endif
						osMcdc << "\n-------------------------------------- AST for influencing condition check\n\n";
						astInfluenceSet.printTree(osMcdc);
						osMcdc << "\n\n";
					}
				}
				else if (!outStreamSelection.isNull() && programOption.option[ProgramOption::paast].optionSelected)
				{
					osMcdc << "\n-------------------------------------- AST for value: " << outer << "\n\n";
					astOuter.printTree(osMcdc);
					osMcdc << "\n-------------------------------------- AST for value: " << inner << "\n\n";
	#ifdef PREEVALUATION
					astInnerPreEvaluated[static_cast<ull>(inner) - static_cast<ull>(maxPreEvaluatedBegin)].printTree(osMcdc);
	#else
					astInner.printTree(osMcdc);
	#endif
					osMcdc << "\n-------------------------------------- AST for influencing condition check\n\n";
					astInfluenceSet.printTree(osMcdc);
					osMcdc << "\n\n";
				}
			}
		}
		if (testSetUniqueCauseMCDC.size() > 0)
		{
			osMcdc << "\n\n----------------------\nUnique Cause MCDC Test Set\n";
			for (auto ts : testSetUniqueCauseMCDC)
			{
				osMcdc << ts << ' ';
			}
			osMcdc << '\n';
		}
		if (testSetUniqueCauseMaskingMCDC.size() > 0)
		{
			osMcdc << "\n\n----------------------\nUnique Cause + Masking MCDC Test Set\n";
			for (auto ts : testSetUniqueCauseMaskingMCDC)
			{
				osMcdc << ts << ' ';
			}
			osMcdc << '\n';
		}
		if (testSetMaskingMCDC.size() > 0)
		{
			osMcdc << "\n\n----------------------\nMasking MCDC Test Set\n";
			for (auto ts : testSetMaskingMCDC)
			{
				osMcdc << ts << ' ';
			}
			osMcdc << '\n';
		}
}
	checkForCoverage();
}






std::pair<McdcType, cchar> Mcdc::getMcdcType(AST& ast)
{
	McdcType resultingMcdcType = McdcType::NONE;
	cchar lastInfluencingSymbol{ ' ' };
	
	sint countOfOneModfiedPerVariable[256];
	std::memset(&countOfOneModfiedPerVariable[0], 0, sizeof(countOfOneModfiedPerVariable));


	// For Masking
	sint modifiedCondition{ 0 };
	sint influencingSymbolCounter{ 0 };

	bool result{ false };
	const uint astSize = narrow_cast<uint> (ast.size());

	for (uint i = 0; i < astSize; ++i)
	{
		const AstNode& astNode = ast[i];
		if ((NumberOfChildren::zero == astNode.numberOfChildren) && astNode.value)
		{
			cchar inputTerminalSymbol = astNode.tokenWithAttribute.inputSymbolLowerCase;
			// count the number of symbols that have value true. 
			// That means the variable has been changed from on test to the other

			++countOfOneModfiedPerVariable[inputTerminalSymbol];

			++modifiedCondition;
			// Check if a variable is influencing
			// That means all true up to the root node
			uint k = i;
			bool check = true;
			do
			{
				check = check && ast[k].value;
				k = ast[k].parentID;
			} while (check && (k != ASTNoLinkedElement));

			if (check)
			{
				++influencingSymbolCounter;
				lastInfluencingSymbol = inputTerminalSymbol;
			}
			result = result || check;
		}

	}
	if (result)
	{
		sint overallModifiedVariables{ 0 };
		for (uint i = 0; i < 256; ++i)
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
			// Unique cause MCDC
			if (1 == modifiedCondition)
			{
				resultingMcdcType = McdcType::UniqueCause;

			}
			// Unique Cause + Masking MCDC
			else if ((1 < modifiedCondition) && (1 < countOfOneModfiedPerVariable[lastInfluencingSymbol]) && (countOfOneModfiedPerVariable[lastInfluencingSymbol] == overallModifiedVariables))
			{
				resultingMcdcType = McdcType::UniqueCauseMasking;
			}
			// Masking
			else if (1 < modifiedCondition)
			{
				resultingMcdcType = McdcType::Masking;
			}
		}
	}
	return std::make_pair(resultingMcdcType, lastInfluencingSymbol);

}

