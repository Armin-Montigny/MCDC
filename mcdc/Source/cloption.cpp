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



// This program must beinvoked with comand line options
//
// There is also the possibility to specify options in a textfile
//
// This functionality is implemented with below defined classes and functions


//
// Options control the behaviour of the porgram. 
// Mainly Output options are available

// All output options are arganized in groups.
// There is the main option, which specifies the area of data that should be shown
// The suboptions, denoted by a suffix, determine, where the data go to
//
// Suffixes and their meaning:
//
// c		Output to console 
// f		Output to a given filename 
// a		Output to a file with automatically selected name
// fauto	In case of big data, so many lines to print, output goes to a file with given name. Else to console
// fautoa	In case of big data, so many lines to print, output goes to a file with automatically selected name. Else to console


// All Options are listed below.

// ---------------------------------------------------------------
// 1. General Options

// -s "boolean expression"	Define boolean expression to evaluate
// -sa						Ask for boolean expression in program
// -umdnf					Use minimum DNF resulting from QuineMcLuskey Reduction for MCDC calculation
// -bse						Use boolean short cut evaluation in abstract syntax trees
// -opt "filename"			Use file "filename" to read options


// ---------------------------------------------------------------
// 2. File Handling related Options

// -fd "directory"			Base directory, where all resulting files will be written to.  Default is current directory
// -fp "prefix"				Prefix which will be prepended befor all filenames. Default ist no prefix
// -fo						Overwrite all files with new data. Default, priority over -fa
// -fa						Append new files to existing data. Will be overwritten by -fo
// -fafwsfn					Append files with same given filenames. If for the below files a double name is given, then the data will go into one file. in the case of -fo, the initial file is overwritten.


// ---------------------------------------------------------------
// 3. Output controlling Options
// -paast					Print all abstract syntax trees in MCDC test. Not only those with positive MCDC outcome
// -ppst					Print parsestack to console
// -sfqmt					Show full Quine&  McCluskey Reduction Table (will be a bit slower)
// -nomcdc					Do Not calculate MCDC test vectors
// -dnpast					Do not print ASTs while searching for test pairs

// -------------------------------------------
// 3.1 Definition for all other output Options

// -pallc					Write all Data to console
// -pallf "filename"		Write all Data to file "filename"
// -palla"					Write all Data to file with automatic created filename
// -pallfauto "filename"	Automatically write big data with lots of lines to file "filename", the rest to console
// -pallfautoa				Automatically write big data with lots of lines to file with automatic created filename, the rest to console. Default


// -------------------------------------------
// 3.2 Object code dump

// -pcoc					Print compiled object code to console
// -pcof "filename"			Print compiled object code to file "filename"
// -pcofa					Print compiled object code to file with automatic created filename
// -pcofauto "filename"		Automatically write big compiled object code to file "filename", write small data to console
// -pcofautoa 				Automatically write big compiled object code to file with automatic created filename, write small data to console. Default

// -------------------------------------------
// 3.3 Truth table output

// -pttc					Print truth table to console
// -pttf "filename"			Print truth table to file "filename"
// -pttfa					Print truth table to file with automatic created filename
// -pttfauto "filename"		Automatically write big truth table to file "filename", write small data to console
// -pttfautoa				Automatically write big truth table to file with automatic created filename, write small data to console. Default



// -------------------------------------------
// 3.4 Show Quine Mccluskey reduction table

// -pqmtc					Print Quine Mcluskey Reduction Tables to console 				
// -pqmtf "filename"		Print Quine Mcluskey Reduction Tables to file "filename"
// -pqmtfa					Print Quine Mcluskey Reduction Tables to file with automatic created filename
// -pqmtfauto "filename"	Automatically write big Quine Mcluskey Reduction Tables to file "filename", write samll data to console
// -pqmtfautoa				Automatically write big Quine Mcluskey Reduction Tables to file with automatic created filename, write samll data to console. Default


// -------------------------------------------
// 3.5 Show Prime Impicant reduction table

// -ppirtc					Print Prime Implicant Reduction Table to console 				
// -ppirtf "filename"		Print Prime Implicant Reduction Table to file "filename"
// -ppirtfa					Print Prime Implicant Reduction Table to file with automatic created filename
// -ppirtfauto "filename"	Automatically write big Prime Implicant Reduction Table to file "filename", write small data to console
// -ppirtfautoa				Automatically write big Prime Implicant Reduction Table to file file with automatic created filename, write small data to console. Default


// -------------------------------------------
// 3.6 Show Abstract Syntax Tree

// -pastc					Print Abtract Syntax Tree to console 				
// -pastf "filename"		Print Abtract Syntax Tree to file "filename" 				
// -pasta					Print Abtract Syntax Tree to file with automatic created filename 				
// -pastfauto "filename"	Automatically write big Abtract Syntax Tree to file "filename", write small data to console				
// -pastfautoa				Automatically write big Abtract Syntax Tree to file with automatic created filename, write small data to console. Default



// -------------------------------------------
// 3.7 Show Abstract Syntax Tree for MC/DC relevant values

// -pmastc					Print only MCDC relevant abtract syntac trees during brute force evaluation to console
// -pmastf "filename"		Print only MCDC relevant abtract syntac trees during brute force evaluation to file "filename"
// -pmasta					Print only MCDC relevant abtract syntac trees during brute force evaluation to file with automatic created filename 				
// -pmastfauto "filename"	Automatically write big data for all MCDC relevant abtract syntac trees to file "filename", write small data to console					
// -pmastfautoa				Automatically write big data for all MCDC relevant abtract syntac trees to file with automatic created filename, write small data to console. Default


