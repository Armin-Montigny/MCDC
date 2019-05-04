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
#ifndef CLOPTION_HPP
#define CLOPTION_HPP


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




#include "types.hpp"

#include <iostream>
#include <fstream>

#include <streambuf>



// This is the main storgae area, where will keep all program options and their parameters
// Also some additional attributes for Fielhandling will be stored.
struct CommandLineOption
{
	CommandLineOption(sint i, std::string oc, std::string ods, bool fn) :	id(i), optionIdentificationString(oc), optionParameterString(ods), hasParameter(fn){}

	// Running number of Option Identifier. Option maybe selected through this
	sint id;	

	// The option identification Strin´g, something like "-fd"
	std::string optionIdentificationString;
	// If an option has a parameter, like a filename, it will be stored here
	std::string optionParameterString;

	// Indicates that an option has a parameter that needs to be read, when the option is given
	bool hasParameter;
	// Show that the option was selected and is active
	bool optionSelected{ false };

	// This is for file handling. It describes the open mode for a file. Either override or append.
	bool fileAppend{ false };  // This means, default is override
	// If 2 options with file name use the same filename. Then we do a special handling
	bool filenameExistingAlsoElswhere{ false };
};



// This is a container for all comand line options and some additional helper functions
class ProgramOption
{
public:

#pragma warning(suppress: 26455)
	ProgramOption() : option() { setDefaultOptions(); }
	// Read the options given from the comand line. Uses parameters given by main
	void readOptions(const sint argc, const cchar* const argv[]);
	// This evaluates the command line options, make plausibility checks and sets priorities
	void evaluateCommandLine();

	// This will allow for easier access of the options with a name (and not a numeric index only)
	enum OptionID
	{
		s,
		sa,
		umdnf,
		bse,
		opt,
		fd,
		fp,
		fo,
		fa,
		fafwsfn,
		paast,
		ppst,
		sfqmt,
		nomcdc,
		dnpast,
		pallc,
		pallf,
		palla,
		pallfauto,
		pallfautoa,
		pcoc,
		pcof,
		pcoa,
		pcofauto,
		pcofautoa,
		pttc,
		pttf,
		ptta,
		pttfauto,
		pttfautoa,
		pqmtc,
		pqmtf,
		pqmta,
		pqmtfauto,
		pqmtfautoa,
		ppirtc,
		ppirtf,
		ppirta,
		ppirtfauto,
		ppirtfautoa,
		pastc,
		pastf,
		pasta,
		pastfauto,
		pastfautoa,
		pmastc,
		pmastf,
		pmasta,
		pmastfauto,
		pmastfautoa,
		pmtpc,
		pmtpf,
		pmtpa,
		pmtpfauto,
		pmtpfautoa,
		pmcsc,
		pmcsf,
		pmcsa,
		pmcsfauto,
		pmcsfautoa
	};

	// In the first step, the options from the comand line will be stored in a very simple POD, without additional overhead
	// Later it will be moved to The Option vector
	struct OptionWithParameter
	{
		OptionWithParameter(std::string sOption) : groupOptionString(sOption) {}
		OptionWithParameter(std::string sOption, std::string sParameter) : groupOptionString(sOption), parameter(sParameter) {}

		std::string groupOptionString;  // whatever is read from the command line. Can also be invalid or a comment
		std::string parameter; // Whatever parameter is read, can also be invalid
	};

	// The options
	using Option = std::vector<CommandLineOption>;
	Option option;
protected:


	// Initialize the option vector with all necessary "options"
	void setDefaultOptions();

	// Read options from file. If the "-opt" is given then additional options will be read from files
	void readOptionsFromFile(std::ifstream& infile, uint& level);

	bool checkIfStringIsAValidOption(const std::string& optString);

	// Saome group options have priority over others. This will evaluate and set the values as desired
	void checkPrioritiesWithinGroups(uint offset);

	// Considers the handling of file open modes, dependent on flags and maybe similar filenames
	void setOverwriteAppendFlag();

	// The raw comand line values.
	using ComandLineVector = std::vector<OptionWithParameter>;
	using ComandLineVectorIter = ComandLineVector::iterator;
	ComandLineVector commandLineString;

	// Parameters can not only be given in the comand line but also in 1 or many options files (maybe nested)
	using IncludeFileNameVector = std::vector<std::string>;	IncludeFileNameVector includeFileName;
};



namespace OutputHandling
{

	// Some constants to make more easy and readbale names for file open modes
	constexpr std::ios_base::openmode IosModeAppend{ std::ios_base::out | std::ios_base::app };
	constexpr std::ios_base::openmode IosModeOverwrite{ std::ios_base::out };
	// Depending on slected  options, program output can be driected to differen streams
	// So output of data into a file, into more than on file and or to std::cout

	// This is a stream which does not output anything
	class NullStream : public std::ostream
	{
		// sreambuffer doing nothing
		class NullBuffer : public std::streambuf
		{
		public:
			sint overflow(sint c) noexcept override { return c; }
		} nullBuffer;
	public:
#pragma warning(suppress: 26455)
		NullStream() : std::ostream(&nullBuffer) {}
		NullStream(const NullStream& ) = delete;
		NullStream& operator=(const NullStream& ) = delete;
	};





