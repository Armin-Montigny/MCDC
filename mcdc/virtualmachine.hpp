#pragma once

#ifndef VIRTUALMACHINE_HPP
#define VIRTUALMACHINE_HPP

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
// This is a boolean machine


#include "types.hpp"
#include "codegenerator.hpp"

#include <functional> 



using MachineRegisterType = uint_fast8_t;


class VirtualMachineForBooleanExpressions
{

public:
	// The virtual machin loads the object code and creates an executable programm. 
	void load(ObjectCode &objectCode, const  std::string &source);	// source is for debug purposes only
	
	// Run the executable code with an input source value of sv
	bool run(uint sv);


protected:

	// For operations LoadID and LoadNotID
	MachineRegisterType getSourceValue(uint index) noexcept;
	MachineRegisterType getSourceValueNegated(uint index) noexcept;

	// Machine registers for storing intermediate calculation results
	std::vector<MachineRegisterType> machineRegister;

	// When the program is run, then the value in this variable will be used as input and mapped to the boolean variables
	uint32 sourceValue{ null<uint32>() };
	// Holds the number of literals in the boolean expression. Get from symbol table in object code
	uint numberOfLiterals{ 0U };


	// A program consists of executable code lines
	// The virtual machine can interpret opcodes with 3 parameters
	struct ExecutableCodeLine
	{
		std::function<void(uint, uint, uint)>  execute;	// One of the functions defined below. Will be set by the loader
		// Up to 3 parameters for the opcode
		uint parameter1{ null< uint>() };
		uint parameter2 { null< uint>() };
		uint parameter3 { null< uint>() };
	};

	// The executable program
	using Program = std::vector<ExecutableCodeLine>;
	Program program;

	// The result of an executed program
	MachineRegisterType programResult{};

	// The executables for one opcode
	void operationLoadID(uint source, uint destination, uint) { machineRegister[destination] = getSourceValue(source); }
	void operationLoadNotID(uint source, uint destination, uint) { machineRegister[destination] = getSourceValueNegated(source); }
	void operationOr(uint source1, uint source2, uint destination) { machineRegister[destination] = static_cast<uint>(machineRegister[source1] | machineRegister[source2]); }
	void operationXor(uint source1, uint source2, uint destination) { machineRegister[destination] = static_cast<uint>(machineRegister[source1] ^ machineRegister[source2]); }
	void operationAnd(uint source1, uint source2, uint destination) { machineRegister[destination] = static_cast<uint>(machineRegister[source1] & machineRegister[source2]); }
	void operationNot(uint source, uint destination, uint) { machineRegister[destination] = static_cast<uint>((~machineRegister[source] & bitMask[0])); }
	void operationEnd(uint source, uint, uint) { programResult = machineRegister[source]; }

};



inline MachineRegisterType VirtualMachineForBooleanExpressions::getSourceValue(uint index)  noexcept
{
	const MachineRegisterType result = (null<uint32>() == (sourceValue & bitMask[index])) ? 0U : 1U;
	return result;

}
inline MachineRegisterType VirtualMachineForBooleanExpressions::getSourceValueNegated(uint index)  noexcept
{
	const MachineRegisterType result = (null<uint32>() == (sourceValue & bitMask[index])) ? 1U : 0U;
	return result;
}

#endif