// -------------------------------------------
// 3.8 Show Resulting MC/DC test pairs


// -pmtpc					Print all MC/DC testpairs to console
// -pmtpf "filename"		Print all MC/DC testpairs to file "filename"
// -pmtpa					Print all MC/DC testpairs to file with automatic created filename
// -pmtpfauto "filename"	Automatically write big data for all MC/DC testpairs to file "filename", write small data to console
// -pmtpfautoa				Automatically writ         e big data for all MC/DC testpairs to file with automatic created filename, write small data to console. Default


// -------------------------------------------
// 3.9 Show Resulting test coverage sets

// -pmcsc					Print all MC/DC coverage sets to console
// -pmcsf "filename"		Print all MC/DC coverage sets to file filename
// -pmcsa					Print all MC/DC coverage sets to console file with automatic created filename
// -pmcsfauto "filename"	Automatically write big data for all MC/DC coverage sets to file "filename", write small data to console
// -pmcsfautoa				Automatically write big data for all MC/DC coverage sets to file with automatic created filename, write small data to console. Default


// Any 3.7 option switches on the respective 3.8 options

//
// if an explicit filename is given, it will always be used in favour auf a automatic generated file name.
// Means, if "f" and "a" suffix are both present, suffix "f" will be used.

// if -pall someting is given and other output options are present, then the other output options win.

// In case of double options, the last will win





#include "cloption.hpp"
#include <cctype>
#include <filesystem>


// A major part of this program deals with sending output to the screen or to files
// This is handled with a customized iostream.
// We will use especially a T-stream. Inserting something into this stream will lead to a 
// output to cout and or one or 2 files (or nothing at all)

namespace OutputHandling
{
	// Custom streambuffer main output function
	// WIll send tth inout data to up to 3 output streams
	sint TeeStream::TeeStreamBuffer::overflow(int_type c)
	{
		// Short cut name
		using traits = std::streambuf::traits_type;
		// Function return code. True means success
		bool rc{ true };
		
		// If character to output is not EOF
		if (!traits::eq_int_type(traits::eof(), c))
		{
			// Put into streambuffer one
			if (traits::eq_int_type(narrow_cast<int_type>(this->streamBufferPath1->sputc(narrow_cast<char_type>(c))),traits::eof()))
			{
				rc = false;
			}
			// Put into streambuffer two
			if (traits::eq_int_type(narrow_cast<int_type>(this->streamBufferPath2->sputc(narrow_cast<char_type>(c))), traits::eof()))
			{
				rc = false;
			}
			// And if streambuffer tree is existing, then put it there as well
			if (null<std::streambuf*>() != streamBufferPath3)
			{
				if (traits::eq_int_type(narrow_cast<int_type>(this->streamBufferPath3->sputc(narrow_cast<char_type>(c))), traits::eof()))
				{
					rc = false;
				}
			}
		}
		// Depending on outcoume of above functions, return NOT EOF (OK) or EOF (Error or done)
		return rc ? traits::not_eof(c) : traits::eof();
	}

	// The sync function is used to sync internal buffer structures with external devices like HDDs
	// For or internally used T we will send the sync request to all streams
	sint  TeeStream::TeeStreamBuffer::sync()
	{
		// Function returns 0 on success and -1 on failure
		// Return code of all functions. If any functions is not successfull, the whole function will return error
		bool rc{ true }; // Assume OK
		
		// Call pubsync for all 3 possible streams
		if (-1 == this->streamBufferPath1->pubsync())
		{
			rc = false;
		}
		if (-1 == this->streamBufferPath2->pubsync())
		{
			rc = false;
		}
		if (null<std::streambuf*>() != streamBufferPath3)
		{
			if (-1 == this->streamBufferPath3->pubsync())
			{
				rc = false;
			}
		}
		// If there was any error, return error (-1)
		return rc ? 0 : -1;
	}



	
	// Simple initialisation functions for setting the streambuffer for a stream
	// Additionally, if a target is a file, the file will be opened
	// Always versions with filename as "std::string" and "const char*"
	
	// Append Mode is the file open mode. So, either "overwrite" or "append"
	
