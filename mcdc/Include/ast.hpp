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
#ifndef AST_HPP
#define AST_HPP

// Classes and functions to deal with a Abstract Syntax Tree (AST) for boolean expressions
// 
// The input to this software is a boolean expression given as string. The boolean expression consists
// of terminals/variables/conditions a-z and, in the negated form A-Z. Meaning A ist the same as (NOT a).
// As operators are defined: AND, OR, XOR, NOT, BRACKETS. Details are shown in the grammar file.
//
// A compiler, sonsisting of a Scanner, Parser and Code Generator, reads the source boolean expression
// and converts it to a Abstract Syntax Tree. The Abstract syntax tree is implemented as a vector of
// tree elements with a linkage to children and parents. The linkage is done via the index in the vector.
// If a tree element has no parent, then it is the root element. A binary tree, meaning a alement (node)
// can have maximum 2 children. This approach is used, because boolean operations are binary as well.
// So, each node can have 0, 1 or 2 children. In case of 0 children, the node is a leaf, and denotes 
// a variable. 2 children are for Standard binary boolean operations. And for example, the unary
// not has only one child.
//
// The AST uses attributes for evaluation purposes. Giving a test vector for the used boolean
// variables (Example. Variables abc. Testvector 3 means: a=0,b=1,c=1) the resulting values
// of all nodes can be calculated.
// Also Booelan Short Cut Evealuation can be taken into account.
//
// The tree can be printed to the screen or in a file. For easier reading, the printout is turned
// 90° counter clock wise.
//
// The virtual Machine for AST is used to perform operations on ASTs




#include "types.hpp"
#include "token.hpp"
#include "symboltable.hpp"
#include "cloption.hpp"

// This is the indicator for no linkage. So if the node has no children, we will set this value in the link field
#pragma warning(suppress: 26472)
constexpr uint ASTNoLinkedElement{ UINT_MAX };




// The AST element, can be root or node or leaf. Depending on linkage
// Since therr can be only zero or one or two chuldren, we will not use an sint, to void assignment errors
enum class NumberOfChildren : uint_fast8_t { zero, one, two };

// AST element or node. Will be put in a vector to form the abstract syntax tree
struct AstNode
{
	// This is the ID of the not. It is normally the same as the index in the vector
	// where it is stored. However, Should there be an erase or insert operation 
	// in the surrounding container, this value will always be valid.
	// It is also used in liankacke calculations

	uint ownID{ 0U };

	// Number of children: Either 0 or 1 or 2
	NumberOfChildren numberOfChildren{ NumberOfChildren::zero };


	// The linkage fields. Will be filled with the ownID of the linked children or parent
	// The order is important. In the boolean expression "a+b", a will be the left child
	uint childLeftID{ ASTNoLinkedElement };
	uint childRightID{ ASTNoLinkedElement };

	// The parent is not necessarily needed, but makes traversal easier
	// No parent means: It is the root element of the tree
	uint parentID{ ASTNoLinkedElement };



	// used for evaluating the boolean expression. 
	// According tho the syntax and the given values for the input variables
	// a resulting value will be calculated for this node
	bool value{ false };

	// In case of boolean-shortcut-evaluation some nodes and leaf will not be considered
	// in the evaluation of the results. This will be indicated with this flag
	// In the printout of the evaluated AST, an X is used to show none evaluated nodes
	bool notEvaluated{ false };
	
	// Stores the token given by the compiler/scanner
	TokenWithAttribute tokenWithAttribute;

	// Helper Variables. Just used for printing the AST
	uint printRow{ 0U };	// In what row the node name with attributes will be printed
	uint level{ 0U }; // And the column for printing. Final column is level * "some fixed tabulator value".

};



// The Abstract Syntax Tree is symply a vector of Nodes.
// So the tree elements will be stored linearly. Intepretation of the tree is
// only possible with linkage fields
 
using AbstractSyntaxTree = std::vector<AstNode>;
using AST = AbstractSyntaxTree;




// This class is used to perform some operations on a AST
// It also contains an AST

class VirtualMachineForAST
{
public:
	// Reset everything
	void clear() noexcept { ast.clear(); row = 0; level = 0U; sourceValue = 0U;}
	// Add a node to the tree
	void add(const AstNode && astElement) { ast.push_back(astElement); }

	// Program output. Print jsut the structure of a tree
	void printTreeStandard(const std::string& source);
	// Program output. Print the structure of a tree and the associated values to stream os
	void printTree(std::ostream& os);

	// After all nodes have been added, the properties of the nodes will be set
	void calculateAstProperties();

	// Add a symbol, a variable/terminal/condition to the symbol table. Done in the compilation phase
	void addSymbol(cchar s); 

	// For printing and MCDC coverage calculation, we want to know, how many 
	// different variables/terminals/conditions are in our boolean expression
	uint maxConditionsInTree() noexcept { return symbolTable.numberOfSymbols();	}

	// This sets the varaibles/terminals/conditions to a value and calculates the result according the the rules of boolean algebra
	bool evaluateTree(uint inputValue);

	// The AST. And yes, it is public.
	AST ast{};
	// Table of symbols/variables/terminals/conditions used in the tree
	SymbolTable symbolTable;
protected:


	// Evaluation of the AST (calculating the result) will be done recursivly
	void evaluateTreeRecursive(uint index);
	// Calculating the properties of the AST will also be done recursively. 
	void calculateAstPropertiesRecursive(uint index);

	// This is important for boolean-shortcut-evaluation
	// If a shortcut evaluation is possible, then all subtree elements will also not be evaluated 
	// This sets all children to "not evaluated"
	void setSubTreetoMasked(uint index);

	// Helper Variables. Just used for printing the AST
	uint row{ 0U };
	uint level{ 0U };

	// And this is the source value for the variables in the AST. The smallest letter is the MSB
	// So, for an expression "cd + ab", a source value of 6 means: a=0, b=1, c=1, d=0
	uint sourceValue{ 0U };
};


#endif // !AST_HPP

