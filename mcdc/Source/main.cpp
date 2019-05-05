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


#include "cloption.hpp"
#include "quinemccluskey.hpp"
#include "mcdc.hpp"

#include <iostream>
#include <assert.h>

void showHelp();

sint main(const sint argc, const cchar* const argv[])
{
    static_assert(sizeof(uint)==4, "This programm needs a 4 byte integer type"); // compile-time error if not

    // Read the command line options and evaluate them
    programOption.readOptions(argc, argv);
    programOption.evaluateCommandLine();

    // If the program is called without any parameters, then help is shown
    // The minimum necessary is that a boolean source expression is given
    // Can also be read interactively
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
    // Now we have a boolean expression in variable source


	// The number of variables/conditions/symbols/literals in the source string
    NumberOfBitsCountType numberOfVariables{ 0 };
    bool booleanExpresionsValid{ true };	// We expect the boolean expression to be true

	// All symbols will be stored in the symbol table by the compiler as a part of the object code
    SymbolTable symbolTable;

	// After evaluationg the boolean expression with all possible source values, we will receive a list of minterms
    MintermVector mv;
    MintermCalculator mc(source);

	// Compile the source code boolean expression 
	const bool termIsOk{ mc.runCompiler() };

	// CHeck result of compiler. Maybe syntax error
    if (!termIsOk)
    {
        std::cout << "\n\n*** Syntax Error in boolean expression\n\n";
        booleanExpresionsValid = false;
    }
    else
    {
		// Everything is OK
		// Get the symbol table and the number of variables/conditions
        symbolTable = mc.getSymbolTable();
        numberOfVariables = symbolTable.numberOfSymbols();

		// Because increasing the number of variables makes the program geometrically slow, we
		// limit the number of input varaiables
        if (numberOfVariables > MaxNumberOfBitsForEvaluation)
        {
            std::cerr << "\n\n*** Error: Too many boolean variables for this program version (" << narrow_cast<uint>(numberOfVariables) << "). Max is: " << MaxNumberOfBitsForEvaluation << "\n\n";
            booleanExpresionsValid = false;
        }

        if (booleanExpresionsValid)
        {
			// So, the boolean expression was valid
			// Now calculate the vector with the minterms (slow)
            mv = mc.calculate();

            if (null<MintermVector::size_type>() == mv.size())
            {
				// If there are no minterms, the boolean expressions evaluates always to false
				// This is called a contradiction
                std::cerr << "\n\n*** Contradiction. Boolean expression '" << source << "' is always false\n\n";
                booleanExpresionsValid = false;
            }
            if (narrow_cast<MintermVector::size_type>((narrow_cast <MintermVector::size_type>(1) << mc.getNumberOfBooleanVariables())) == mv.size())
            {
				// If there are minterms for every inpiut value, the boolean expressions evaluates always to true
				// This is called a tautology
				std::cerr << "\n\n*** Tautology. Boolean expression '" << source << "' is always true\n\n";
                booleanExpresionsValid = false;
            }
        }
    }


	// We checked the validity
    if (booleanExpresionsValid)
    {

        std::cout << "\n\nStart to evaluate boolean expression\n\n'" << source << "'\n\nNumber of Variables : " << narrow_cast<uint>(numberOfVariables) << "   Calculated number of minterms : " << mv.size() << "\n\n\n";
        printTruthTable(source, mv, symbolTable);

		// Start the Quine & McCluskey Algorithm and get aminimum DNF
        QuineMcluskey quineMcluskey;
		std::string minimizedSource{ quineMcluskey.getMinimumDisjunctiveNormalForm(mv, symbolTable, source) };
        std::cout << "\n\nResult of Quine and McCluskey minimizing algorithm:\n\n'" << minimizedSource << "'\n\n";

		// We can continue to do the MCDC analysis with the original source code
		// This may result in unnecessary large Abstract Syntax Trees
		// Therefore there is an option to continue with the just calculated minimum DNF
        if (programOption.option[ProgramOption::umdnf].optionSelected)
        {
			// In  that case, reassign the just found minimum DNF to the source code string
            source = minimizedSource;
            std::cout << "Continue Abstract Syntax Tree Analyis with minimized boolean expression\n\n'" << source << "'\n\n";
        }
        else
        {
            std::cout << "Continue Abstract Syntax Tree Analyis with original boolean expression\n\n'" << source << "'\n\n";
        }



		// We and to create the AST
        VirtualMachineForAST virtualMachineForAST;
        
		// Compile AST
        CompilerForAST compilerForAST(source, virtualMachineForAST);
		const bool compilerResult{ compilerForAST() };
        
        if (compilerResult)
        {
			// If that was OK. Show tree
            virtualMachineForAST.printTreeStandard(source);

			// If selected, find MCDC test pairs and test set
            if (!programOption.option[ProgramOption::nomcdc].optionSelected)
            {
                Mcdc mcdc;

                mcdc.findMcdcIndependencePairs(virtualMachineForAST);
                mcdc.generateTestSets();
            }
        }
    }
    return 0;
}