	// Cout + One file. Filename as std::string
	void TeeStream::setFileAndMode(const std::string& filename, const bool appendMode)
	{
		if (!fileOut1.is_open())
		{
			fileOut1.open(filename, (appendMode ? IosModeAppend : IosModeOverwrite));
		}
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf());
	}
	
	// Cout + 2 files. Filename as std::string
	void TeeStream::setFileAndMode(const std::string& filename1, const bool appendMode1, const std::string& filename2, const bool appendMode2)
	{
		if (!fileOut1.is_open())
		{
			fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		}
		if (!fileOut2.is_open())
		{
			fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
		}
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf(), fileOut2.rdbuf());
	}

	// Cout + One file. Filename as const char *
	void TeeStream::setFileAndMode(const cchar* filename, const bool appendMode)
	{
		if (!fileOut1.is_open())
		{
			fileOut1.open(filename, (appendMode ? IosModeAppend : IosModeOverwrite));
		}
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf());
	}

	// Cout + 2 files. Filename as const char *
	void TeeStream::setFileAndMode(const cchar* filename1, bool appendMode1, const cchar* filename2, bool appendMode2)
	{
		if (!fileOut1.is_open())
		{
			fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		}
		if (!fileOut2.is_open())
		{
			fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
		}
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf(), fileOut2.rdbuf());
	}
	
	// 2 files. Filename as std::string
	void TeeStream::setFileAndMode2FilesNoCout(const std::string& filename1, const bool appendMode1, const std::string& filename2, const bool appendMode2)
	{
		if (!fileOut1.is_open())
		{
			fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		}
		if (!fileOut2.is_open())
		{
			fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
		}
		teeStreamBuffer.setStreamBuffer(fileOut1.rdbuf(), fileOut2.rdbuf());
	}

	// 2 files. Filename as const char *
	void TeeStream::setFileAndMode2FilesNoCout(const cchar* filename1, const bool appendMode1, const cchar* filename2, const bool appendMode2)
	{
		if (!fileOut1.is_open())
		{
			fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		}
		if (!fileOut2.is_open())
		{
			fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
		}
		teeStreamBuffer.setStreamBuffer(fileOut1.rdbuf(), fileOut2.rdbuf());
	}

}




// This function will setup an ostream, depending on program command line options
// As described above, their are groups of output options
// Within the group, it can be selected, where the output should go to

// This is including flages, with an automated selection, if small output should go
// to standard out or to one or more files.
// This shall help to reduce too muach output to std::cout.
// A predicate (a boolean parameter) is used for that purpose
OutStreamSelection::OutStreamSelection(uint optionFirstIndexInGroupU, const bool predicateForOutputToFile) : selectedOstream(nullptr), teeStream(), nullStream(), fileOnlyOutput1(), fileOnlyOutput2()
{
	// May not be needed, but will be defined anyway.
	// We can output to anything of cout, File1, file2 or nothing
	std::string outputFileName1;
	std::string outputFileName2;
	// File open mode. Append or Overwrite
	bool modeAppend1{ false };
	bool modeAppend2{ false };
	
	// For the selection of the output stream we will use bitwise encoding// so that we can select 
	// various options in one variable
	selection = 0 ;

	// This is only a acst to unsigned long long
	const uint optionFirstIndexInGroup{ optionFirstIndexInGroupU };

	if (programOption.option[optionFirstIndexInGroup].optionSelected)
	{
		// Output to Standard out
		selection = selection | OutputDestinationCOut;
	}
	
	
	// There will be only one of the next 2 option valid
	// Either output to a file with a given name or
	// output to a file with a predefined name
	if (programOption.option[optionFirstIndexInGroup + 1].optionSelected)
	{
		// Output to file
		outputFileName1 = programOption.option[optionFirstIndexInGroup + 1].optionParameterString;
		modeAppend1 = programOption.option[optionFirstIndexInGroup + 1].fileAppend;
		selection = selection | OutputDestinationFile;

	}
	if (programOption.option[optionFirstIndexInGroup + 2].optionSelected)
	{
		// Output to file
		outputFileName1 = programOption.option[optionFirstIndexInGroup + 2].optionParameterString;
		modeAppend1 = programOption.option[optionFirstIndexInGroup + 2].fileAppend;
		selection = selection | OutputDestinationFile;
	}


	// There will be only one of the next 2 option valid
	// Depending on the complexity of the data to output
	// Either output to a file with a given name or
	// output to a file with a predefined name
	if (programOption.option[optionFirstIndexInGroup + 3].optionSelected)
	{
		// Conditional Output to file
		if (predicateForOutputToFile)
		{
			outputFileName2 = programOption.option[optionFirstIndexInGroup + 3].optionParameterString;
			modeAppend2 = programOption.option[optionFirstIndexInGroup + 3].fileAppend;
			selection = selection | OutputDestinationOptionalFile;
		}
		else
		{
			selection = selection | OutputDestinationCOut;
		}
	}
	if (programOption.option[optionFirstIndexInGroup + 4].optionSelected)
	{
		// Conditional Output to file
		// Conditional Output to file
		if (predicateForOutputToFile)
		{
			outputFileName2 = programOption.option[optionFirstIndexInGroup + 4].optionParameterString;
			modeAppend2 = programOption.option[optionFirstIndexInGroup + 4].fileAppend;
			selection = selection | OutputDestinationOptionalFile;
		}
		else
		{
			selection = selection | OutputDestinationCOut;
		}
	}
	
	// In case of double or equal filenames
	if ((OutputDestinationFileAndOptionalFile == selection) && (outputFileName1 == outputFileName2))
	{
		// Overwrite previous selections
		selection = OutputDestinationFile;
	}
	if ((OutputDestinationFileAndOptionalFileAndCout == selection) && (outputFileName1 == outputFileName2))
	{
		// Overwrite previous selections
		selection = OutputDestinationFileAndCOut;
	}


	// So, now, depending on previous selected bits for output destinations
	switch (selection)
	{

	case OutputDestinationNull: // Nothing Selected
		selectedOstream =& nullStream;
		break;

	case OutputDestinationCOut:	// Only cout selected
		selectedOstream =& std::cout;
		break;

	case OutputDestinationFile:	// 1 File Only seelcted
		fileOnlyOutput1.open(outputFileName1, (modeAppend1 ? OutputHandling::IosModeAppend : OutputHandling::IosModeOverwrite));
		selectedOstream =& fileOnlyOutput1;
		break;

	case OutputDestinationFileAndCOut:	// 1 file and cout selected
		teeStream.setFileAndMode(outputFileName1, modeAppend1);
		selectedOstream =& teeStream;
		break;

	case OutputDestinationOptionalFile: // Output to optional file
		fileOnlyOutput2.open(outputFileName2, (modeAppend2 ? OutputHandling::IosModeAppend : OutputHandling::IosModeOverwrite));
		selectedOstream =& fileOnlyOutput2;
		break;
	case OutputDestinationOptionalFileAndCout: // Output to optional file and cout
		teeStream.setFileAndMode(outputFileName2, modeAppend2);
		selectedOstream =& teeStream;
		break;
	case OutputDestinationFileAndOptionalFile: // Output to optional file and normal file
		teeStream.setFileAndMode2FilesNoCout(outputFileName1, modeAppend1, outputFileName2, modeAppend2);
		selectedOstream =& teeStream;
		break;
	case OutputDestinationFileAndOptionalFileAndCout: // Output to optional file and normal file and cout
		teeStream.setFileAndMode(outputFileName1, modeAppend1, outputFileName2, modeAppend2);
		selectedOstream =& teeStream;
		break;

	}
}





