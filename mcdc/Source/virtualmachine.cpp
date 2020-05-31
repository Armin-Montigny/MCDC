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



//
// A virtual machine to evaluate boolean expression
//
// A compiler, consisting of a scanner, a parser and a code generator creates object code
// with a contained symbol table.
//
// The virtual machine has a loader, that loads the object code and creates an executable programm
// This is comparable with a linker.
//
// The generated executable program can be printed.
// The program consist of Opcodes. Each Opcode has a  execution function
// and up to 3 parameters. The program can be run and will then interpret opcodes
// end execute them. The input variable value (given as a parameter) will be
// assigned to the boolean variables. Intermediate results will be stored in machine registers.
// Machine Registers are temporary memory locations that can also be reused.
// The boolean expression will be evaluated according to the program and the result will be returned.
// Feeding the virtual machine with all possible compbinations of input variables will create
// a truth table. But only the minterms will be stored later.
//


#include "codegenerator.hpp"
#include "cloption.hpp"
#include "virtualmachine.hpp"

#include <iostream>
#include <iomanip>
#include <string>


// Load the object code into the virtual machine for execution
// The source is needed only for documentation output
// The resulting code is printed
// After loading the object code, the program can be ececuted

void VirtualMachineForBooleanExpressions::load(ObjectCode& objectCode, const std::string& source)
{
	// On executable code line of the resulting programm
	// An executable code line consists of a function (for the operation) and 3 parameters
	ExecutableCodeLine executableCodeLine;
	// Counting the needed virtual machine registers, so that those can be allocated in the end.
	uint maxRegister{ 0 };

	// Clear the program before loading new object code. So delete all executable lines
	program.clear();

	// And, before starting, compact the symbol table
	objectCode.symbolTable.compact();
	numberOfLiterals = objectCode.symbolTable.numberOfSymbols();
	
	// In order to select the correct output destination, we want to find some complexity metric
	const uint opCodeSize{ narrow_cast<uint>(objectCode.opCode.size()) };
	const bool predicateForOutputToFile{ (opCodeSize > 150) };
	OutStreamSelection outStreamSelection(ProgramOption::pcoc, predicateForOutputToFile);
	std::ostream& os{ outStreamSelection() };

	// Inform the user
	os << "\n------------------ Compiling boolean expression\n\n'" << source << "'\n\n------------------ Compilation result. Virtual Machine Executable Code\n\n" ;
	uint counter{ 0 };
	os << std::left;

	// Iterate over the object code and create executables
	for (const OpCodeLine& opCodeLine : objectCode.opCode)
	{
		// Function for getting the max register number
		const auto gMR = [&maxRegister](uint targetResister) { if (targetResister > maxRegister) maxRegister = targetResister; return targetResister; };

		// Depending on opcode in Object Code
		switch (opCodeLine.token)
		{
		case Token::ID:
			// Print to the output destination and calculate max register number
			os << std::setw(8) << counter << " L    " << narrow_cast<cchar>(opCodeLine.parameter3) << "         --> R" << gMR(opCodeLine.parameter2) << '\n';
			// Set the "execution function" which is implemented as lambda
			executableCodeLine.execute = [&](uint a, uint b, uint c) {	this->operationLoadID(a, b, c); };
			// And then copy the üarameters, which are virtual machine registers
			executableCodeLine.parameter1 = objectCode.symbolTable.normalizedInput[opCodeLine.parameter1];
			executableCodeLine.parameter2 = opCodeLine.parameter2;
			break;

		case Token::IDNOT:
			// Print to the output destination and calculate max register number
			os << std::setw(8) << counter << " LN   " << narrow_cast<cchar>(opCodeLine.parameter3) << "         --> R" << gMR(opCodeLine.parameter2) << '\n';
			executableCodeLine.execute = [&](uint a, uint b, uint c) {	this->operationLoadNotID(a, b, c); };
			// And then copy the üarameters, which are virtual machine registers
			executableCodeLine.parameter1 = objectCode.symbolTable.normalizedInput[opCodeLine.parameter1];
			executableCodeLine.parameter2 = opCodeLine.parameter2;

			break;
		case Token::OR:
			// Print to the output destination and calculate max register number
			os << std::setw(8) << counter << " OR   R" <<  std::setw(2) << opCodeLine.parameter1 << "  R" << std::setw(2) << opCodeLine.parameter2 << "  --> R" << gMR(opCodeLine.parameter3) << '\n';
			// Set the "execution function" which is implemented as lambda
			executableCodeLine.execute = [&](uint a, uint b, uint c) {	this->operationOr(a, b, c); };
			// And then copy the üarameters, which are virtual machine registers
			executableCodeLine.parameter1 = opCodeLine.parameter1;
			executableCodeLine.parameter2 = opCodeLine.parameter2;
			executableCodeLine.parameter3 = opCodeLine.parameter3;
			break;
		case Token::XOR:
			// Print to the output destination and calculate max register number
			os << std::setw(8) << counter << " XOR  R" << std::setw(2) << opCodeLine.parameter1 << "  R" << std::setw(2) << opCodeLine.parameter2 << "  --> R" << gMR(opCodeLine.parameter3) << '\n';
			// Set the "execution function" which is implemented as lambda
			executableCodeLine.execute = [&](uint a, uint b, uint c) {	this->operationXor(a, b, c); };
			// And then copy the üarameters, which are virtual machine registers
			executableCodeLine.parameter1 = opCodeLine.parameter1;
			executableCodeLine.parameter2 = opCodeLine.parameter2;
			executableCodeLine.parameter3 = opCodeLine.parameter3;
			break;
		case Token::AND:
			// Print to the output destination and calculate max register number
			os << std::setw(8) << counter << " AND  R" <<  std::setw(2) << opCodeLine.parameter1 << "  R" << std::setw(2) << opCodeLine.parameter2 << "  --> R" << gMR(opCodeLine.parameter3) << '\n';
			// Set the "execution function" which is implemented as lambda
			executableCodeLine.execute = [&](uint a, uint b, uint c) {	this->operationAnd(a, b, c); };
			// And then copy the üarameters, which are virtual machine registers
			executableCodeLine.parameter1 = opCodeLine.parameter1;
			executableCodeLine.parameter2 = opCodeLine.parameter2;
			executableCodeLine.parameter3 = opCodeLine.parameter3;
			break;
		case Token::NOT:
			// Print to the output destination and calculate max register number
			os << std::setw(8) << counter << " NOT  R" <<  std::setw(2) << opCodeLine.parameter1 << "       --> R" << gMR(opCodeLine.parameter2) << '\n';
			// Set the "execution function" which is implemented as lambda
			executableCodeLine.execute = [&](uint a, uint b, uint c) {	this->operationNot(a, b, c); };
			// And then copy the üarameters, which are virtual machine registers
			executableCodeLine.parameter1 = opCodeLine.parameter1;
			executableCodeLine.parameter2 = opCodeLine.parameter2;
			break;
		case Token::END:
			// Print to the output destination and calculate max register number
			os << std::setw(8) << counter << " END            --> R" << gMR(opCodeLine.parameter1) << '\n';
			// Set the "execution function" which is implemented as lambda
			executableCodeLine.execute = [&](uint a, uint b, uint c) {	this->operationEnd(a, b, c); };
			// And then copy the üarameters, which are virtual machine registers
			executableCodeLine.parameter1 = opCodeLine.parameter1;
			break;
#pragma warning(suppress: 4061)
#pragma warning(suppress: 4062)
		}
		// Add a new executable line to the program
		program.push_back(executableCodeLine);
		++counter;
	}
	// After program has bee created, provide space for machine registers
	machineRegister.resize(static_cast<size_t>(maxRegister) + 1U, 0);

	os << "\n\n";

}



// Run the program, generated from the boolean expression, for a given input value
bool VirtualMachineForBooleanExpressions::run(uint sv)
{
	sourceValue = sv;
	// Run the program line by line 
	for (const ExecutableCodeLine& executableCodeLine : program)
	{
		// Call the function pointer for the operation and hand over 3 parameters
		executableCodeLine.execute(executableCodeLine.parameter1, executableCodeLine.parameter2, executableCodeLine.parameter3);
	}
	// ProgramResult will be set by end operation
	return programResult ? true: false;
}