void showHelp()
{
	std::cout << "Help\n\n\n";
	std::cout << " This program must beinvoked with comand line options\n";
	std::cout << "\n";
	std::cout << "Call with: mcdc [-options]\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " There is also the possibility to specify options in a textfile\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " Options control the behaviour of the porgram. \n";
	std::cout << " Mainly Output options are available\n";
	std::cout << "\n";
	std::cout << " All output options are arganized in groups.\n";
	std::cout << " There is the main option, which specifies the area of data that should be shown\n";
	std::cout << " The suboptions, denoted by a suffix, determine, where the data go to\n";
	std::cout << "\n";
	std::cout << " Suffixes and their meaning:\n";
	std::cout << "\n";
	std::cout << " c          Output to console \n";
	std::cout << " f          Output to a given filename \n";
	std::cout << " a          Output to a file with automatically selected name\n";
	std::cout << " fauto      In case of big data, so many lines to print, output goes to a file with given name. Else to console\n";
	std::cout << " fautoa     In case of big data, so many lines to print, output goes to a file with automatically selected name. Else to console\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " All Options are listed below.\n";
	std::cout << "\n";
	std::cout << " ---------------------------------------------------------------\n";
	std::cout << " 1. General Options\n";
	std::cout << "\n";
	std::cout << " -s \"boolean expression\"      Define boolean expression to evaluate\n";
	std::cout << " -sa                          Ask for boolean expression in program\n";
	std::cout << " -umdnf                       Use minimum DNF resulting from QuineMcLuskey Reduction for MCDC calculation\n";
	std::cout << " -bse                         Use boolean short cut evaluation in abstract syntax trees\n";
	std::cout << " -opt \"filename\"              Use file \"filename\" to read options\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " ---------------------------------------------------------------\n";
	std::cout << " 2. File Handling related Options\n";
	std::cout << "\n";
	std::cout << " -fd \"directory\"               Base directory, where all resulting files will be written to.  Default is current directory\n";
	std::cout << " -fp \"prefix\"                  Prefix which will be prepended befor all filenames. Default ist no prefix\n";
	std::cout << " -fo                           Overwrite all files with new data. Default, priority over -fa\n";
	std::cout << " -fa                           Append new files to existing data. Will be overwritten by -fo\n";
	std::cout << " -fafwsfn                      Append files with same given filenames. If for the below files a double name is given, then the data will go into one file. in the case of -fo, the initial file is overwritten.\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " ---------------------------------------------------------------\n";
	std::cout << " 3. Output controlling Options\n";
	std::cout << " -paast                        Print all abstract syntax trees in MCDC test. Not only those with positive MCDC outcome\n";
	std::cout << " -ppst                         Print parsestack to console\n";
	std::cout << " -sfqmt                        Show full Quine&  McCluskey Reduction Table (will be a bit slower)\n";
	std::cout << " -nomcdc                       Do Not calculate MCDC test vectors\n";
	std::cout << " -dnpast                       Do not print ASTs while searching for test pairs\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.1 Definition for all other output Options\n";
	std::cout << "\n";
	std::cout << " -pallc                        Write all Data to console\n";
	std::cout << " -pallf \"filename\"             Write all Data to file \"filename\"\n";
	std::cout << " -palla                        Write all Data to file with automatic created filename\n";
	std::cout << " -pallfauto \"filename\"         Automatically write big data with lots of lines to file \"filename\", the rest to console\n";
	std::cout << " -pallfautoa                   Automatically write big data with lots of lines to file with automatic created filename, the rest to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.2 Object code dump\n";
	std::cout << "\n";
	std::cout << " -pcoc                         Print compiled object code to console\n";
	std::cout << " -pcof \"filename\"              Print compiled object code to file \"filename\"\n";
	std::cout << " -pcofa                        Print compiled object code to file with automatic created filename\n";
	std::cout << " -pcofauto \"filename\"          Automatically write big compiled object code to file \"filename\", write small data to console\n";
	std::cout << " -pcofautoa                    Automatically write big compiled object code to file with automatic created filename, write small data to console. Default\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.3 Truth table output\n";
	std::cout << "\n";
	std::cout << " -pttc                         Print truth table to console\n";
	std::cout << " -pttf \"filename\"              Print truth table to file \"filename\"\n";
	std::cout << " -pttfa                        Print truth table to file with automatic created filename\n";
	std::cout << " -pttfauto \"filename\"          Automatically write big truth table to file \"filename\", write small data to console\n";
	std::cout << " -pttfautoa                    Automatically write big truth table to file with automatic created filename, write small data to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.4 Show Quine Mccluskey reduction table\n";
	std::cout << "\n";
	std::cout << " -pqmtc                        Print Quine Mcluskey Reduction Tables to console                 \n";
	std::cout << " -pqmtf \"filename\"             Print Quine Mcluskey Reduction Tables to file \"filename\"\n";
	std::cout << " -pqmtfa                       Print Quine Mcluskey Reduction Tables to file with automatic created filename\n";
	std::cout << " -pqmtfauto \"filename\"         Automatically write big Quine Mcluskey Reduction Tables to file \"filename\", write samll data to console\n";
	std::cout << " -pqmtfautoa                   Automatically write big Quine Mcluskey Reduction Tables to file with automatic created filename, write samll data to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.5 Show Prime Impicant reduction table\n";
	std::cout << "\n";
	std::cout << " -ppirtc                       Print Prime Implicant Reduction Table to console                 \n";
	std::cout << " -ppirtf \"filename\"            Print Prime Implicant Reduction Table to file \"filename\"\n";
	std::cout << " -ppirtfa                      Print Prime Implicant Reduction Table to file with automatic created filename\n";
	std::cout << " -ppirtfauto \"filename\"        Automatically write big Prime Implicant Reduction Table to file \"filename\", write small data to console\n";
	std::cout << " -ppirtfautoa                  Automatically write big Prime Implicant Reduction Table to file file with automatic created filename, write small data to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.6 Show Abstract Syntax Tree\n";
	std::cout << "\n";
	std::cout << " -pastc                        Print Abtract Syntax Tree to console                 \n";
	std::cout << " -pastf \"filename\"             Print Abtract Syntax Tree to file \"filename\"                 \n";
	std::cout << " -pasta                        Print Abtract Syntax Tree to file with automatic created filename                 \n";
	std::cout << " -pastfauto \"filename\"         Automatically write big Abtract Syntax Tree to file \"filename\", write small data to console                \n";
	std::cout << " -pastfautoa                   Automatically write big Abtract Syntax Tree to file with automatic created filename, write small data to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.7 Show Abstract Syntax Tree for MC/DC relevant values\n";
	std::cout << "\n";
	std::cout << " -pmastc                       Print only MCDC relevant abtract syntac trees during brute force evaluation to console\n";
	std::cout << " -pmastf \"filename\"            Print only MCDC relevant abtract syntac trees during brute force evaluation to file \"filename\"\n";
	std::cout << " -pmasta                       Print only MCDC relevant abtract syntac trees during brute force evaluation to file with automatic created filename                 \n";
	std::cout << " -pmastfauto \"filename\"        Automatically write big data for all MCDC relevant abtract syntac trees to file \"filename\", write small data to console                    \n";
	std::cout << " -pmastfautoa                  Automatically write big data for all MCDC relevant abtract syntac trees to file with automatic created filename, write small data to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.8 Show Resulting MC/DC test pairs\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -pmtpc                        Print all MC/DC testpairs to console\n";
	std::cout << " -pmtpf \"filename\"             Print all MC/DC testpairs to file \"filename\"\n";
	std::cout << " -pmtpa                        Print all MC/DC testpairs to file with automatic created filename\n";
	std::cout << " -pmtpfauto \"filename\"         Automatically write big data for all MC/DC testpairs to file \"filename\", write small data to console\n";
	std::cout << " -pmtpfautoa                   Automatically write big data for all MC/DC testpairs to file with automatic created filename, write small data to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " -------------------------------------------\n";
	std::cout << " 3.9 Show Resulting test coverage sets\n";
	std::cout << "\n";
	std::cout << " -pmcsc                        Print all MC/DC coverage sets to console\n";
	std::cout << " -pmcsf \"filename\"             Print all MC/DC coverage sets to file filename\n";
	std::cout << " -pmcsa                        Print all MC/DC coverage sets to console file with automatic created filename\n";
	std::cout << " -pmcsfauto \"filename\"         Automatically write big data for all MC/DC coverage sets to file \"filename\", write small data to console\n";
	std::cout << " -pmcsfautoa                   Automatically write big data for all MC/DC coverage sets to file with automatic created filename, write small data to console. Default\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " Any 3.7 option switches on the respective 3.8 options\n";
	std::cout << "\n";
	std::cout << "\n";
	std::cout << " if an explicit filename is given, it will always be used in favour auf a automatic generated file name.\n";
	std::cout << " Means, if \"f\" and \"a\" suffix are both present, suffix \"f\" will be used.\n";
	std::cout << "\n";
	std::cout << " if -pall someting is given and other output options are present, then the other output options win.\n";
	std::cout << "\n";
	std::cout << " In case of double options, the last will win\n";
	std::cout << "\n";

}