// Yes, indeed. A global variable
// Here the program comand line parameters will be stored
ProgramOption programOption;


// Checks, if a string given in the command line is something that we are expecting
bool ProgramOption::checkIfStringIsAValidOption(const std::string& optString)
{
	return (option.end() != std::find_if(option.begin(), option.end(), [optString](const CommandLineOption& go) noexcept {return (optString == go.optionIdentificationString); }));
}


// Ste the options name string
void ProgramOption::setDefaultOptions()
{
	// 1. General Programm Options
	option.emplace_back(CommandLineOption(0, "-s", "", true));
	option.emplace_back(CommandLineOption(1, "-sa", "", false));
	option.emplace_back(CommandLineOption(2, "-umdnf", "", false));
	option.emplace_back(CommandLineOption(3, "-bse", "", false));
	option.emplace_back(CommandLineOption(4, "-opt", "", true));

	// 2. File Handling related Options
	option.emplace_back(CommandLineOption(5, "-fd", ".\\", true));
	option.emplace_back(CommandLineOption(6, "-fp", "", true));
	option.emplace_back(CommandLineOption(7, "-fo", "", false));
	option.emplace_back(CommandLineOption(8, "-fa", "", false));
	option.emplace_back(CommandLineOption(9, "-fafwsfn", "", false));

	// 
	option.emplace_back(CommandLineOption(10, "-paast", "", false));
	option.emplace_back(CommandLineOption(11, "-ppst", "", false));
	option.emplace_back(CommandLineOption(12, "-sfqmt", "", false));
	option.emplace_back(CommandLineOption(13, "-nomcdc", "", false));
	option.emplace_back(CommandLineOption(14, "-dnpast", "", false));

	// 3. Output controlling Options
	// 3.1 Definition for all other output Options
	option.emplace_back(CommandLineOption(15, "-pallc", "", false));
	option.emplace_back(CommandLineOption(16, "-pallf", "all.txt", true));
	option.emplace_back(CommandLineOption(17, "-palla", "all.txt", false));
	option.emplace_back(CommandLineOption(18, "-pallfauto", "all.txt", true));
	option.emplace_back(CommandLineOption(19, "-pallfautoa", "all.txt", false));

	// 3.2 Object code dump
	option.emplace_back(CommandLineOption(20, "-pcoc", "", false));
	option.emplace_back(CommandLineOption(21, "-pcof", "", true));
	option.emplace_back(CommandLineOption(22, "-pcoa", "objectcode.txt", false));
	option.emplace_back(CommandLineOption(23, "-pcofauto", "", true));
	option.emplace_back(CommandLineOption(24, "-pcofautoa", "objectcode.txt", false));


	// 3.3 Truth table output
	option.emplace_back(CommandLineOption(25, "-pttc", "", false));
	option.emplace_back(CommandLineOption(26, "-pttf", "", true));
	option.emplace_back(CommandLineOption(27, "-ptta", "truthtable.txt", false));
	option.emplace_back(CommandLineOption(28, "-pttfauto", "", true));
	option.emplace_back(CommandLineOption(29, "-pttfautoa", "truthtable.txt", false));


	// 3.4 Show Quine Mccluskey reduction table
	option.emplace_back(CommandLineOption(30, "-pqmtc", "", false));
	option.emplace_back(CommandLineOption(31, "-pqmtf", "", true));
	option.emplace_back(CommandLineOption(32, "-pqmta", "qmreduction.txt", false));
	option.emplace_back(CommandLineOption(33, "-pqmtfauto", "", true));
	option.emplace_back(CommandLineOption(34, "-pqmtfautoa", "qmreduction.txt", false));

	// 3.5 Show Prime Impicant reduction table
	option.emplace_back(CommandLineOption(35, "-ppirtc", "", false));
	option.emplace_back(CommandLineOption(36, "-ppirtf", "", true));
	option.emplace_back(CommandLineOption(37, "-ppirta", "pireduction.txt", false));
	option.emplace_back(CommandLineOption(38, "-ppirtfauto", "", true));
	option.emplace_back(CommandLineOption(39, "-ppirtfautoa", "pireduction.txt", false));

	// 3.6 Show Abstract Syntax Tree
	option.emplace_back(CommandLineOption(40, "-pastc", "", false));
	option.emplace_back(CommandLineOption(41, "-pastf", "", true));
	option.emplace_back(CommandLineOption(42, "-pasta", "ast.txt", false));
	option.emplace_back(CommandLineOption(43, "-pastfauto", "", true));
	option.emplace_back(CommandLineOption(44, "-pastfautoa", "ast.txt", false));


	// 3.7 Show Abstract Syntax Tree for MC/DC relevant values
	option.emplace_back(CommandLineOption(45, "-pmastc", "", false));
	option.emplace_back(CommandLineOption(46, "-pmastf", "", true));
	option.emplace_back(CommandLineOption(47, "-pmasta", "astmcdc.txt", false));
	option.emplace_back(CommandLineOption(48, "-pmastfauto", "", true));
	option.emplace_back(CommandLineOption(49, "-pmastfautoa", "astmcdc.txt", false));

	// 3.8 Show Resulting MC/DC test pairs
	option.emplace_back(CommandLineOption(50, "-pmtpc", "", false));
	option.emplace_back(CommandLineOption(51, "-pmtpf", "", true));
	option.emplace_back(CommandLineOption(52, "-pmtpa", "mcdctestpair.txt", false));
	option.emplace_back(CommandLineOption(53, "-pmtpfauto", "", true));
	option.emplace_back(CommandLineOption(54, "-pmtpfautoa", "mcdctestpair.txt", false));


	// 3.9 Show Resulting test coverage sets
	option.emplace_back(CommandLineOption(55, "-pmcsc", "", false));
	option.emplace_back(CommandLineOption(56, "-pmcsf", "", true));
	option.emplace_back(CommandLineOption(57, "-pmcsa", "testcoveragesets.txt", false));
	option.emplace_back(CommandLineOption(58, "-pmcsfauto", "", true));
	option.emplace_back(CommandLineOption(59, "-pmcsfautoa", "testcoveragesets.txt", false));

}


