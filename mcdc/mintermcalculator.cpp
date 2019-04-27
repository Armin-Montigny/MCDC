#include "pch.h"
#include "mintermcalculater.hpp"
#include "cloption.hpp"


#include <thread>
#include <future>
#include <utility>
#include <array>
#include <chrono>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <iterator>

MintermCalculator::MintermCalculator(std::string &booleanExpression) : objectCode(), compiler(booleanExpression, objectCode), vmfbe(), source(booleanExpression)
{
}



MintermCalculator::MintermRangeVector MintermCalculator::calculateMintermRanges(uint numberOfRanges, uint upperBound)
{
	MintermRangeVector mrv;
	if (upperBound < numberOfRanges)
	{
		mrv.emplace_back(std::move(MintermRange(0, upperBound)));
	}
	else
	{
		const uint rangeValue = upperBound / numberOfRanges;
		uint startValue = 0;
		for (uint ui = 0; ui < numberOfRanges; ++ui)
		{
			uint endValue = startValue + rangeValue;
			if (endValue > (upperBound))
			{
				endValue = upperBound;
			}
			mrv.emplace_back(MintermRange(startValue, endValue));
			startValue = endValue;
		}
	}
	return mrv;
}

MintermVector MintermCalculator::calculate()
{
	MintermVector mintermVector;

	
	if (!compilerResultOK)
	{
		compilerResultOK = compiler();
	}
	if (compilerResultOK)
	{
		numberOfBooleanVariables = objectCode.symbolTable.numberOfSymbols();
		const uint maxEvaluations = 1U << numberOfBooleanVariables;

		if (numberOfBooleanVariables > 10)
		{
			std::future<MintermVector> futures[numberOfThreads];
			MintermRangeVector mrv = calculateMintermRanges(numberOfThreads, maxEvaluations);

			for (uint i = 0; i < numberOfThreads; ++i)
			{
				futures[i] = std::async(std::launch::async, &MintermCalculator::calculateAsThread, this, mrv[i], objectCode);
			}
			for (uint i = 0; i < numberOfThreads; ++i)
			{
				MintermVector mvTemp = futures[i].get();
				mintermVector.insert(mintermVector.end(), std::make_move_iterator(mvTemp.begin()), std::make_move_iterator(mvTemp.end()));
			}
		}
		else
		{
			vmfbe.load(objectCode, source);
			for (uint ui = 0; ui < maxEvaluations; ++ui)
			{
				if (vmfbe.run(ui))
				{
					mintermVector.push_back(ui);
				}
			}
		}
	}
	return mintermVector;
}



MintermVector MintermCalculator::calculateAsThread(MintermRange mt, ObjectCode objectCodeParameter)
{
	MintermVector mv;

	VirtualMachineForBooleanExpressions vm;
	vm.load(objectCodeParameter, source);

	for (uint ui = mt.first; ui < mt.second; ++ui)
	{
		if (vm.run(ui))
		{
			mv.push_back(ui);
		}
	}
	return mv;
}


void printTruthTable(const std::string& source, MintermVector& mintermVector, const SymbolTable &symbolTable)
{
	const uint symbolCount = symbolTable.numberOfSymbols();
	const uint32 maxEvaluations = bitMask[symbolCount];
	const MinTermType maxLengthDecimalEquivalent = static_cast<uint>(std::log10(maxEvaluations)) + 1;

	const bool predicateForOutputToFile = (symbolCount > 4);
	OutStreamSelection outStreamSelection(ProgramOption::pttc, predicateForOutputToFile) ;
	std::ostream& os = outStreamSelection();

	os << "------------------ Truth table for boolean expression:\n\n'"<<source<<"'\n\n";

	for (uint i = 0; i < (1 + maxLengthDecimalEquivalent + 1); ++i) { os << ' '; }
	for (const cchar c : symbolTable.symbol) { os << c; }
	os << '\n';

	for (uint i = 0; i < (1 + maxLengthDecimalEquivalent + 1 + symbolCount + 2); ++i) { os << '-'; }
	os << "+--\n";

	std::bitset<26> b;


	for (uint i = 0; i < maxEvaluations; ++i)
	{
		b = i;

		uint result = (std::find(mintermVector.begin(), mintermVector.end(), i) != mintermVector.end()) ? 1U : 0U;
		os << std::setw(static_cast<std::streamsize>(maxLengthDecimalEquivalent) + 1) << i << " " << b.to_string().substr(26 - symbolCount) << "    " << result << '\n';
	}
	os << "\n\n";

}