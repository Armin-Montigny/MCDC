#include "pch.h"


#include "compiler.hpp"
#include "virtualmachine.hpp"
#include "mintermcalculater.hpp"
#include "quinemccluskey.hpp"

#include "petrick.hpp"
#include "mcdc.hpp"
#include "coverage.hpp"
#include "ast.hpp"
#include "cloption.hpp"



#include <bitset>
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <cctype>
#include <fstream>
#include <assert.h>

void showHelp()
{
	std::cout << "Help\n\n";
}



sint main(const sint argc, cchar *argv[])
{
	static_assert(sizeof(uint)>=4,	"This programm needs a 4 byte integer type"); // compile-time error if not

	programOption.readOptions(argc, argv);
	programOption.evaluateCommandLine();


	std::string source;
	if (programOption.option[ProgramOption::sa].optionSelected)
	{
		std::cout << "Please enter boolean expression:\n";
		std::getline(std::cin, source);
	}
	else if (programOption.option[ProgramOption::s].optionSelected)
	{
		source = programOption.option[0].optionParameterString;
	}
	else
	{
		showHelp();
		return 0;
	}




	NumberOfBitsCountType numberOfVariables{ 0 };
	bool booleanExpresionsValid{ true };

	SymbolTable symbolTable;

	MintermVector mv;
	MintermCalculator mc(source);

	const bool termIsOk = mc.runCompiler();
	if (!termIsOk)
	{
		std::cout << "\n\n*** Syntax Error in boolean expression\n\n";
		booleanExpresionsValid = false;
	}
	else
	{
		symbolTable = mc.getSymbolTable();
		numberOfVariables = symbolTable.numberOfSymbols();

		if (numberOfVariables > MaxNumberOfBitsForEvaluation)
		{
			std::cerr << "\n\n*** Error: Too many boolean variables for this program version (" << narrow_cast<uint>(numberOfVariables) << "). Max is: " << MaxNumberOfBitsForEvaluation << "\n\n";
			booleanExpresionsValid = false;
		}

		if (booleanExpresionsValid)
		{
			mv = mc.calculate();
			if (null<MintermVector::size_type>() == mv.size())
			{
				std::cerr << "\n\n*** Contradiction. Boolean expression '" << source << "' is always false\n\n";
				booleanExpresionsValid = false;
			}
			if (narrow_cast<MintermVector::size_type>((narrow_cast <MintermVector::size_type>(1) << mc.getNumberOfBooleanVariables())) == mv.size())
			{
				std::cerr << "\n\n*** Tautology. Boolean expression '" << source << "' is always true\n\n";
				booleanExpresionsValid = false;
			}
		}
	}



	if (booleanExpresionsValid)
	{

		std::cout << "\n\nStart to evaluate boolean expression\n\n'" << source << "'\n\nNumber of Variables : " << narrow_cast<uint>(numberOfVariables) << "   Calculated number of minterms : " << mv.size() << "\n\n\n";
		printTruthTable(source, mv, symbolTable);


		QuineMcluskey quineMcluskey;
		std::string minimizedSource = quineMcluskey.getMinimumDisjunctiveNormalForm(mv, symbolTable, source);


		std::cout << "\n\nResult of Quine and McCluskey minimizing algorithm:\n\n'" << minimizedSource << "'\n\n";
		if (programOption.option[ProgramOption::umdnf].optionSelected)
		{
			source = minimizedSource;
			std::cout << "Continue Abstract Syntax Tree Analyis with minimized boolean expression\n\n'" << source << "'\n\n";
		}
		else
		{
			std::cout << "Continue Abstract Syntax Tree Analyis with original boolean expression\n\n'" << source << "'\n\n";
		}


		VirtualMachineForAST virtualMachineForAST;
		
		CompilerForAST compilerForAST(source, virtualMachineForAST);
		const bool compilerResult = compilerForAST();
		
		if (compilerResult)
		{
			virtualMachineForAST.printTreeStandard(source);

			if (!programOption.option[ProgramOption::nomcdc].optionSelected)
			{
				Mcdc mcdc;

				mcdc.findMcdcIndependencePairs(virtualMachineForAST);
				mcdc.generateTestSets(virtualMachineForAST.symbolTable, mv);
			}
		}
	}
	return 0;
}