// Read the comand line options and process them
// function "main"'s comand line parameters can be directly passed to this function
void ProgramOption::readOptions(const sint argc, const cchar* const argv[])
{
	// Iterate over all program command line options
	for (sint i = 0; i < argc; )
	{
		// Copy command line option to a string
		std::string optionWord(argv[i]);
		// Check, if the given option is in the list of acceptable options
		const bool validOption{ checkIfStringIsAValidOption(optionWord) };
		// Command line parameter 0 ist the name of your program, including the path
		if ((i > 0) && !validOption)
		{
			// If option is invalid, emit error message and skip that
			std::cerr << "Error: Invalid Option specified: '" << optionWord << "'\n";
			++i;
		}
		else
		{
			// Something valid
			// if the next parameter is a parameter, then special handling to read a quotet string as filename with "\" not beeing a escape character
			// Meaning if this option in "optionWord", has a parameter
			if (option.end() != std::find_if(option.begin(), option.end(), [&](const CommandLineOption& go) noexcept {return (optionWord == go.optionIdentificationString) && (go.hasParameter); }))
			{
				// So this option should have a parameter
				// Out of bounds check. Is an additional parameter available
				if ((i + 1) < argc)
				{
					// Yes, there is more data in the command line
					// Get next data
					//std::quoted(optionWord, '"', '\000');
					std::string parameter(argv[i + 1]);

					// Check if the next parameter is an option, although we expect other data for the parameter
					if ('-' == parameter[0])
					{
						// Obviously somebody forgot to specify the parameter
						std::cerr << "Parameter following option '" << optionWord << "' not found\n";
						 
						++i; // Pointing to the option we just read. Do not save anything

					}
					else
					{
						// OK the parameter we read did not start with a "-" and is no option, but data
						// We will check now, if a special handling for a nested include of a additional file is requested
						if (argv[i] == option[OptionID::opt].optionIdentificationString)
						{
							// Yes, requested.
							// Try to open that file
							std::ifstream optionFile(parameter);
							if (!optionFile)
							{
								std::cerr << "ErrorCannot open option file: '" << parameter << "'\n";
								++i; // swallow "-opt"
								++i; // swallow filename
							}
							else
							{
								// Initialize counter for nesting Include files
								uint level{ 0 };
								// We want to prevent circular includes, hence we will check, if a file has already been read
								includeFileName.clear();
								includeFileName.push_back(parameter);

								// Start Recursive invocation of read option files
								readOptionsFromFile(optionFile, level);
								++i; // swallow "-opt"
								++i; // swallow filename
								// Do not store anything
							}
						}
						else
						{
							// the parameter was a standard parameter and does not need special handling
							// Save Option with Parameter
							commandLineString.emplace_back(OptionWithParameter(std::string(argv[i]), std::string(argv[i + 1])));
							++i;  // swallow what ever option
							++i;  // swallow parameter
						}
					}
				}
				else
				{
					++i; // stop loop
				}
			}
			else
			{
				// Just an option, without any parameter
				commandLineString.emplace_back(OptionWithParameter(std::string(argv[i])));
				++i;  // point to next option
			}
		}
	}
}
void ProgramOption::readOptionsFromFile(std::ifstream& infile, uint& level)
{
	// We want too avoid too many nested option files
	if (level > 10)
	{
		std::cerr << "Error: Too many nesting option files (>10)\n";
	}
	else
	{
		// This is the option that will be read
		std::string optionWord;


		// At some points in time we do not want to read a new string from the input stream
		// Special handling may force us to reuse an old string that has been read before.
		// Initially we will read data
		bool doRead{ true };

		// As long as input is available and we did not hit EOF
		while (!infile.eof())
		{

			// If we shall read a next string from the input stream
			if (doRead)
			{
				// Then do that and handle quote´d strings
				infile >> std::quoted(optionWord, '"', '\000');
			}
			else
			{
				// If we did not read a new string this time, then definitly do it next time. 
				doRead = true;
			}
			const bool validOption{ checkIfStringIsAValidOption(optionWord) };
			if (!validOption)
			{
				// Handle comments. Ignore everything # and follwing 
				if ('#' == optionWord[0])
				{
					infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				}
				else
				{
					// Unexpected end of file
					if (!infile.eof() && !validOption)
					{
						std::cerr << "Error: Invalid Option specified: '" << optionWord << "'\n";
					}
				}
			}
			if (!infile.eof() && validOption)
			{
				std::string parameter;

				// if the next option is a parameter, then special handling to read that parameter
				// So has the option a parameter
				if (option.end() != std::find_if(option.begin(), option.end(), [&](const CommandLineOption& go) noexcept {return (optionWord == go.optionIdentificationString) && (go.hasParameter); }) )
				{
					// If so, than read this parameter
					infile >> std::quoted(parameter, '"', '\000');
					if (!infile.eof())
					{
						// If the parameter starts with a hyphen(which denotes an option), then somebody forgot to give the expected parameter for 
						if ('-' == parameter[0])
						{
							std::cerr << "Parameter following option '" << optionWord << "' not found. Ignoring this option\n";
							// do not read a new string from the input stream next time and reuse this parameter as the next option word.
							doRead = false;
							// Set the parameter as next option
							optionWord = parameter;
						}
						else
						{
							// Special handling for option file, so the "-opt" Option
							if (optionWord == option[OptionID::opt].optionIdentificationString)
							{
								// Prevent circular inclusion of files, so check, if this file already has already been read
								if (std::find(includeFileName.begin(), includeFileName.end(), parameter) == includeFileName.end())
								{
									// File has not yet been read
									// So read the nested option file
									std::ifstream optionFile(parameter);
									if (!optionFile)
									{
										std::cerr << "Error: Cannot open option file: '" << parameter << "'\n";
									}
									else
									{
										// Remember that we have included this option file.
										// This to prevent, that we read it again
										includeFileName.push_back(parameter);

										// Nesting level for option includes
										// We will not allow option files to include other option files until ultimo. This will be limited.
										++level;
										readOptionsFromFile(optionFile, level); // Recursive Call
										--level;
									}
								}
								else
								{
									std::cerr << "Circular inclusion of Options file:  '" << parameter << "'    Ignoring that . . .\n";
								}
							}
							else
							{
								// Normal option with parameter,
								// So add option and parameter
								commandLineString.emplace_back(OptionWithParameter(optionWord, parameter));
							}
						}
					}
					else
					{
						std::cerr << "Unexpected End of file while reading parameter for option:  '" << optionWord << "'    Ignoring that . . .\n";
					}
				}
				else
				{
					// Normal option without any parameter
					commandLineString.emplace_back(OptionWithParameter(optionWord));
				}
			}
		}
	}
}