	// This is the implementation of a T Stream. When you insert data into this ostream, it will be send
	// to one or more strwambuffers at the same time
	// At the moment we support up to 2 files and std::cout

	class TeeStream : public std::ostream
	{
		// The streambuffer has 3 internal stream buffers. Date will be send to those
		struct TeeStreamBuffer : std::streambuf
		{
			TeeStreamBuffer() noexcept : streamBufferPath1(nullptr), streamBufferPath2(nullptr), streamBufferPath3(nullptr) {}
			// Set streambuffers with rdbuf values
			void setStreamBuffer(std::streambuf* sb1, std::streambuf* sb2, std::streambuf* sb3 = nullptr) noexcept { streamBufferPath1 = sb1; streamBufferPath2 = sb2; streamBufferPath3 = sb3; }

			// Necessary overwrites for overflow and sync
			sint overflow(sint c) override;
			sint sync() override;

			// pointer to the rdbufs of the streambuffers to use
			std::streambuf* streamBufferPath1;
			std::streambuf* streamBufferPath2;
			std::streambuf* streamBufferPath3;
		};

	public:

#pragma warning(suppress: 26455)
		// Initialize the ostream with the TeeStreamBuffer
		TeeStream() : std::ostream(&teeStreamBuffer), fileOut1(), fileOut2(), teeStreamBuffer() {}
		TeeStream(const TeeStream&) = delete;
		TeeStream& operator =(const TeeStream&) = delete;

		// Send to one file only. Filename as std::string
		void setFileAndMode(const std::string& filename, const bool appendMode);
		// Send to 2 files filename as std::string
		void setFileAndMode(const std::string& filename1, const bool appendMode1, const std::string& filename2, const bool appendMode2);
		// Send to one file only. Filename as cchar*
		void setFileAndMode(const cchar* filename, const bool appendMode);
		// Send to 2 files filename as cchar *
		void setFileAndMode(const cchar* filename1, const bool appendMode1, const cchar* filename2, const bool appendMode2);

		// Sent to 2 files. None of them shall be std::cout. Filenames are given as std::string
		void setFileAndMode2FilesNoCout(const std::string& filename1, const bool appendMode1, const std::string& filename2, const bool appendMode2);
		// Sent to 2 files. None of them shall be std::cout. Filenames are given as cchar*
		void setFileAndMode2FilesNoCout(const cchar* filename1, const bool appendMode1, const cchar* filename2, const bool appendMode2);

	protected:
		std::ofstream fileOut1;
		std::ofstream fileOut2;
		TeeStreamBuffer teeStreamBuffer;
	};


}


// As written above
// Depending on program options, the output can be directed to different streams
// Additionally, based on some precondition, the output can be redirected com std::cout to a file
// This, for example for long lists or big data, where it is not desired to 100000lines on the screen

// There is a function that returens a reference to the stream. This shall be used for all stream insert operations

class OutStreamSelection
{
protected:
	// Binary coding of Output destinations. Will be ored together to calsculate mutltiple targets

	static constexpr uint OutputDestinationNull{ 0U };
	static constexpr uint OutputDestinationCOut{ 1U };
	static constexpr uint OutputDestinationFile{ 2U };
	static constexpr uint OutputDestinationFileAndCOut{ 3U };
	static constexpr uint OutputDestinationOptionalFile{ 4U };
	static constexpr uint OutputDestinationOptionalFileAndCout{ 5U };
	static constexpr uint OutputDestinationFileAndOptionalFile{ 6U };
	static constexpr uint OutputDestinationFileAndOptionalFileAndCout{ 7U };

public:
	// Create a stream. For a option group and with a prdicate for automatic redicrection from std::cout to file
	explicit OutStreamSelection(uint optionFirstIndexInGroup, const bool predicateForOutputToFile);
	OutStreamSelection(const OutStreamSelection&) = delete;
	OutStreamSelection& operator=(const OutStreamSelection&) = delete;


	// Thsi returns the stream to be used
	std::ostream& operator ()() noexcept {	return *selectedOstream; }

	// checks if only a nullStream is selceted
	bool isNull() noexcept {	return selectedOstream ==& nullStream; }
	// Checks, if data will be send to std::cout (no necessarily exclusively)
	bool hasStdOut()noexcept { return (OutputDestinationCOut == (selection&  OutputDestinationCOut));  }

protected:
	// Intermedieate internal storage to the stream. 
	std::ostream* selectedOstream;
	// Depending on program options, varius stream classes may be selected 
	OutputHandling::TeeStream teeStream;
	OutputHandling::NullStream nullStream;
	std::ofstream fileOnlyOutput1;
	std::ofstream fileOnlyOutput2;
	// Herr the selection, the output target, will be stored for later inquiries
	uint selection{ OutputDestinationNull };
};


// Program Options will be global
extern ProgramOption programOption;

#endif