// Evaluate the options given in the commandline string
void ProgramOption::evaluateCommandLine()
{
	// Go through all command line options and set internal option entries
	// Copy command line options

	std::string optionIdString;
	ComandLineVectorIter clvi;

	uint indexBeginOutputOptions{ 0ULL };
	// Go through all possible defined options
	while (indexBeginOutputOptions < pmcsfautoa)
	{
		// All options are organized in groups of 5 elements
		for (uint i = 0; i < 5ULL; ++i)
		{
			// First Check, if a option is present and if so, copy data
			optionIdString = option[indexBeginOutputOptions + i].optionIdentificationString;
			clvi = std::find_if(commandLineString.begin(), commandLineString.end(), [&](const OptionWithParameter& owp) noexcept{ return (optionIdString == owp.groupOptionString); });
			if (commandLineString.end() != clvi)
			{
				// Valid option found. Set flag to active
				option[indexBeginOutputOptions + i].optionSelected = true;
				// If existing copy also the parameter
				if (!clvi->parameter.empty())
				{
					option[indexBeginOutputOptions + i].optionParameterString = clvi->parameter;
				}
			}
		}
		// Next group
		indexBeginOutputOptions += 5;
	}




	// 1. Check, if directory optione is given
	// Calculate the directory used for output

	// For file related waor, we need to check and create some directory / filename related data
	std::filesystem::path thisProgramPathAndFileName{ commandLineString[0].groupOptionString };
	std::filesystem::path parentPathP{ thisProgramPathAndFileName.parent_path() };

	// Search for fd option
	optionIdString = option[fd].optionIdentificationString;
	clvi = std::find_if(commandLineString.begin(), commandLineString.end(), [&](const OptionWithParameter& owp) noexcept{ return (optionIdString == owp.groupOptionString); });

	// Set default 
	// Default is the program execution directory
	option[fd].optionParameterString = parentPathP.string();

	// Check, if option fd is present
	if (commandLineString.end() != clvi )
	{
		// fd option present.
		// Check, if this is a valid directory.

		std::filesystem::path givenDirectory{ clvi->parameter };
		std::error_code errorCode;
		if (std::filesystem::is_directory(givenDirectory, errorCode))
		{
			option[fd].optionParameterString = givenDirectory.string();;
		}
		else
		{
			std::cerr << "Cannot find directory given with option fd:  '" << givenDirectory << "'  Error message: " << errorCode.message() << '\n';
		}
	}
	std::string targetDirectoryForFiles = option[fd].optionParameterString;




	// Check,if a file prefix has been given
	optionIdString = option[fp].optionIdentificationString;
	clvi = std::find_if(commandLineString.begin(), commandLineString.end(), [&](const OptionWithParameter& owp)noexcept { return (optionIdString == owp.groupOptionString); });

	if (commandLineString.end() != clvi)
	{
		// We allow only alpha numeric entries
		if (clvi->parameter.end() != std::find_if(clvi->parameter.begin(), clvi->parameter.end(), [](cchar c) noexcept {return !std::isalnum(narrow_cast<uchar>(c)); }))
		{
			std::cerr << "Error file prefix given with fp must only contain alphanumeric characters. Found:  '" << clvi->parameter << "'\n";
		}
		else
		{
			option[fp].optionParameterString = clvi->parameter;
		}
	}
	// For all filenames which are not explictely given
	std::string directoryPathPrefix = targetDirectoryForFiles + option[fp].optionParameterString;


	// check for option fa, File append mode
	optionIdString = option[fa].optionIdentificationString;
	clvi = std::find_if(commandLineString.begin(), commandLineString.end(), [&](const OptionWithParameter& owp)noexcept { return (optionIdString == owp.groupOptionString); });
	if (commandLineString.end() != clvi)
	{
		// fa found  
		// Set fa, reset fo
		option[fa].optionSelected = true;	// File append mode is true
		option[fo].optionSelected = false;	// File overwrite mode is false
	}

	// check for option fo, File overwrite mode
	// Overwrites possible settings by fa before
	optionIdString = option[fo].optionIdentificationString;
	clvi = std::find_if(commandLineString.begin(), commandLineString.end(), [&](const OptionWithParameter& owp)noexcept { return (optionIdString == owp.groupOptionString); });
	if (commandLineString.end() != clvi)
	{
		// fa found  
		// Set fo, reset fa
		option[fo].optionSelected = true;	// File overwrite mode is true
		option[fa].optionSelected = false;	// File append mode is false
	}

	const bool fileOverwriteMode{ option[fo].optionSelected };



	// check for option  fafwsfn. If you want to put data from several sections in one file, then specify files with same names and add this option
	// If overwrite mode is true, the first file will be overwritten.
	// If not specified and overwrite mode, a previously written file will be overwritten with new data

	optionIdString = option[fafwsfn].optionIdentificationString;
	clvi = std::find_if(commandLineString.begin(), commandLineString.end(), [&](const OptionWithParameter& owp)noexcept { return (optionIdString == owp.groupOptionString); });
	if (clvi != commandLineString.end())
	{
		// Set flag to active
		option[fafwsfn].optionSelected = true;
	}


	// Now go through all goups of output options




	// Check the ALL flags 
	// If one is set, then set the corresponding flags for all other
	// Copy result of all data to all the rest of the output options
	// Maybe overwritte later be section specific flags
	indexBeginOutputOptions = pallc;

	for (uint i = 0; i < 5; ++i)
	{
		//First Check, if a option is present and if so, set the flags
		optionIdString = option[indexBeginOutputOptions + i].optionIdentificationString;
		clvi = std::find_if(commandLineString.begin(), commandLineString.end(), [&](const OptionWithParameter& owp) noexcept{ return (optionIdString == owp.groupOptionString); });
		if (commandLineString.end() != clvi)
		{
			// Set flag to active
			option[indexBeginOutputOptions + i].optionSelected = true;

			uint index2BeginOutputOptions = pcoc;
			while (index2BeginOutputOptions < pmcsfautoa)
			{
				if (!option[index2BeginOutputOptions + i].optionSelected)
				{
					option[index2BeginOutputOptions + i].optionSelected = option[pallc + i].optionSelected;
				}
				// If the filename in the taregt field is empty, then copy it from the all options


				// ************************************************************
				// ************************************************************
				// ************************************************************
				// ************************************************************
				// ************************************************************
				if (option[index2BeginOutputOptions + i].optionParameterString.empty())
				{
					option[index2BeginOutputOptions + i].optionParameterString = option[pallc + i].optionParameterString;
				}
			
				index2BeginOutputOptions += 5;
			}
		}
	}

	// -pallc					Write all Data to console
	// -pallf "filename"		Write all Data to file "filename"
	// -palla"					Write all Data to file with automatic created filename
	// -pallfauto "filename"	Automatically write big data with lots of lines to file "filename", the rest to console
	// -pallfautoa				Automatically write big data with lots of lines to file with automatic created filename, the rest to console. Default



	// If section specific flags are set then flags set by "all" options will be overwritten

	indexBeginOutputOptions = pcoc;
	while (indexBeginOutputOptions < pmcsfautoa)
	{
		checkPrioritiesWithinGroups(indexBeginOutputOptions);
		indexBeginOutputOptions += 5;
	}



	setOverwriteAppendFlag();




	// Prepend all data with directory
	//directoryPathPrefix
		// Go through all entries. FInd the first with doble string. Set it to fo
	for (uint i = 1; i < 5; ++i)
	{
		uint index1BeginOutputOptions = pallc;
		while (index1BeginOutputOptions < pmcsfautoa)
		{
			if (!option[index1BeginOutputOptions + i].optionParameterString.empty())
			{
				option[index1BeginOutputOptions + i].optionParameterString = directoryPathPrefix + option[index1BeginOutputOptions + i].optionParameterString;
			}
			index1BeginOutputOptions += 5;
		}
	}

}


// In one group there may be several flags set. This functions sets the flags in a logical manner
void ProgramOption::checkPrioritiesWithinGroups(uint index)
{
	// -pallc					Write all Data to console
	// -pallf "filename"		Write all Data to file "filename"
	// -palla"					Write all Data to file with automatic created filename
	// -pallfauto "filename"	Automatically write big data with lots of lines to file "filename", the rest to console
	// -pallfautoa				Automatically write big data with lots of lines to file with automatic created filename, the rest to console. Default


	// Now check the priorities
	enum groupOffset { c, f, a, fauto, fautoa };

	// fauto is stronger than fautoa
	if (option[index + fauto].optionSelected && option[index + fauto].optionSelected)
	{
		option[index + fautoa].optionSelected = false;
	}

	// f is stronger than a
	if (option[index + a].optionSelected && option[index + f].optionSelected)
	{
		option[index + a].optionSelected = false;
	}


	// Check for equal filenames for fauto/fautoa and f/a
	// Read the names from fautoa and fauto
	std::string filenameFromAuto;
	if (option[index + fauto].optionSelected)
	{
		filenameFromAuto = option[index + fauto].optionParameterString;
	}
	if (option[index + fautoa].optionSelected)
	{
		filenameFromAuto = option[index + fautoa].optionParameterString;
	}

	// Read the neames from f/a
	std::string filenameFromForA;
	if (option[index + f].optionSelected)
	{
		filenameFromForA = option[index + f].optionParameterString;
	}
	if (option[index + a].optionSelected)
	{
		filenameFromForA = option[index + a].optionParameterString;
	}

	
	// If output filenames are equal, then we will write to f or a specified file
	if (filenameFromAuto == filenameFromForA)
	{
		// Reset filenames for auto entries
		option[index + fautoa].optionParameterString = "";
		option[index + fauto].optionParameterString = "";
		option[index + fautoa].optionSelected = false;
		option[index + fauto].optionSelected = false;
	}
}



// Files may be opened in append or in overwrite mode
// Depending on set flags and possibly double filenames we will set the file open mode correspondingly
void ProgramOption::setOverwriteAppendFlag()
{
	// Initially we expect overwrite mode
	bool appendFlag{ false };
	// If the append flag is set
	if (option[fa].optionSelected)
	{
		appendFlag = true;
	}

	// Go through all output flags and set the global append mode flag
	uint indexBeginOutputOptions{ pallc };
	while (indexBeginOutputOptions < pmcsfautoa)
	{
		for (uint i = 0; i < 5; ++i)
		{
			option[indexBeginOutputOptions + i].fileAppend = appendFlag;
		}
		indexBeginOutputOptions += 5;
	}

	
	// Handle fafwsfn option. 
	// Append files with same given filenames. If for the below files a double name is given, then the data will go into one file. in the case of -fo, the initial file is overwritten.
	if (option[fafwsfn].optionSelected)
	{
		// Go through all filenames and check if we have several identical filenames
		uint index1BeginOutputOptions{ pallc };
		while (index1BeginOutputOptions < (pmcsfautoa))
		{
			// Iterate over all elements in one group
			// Start with 1 . So do not check output to std::cout
			for (uint i = 1; i < 5; ++i)
			{
				// If this specif option is selected
				if (option[index1BeginOutputOptions + i].optionSelected)
				{
					std::string filenameFirst{ option[index1BeginOutputOptions + i].optionParameterString };
					if (!filenameFirst.empty() )
					{
						// Then compare this with the element from the following groups
						uint index2BeginOutputOptions{ index1BeginOutputOptions + i + 5 };
						while (index2BeginOutputOptions < pmcsfautoa)
						{
							// If the option in the other group is also selected
							if (option[index2BeginOutputOptions].optionSelected)
							{
								// And if we have the same filename
								if (filenameFirst == option[index2BeginOutputOptions].optionParameterString)
								{
									// Then we will open this file in append mode
									option[index2BeginOutputOptions].fileAppend = true;
									// And we remeber that for both elements that wed did compare
									option[index1BeginOutputOptions].filenameExistingAlsoElswhere = true;
									option[index2BeginOutputOptions].filenameExistingAlsoElswhere = true;
								}
							}
							index2BeginOutputOptions += 5;
						}
					}
				}
			}
			index1BeginOutputOptions += 5;
		}

		// If there are double filenames and the fo flag is set, then only the first file will be overwritten.
		if (option[fo].optionSelected)
		{
			// Go through all entries. FInd the first with double string. Set it to fo
			for (uint i = 1; i < 5; ++i)
			{
				index1BeginOutputOptions = pallc;
				while (index1BeginOutputOptions < pmcsfautoa)
				{
					if (option[index1BeginOutputOptions+i].filenameExistingAlsoElswhere)
					{
						// Set overwrite for the first element
						option[index1BeginOutputOptions + i].fileAppend = false;
						break;
					}
					index1BeginOutputOptions += 5;
				}
			}
		}
	}
}




