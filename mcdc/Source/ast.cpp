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



#include "ast.hpp"
#include "cloption.hpp"

#include <iostream>
#include <iomanip>
#include <cctype>
#include <sstream> 



// Depending on selected program options, we will show the abstract syntax tree with attributes
void VirtualMachineForAST::printTreeStandard(const std::string& source)
{
	// First we want to see the complexity of the AST
	// Big AST may be automatically printed to a file
	// Calculate the requested output stream. Pleass note: This can also be a T
	// Meaning, output to file and cout at the same time is possible
	const uint astSize{ narrow_cast<uint>(ast.size()) };
	const bool predicateForOutputToFile{ (astSize > 200) };
	OutStreamSelection outStreamSelection(ProgramOption::pastc, predicateForOutputToFile);
	std::ostream& os = outStreamSelection();

	// If program option selected output at all
	if (!outStreamSelection.isNull())
	{
		// Then print headers and footers and the AST (via a subfunction)
		os << "\n------------------ AST   Abstract Syntax Tree      (with attributes) for boolean expression:\n\n'" << source<< "'\n\n------------------\n\n";
		printTree(os);
		os << "\n\n";
	}

}

// Main function to print an AST
// Please note: The given os mayby std::cout, a file or both
// The tree is printed in 90° counter clockwise to have a better usage of the output media
// Line numbers are shown and the operators have also references to these line numbers
// With that it is possible to identify the childs of a node, For eaxmple:
// For the function  "!((a+b)(c+d))"
//// the printout looks like that
// 
// 0                                  ID a(0)
// 1                       OR 0, 2 (0)
// 2                                  ID b(0)
// 3            AND 1, 5 (0)
// 4 NOT 3 (0)                        ID c(0)
// 5                       OR 4, 6 (0)
// 6                                  ID d(0)
// 7
//
// "OR 0, 2 (0)"  means:    Do a OR with the element in line  0 and line 2. The result is (0)
// "AND 1, 5 (0)" means:	Do an AND with the elements in line 1 and line 5.  The result is (0)
//
// Printing is not that easy, because for complex AST theire may be more than one node in a line
// And we can only print line by line. So, we will first print in an internal buffer and 
// if the line is done, we will output the complete line
// 
// The output rows will be calculated in a different routine that sets all properties of an AST
//
void VirtualMachineForAST::printTree(std::ostream& os)
{
	// We will print a tree with a width of maximum 1000 chararcters (columns)
	constexpr uint maxBufSize{ 1000 };
	// This is the tab size, the distance between 2 levels of an AST
	constexpr long tab{ 11 };

	// Because we will c-style output, we need to set a 0 terminator at the end of the buffer, 
	// after the last entry for a line has been printed
	uint outputLastPosition{ 0 };

	// Calculate all output strings. Go through all possible rows
	for (row = 0; row < narrow_cast<uint>(ast.size()); ++row)
	{
		// Temporary buffer. Plain old array
		cchar out[maxBufSize + 1];
		// Set complete output buffer tp space ' '
		memset(&out[0], ' ', maxBufSize);
		// Terminate the output buffer with C-Style string terminator 0
		out[maxBufSize] = null<cchar>();

		// Now check all elements in the AST
		for (uint i = 0; i < narrow_cast<uint>(ast.size()); ++i)
		{
			// If the desired row (from the outer loop) matches with the calculated printRow of this node, then we need to do something
			if (row == ast[i].printRow)
			{
				// We will print temporaray data in a stingstream
				std::ostringstream oss;

				// Dpeneding on the number of children, the output will be different
				// 0 = Leaf, 1= Unary operator, 2 = binary operator

				// 0 children, leaf, variable/condition/terminal
				if (NumberOfChildren::zero == ast[i].numberOfChildren)
				{
					// Name of Token (ID or IDNOT), input terminal/variable name, value, evaluated or not
					oss << tokenToCharP(ast[i].tokenWithAttribute.token) << ' ' << ast[i].tokenWithAttribute.inputTerminalSymbol
						<< " (" << ((ast[i].value) ? 1 : 0) << ((ast[i].notEvaluated) ? "x" : "") << ')';
				}
				// 1 child, unary operator NOT
				if (NumberOfChildren::one == ast[i].numberOfChildren)
				{
					// Name of Token (NOT), reference line of child, value, evaluated or not
					oss << tokenToCharP(ast[i].tokenWithAttribute.token) << ' ' << ast[ast[i].childLeftID].printRow
						<< " (" << ((ast[i].value) ? 1 : 0) << ((ast[i].notEvaluated) ? "x" : "") << ')';
				}
				// 2 children, binary operator (OR, XOR, AND) 
				if (NumberOfChildren::two == ast[i].numberOfChildren)
				{
					// Name of Token (NOT), reference line of child left, reference line of child right, value, evaluated or not
					oss << tokenToCharP(ast[i].tokenWithAttribute.token) << ' ' << ast[ast[i].childLeftID].printRow << ',' << ast[ast[i].childRightID].printRow
						<< " (" << ((ast[i].value) ? 1 : 0) << ((ast[i].notEvaluated) ? "x" : "") << ')';
				}
				// Now convert the temporary stringstream to a temporaray std::string
				const std::string strTemp{ oss.str() };
				// Calculate the last column  of the output. Lokk for the greatest value in case that there will
				// be 2 strings in the same line
				const uint lastPosition{ (tab * ast[i].level) + narrow_cast<uint>(strTemp.size()) + 1 };
				// Prevent buffer overflow
				if (lastPosition < maxBufSize)
				{
					// If we have a new biggest column
					if (lastPosition > outputLastPosition)
					{
						// Then remeber that
						outputLastPosition = lastPosition;
					}
					// Copy the temporary string to the output buffer (that is full of spaces) to the calculated column . 
					memcpy(&out[0] + (static_cast<ull>(tab) * static_cast<ull>(ast[i].level)), strTemp.c_str(), strTemp.size());
				}
			}
		}
		// terminate the output buffer with a C-String terminator  0
		out[outputLastPosition] = 0;
		// And put it to the screen and/or to a file
		os << std::setw(6) << row << ' ' <<& out[0] << '\n';
	}
}



// After the AST has been constrcuted, seveal properties will be set for all nodes
void VirtualMachineForAST::calculateAstProperties()
{
	const uint rootIndex{ ast.rbegin()->ownID };	// Start with root
	calculateAstPropertiesRecursive(rootIndex);		// And traverse the tree
}


// After all nodes have been added to the AST, some properties of the nodes
// will be calculated and set.
// parentID		This is a link to the parent node
// level		for printing the AST, this is the level or deepness of the node in the tree
// printRow		In which row this node shall be printed
//
// Depth First Traversal, Preorder (Root, Left, Right)
void VirtualMachineForAST::calculateAstPropertiesRecursive(uint index)
{
	// If not yet done, compact the symbol table
	symbolTable.compact();

	// All ast nodes will get a level. The level is the deepness in the tree
	// So th root is on level 0, the first children are on level 1, the grand children are on level 2 and so on 
	ast[index].level = level;

	// If this node is a leaf. Only a condition/variable/terminal can be a leaf
	if (NumberOfChildren::zero == ast[index].numberOfChildren)
	{
		// To spped up things later, we store the mask for reading the correct source input value
		ast[index].tokenWithAttribute.sourceMask = bitMask[symbolTable.normalizedInput[ast[index].tokenWithAttribute.sourceIndex]];
		// And we store the name of the condition/variable/terminal
		ast[index].tokenWithAttribute.inputSymbolLowerCase = narrow_cast<cchar>(std::tolower(ast[index].tokenWithAttribute.inputTerminalSymbol));
		// Where to print. The leave is the master for determining row. Everything else is referenced to this row
		ast[index].printRow = row;

		// The next node will be 3 rows below
		++row;
		++row;
	}

	// A node with only one child, e.g. NOT
	else if (NumberOfChildren::one == ast[index].numberOfChildren)
	{
		// Make the linkage to the parent node for our child
		// So, our child here, will recieve our own ID as parent ID
		ast[ast[index].childLeftID].parentID = ast[index].ownID;

		// We will go down one level, recursive call
		++level;
		calculateAstPropertiesRecursive(ast[index].childLeftID);
		// And we are back from below, Reset level
		--level;
		// And this node will be printed one row below its child
		ast[index].printRow = ast[ast[index].childLeftID].printRow + 1;
	}

	// A node with 2 children, eg. Operation AND, XOR, OR
	else if (NumberOfChildren::two == ast[index].numberOfChildren)
	{
		// Make the linkage to the parent node for our children
		// So, our 2 children here, will recieve our own ID as parent ID
		ast[ast[index].childLeftID].parentID = ast[index].ownID;
		ast[ast[index].childRightID].parentID = ast[index].ownID;


		// We will go down one level, recursive call
		++level;
		calculateAstPropertiesRecursive(ast[index].childLeftID);
		calculateAstPropertiesRecursive(ast[index].childRightID);
		// And we are back from below, Reset level
		--level;
		// And we will print this node in the middle (and above) of our 2 children
		ast[index].printRow = (ast[ast[index].childLeftID].printRow + ast[ast[index].childRightID].printRow) / 2;
	} // else nothing
}


// This is the application of the virtual machine for the AST
// It takes a source value and uses the operations encoded in nodes and leave values
// and calculates a result.
//
// Depth First Traversal, Preorder (Root, Left, Right)
bool VirtualMachineForAST::evaluateTree(uint inputValue)
{
	sourceValue = inputValue;
	const uint rootIndex{ ast.rbegin()->ownID };
	evaluateTreeRecursive(rootIndex);
	return ast[rootIndex].value;
}

// Recursive FUnction for wvaluating the boolean value of a node
// Depth First Traversal, Preorder (Root, Left, Right)
void VirtualMachineForAST::evaluateTreeRecursive(uint index)
{
	// Get a reference to the node and the number of children
	AstNode& nodeAtIndex{ ast[index] };
	const NumberOfChildren numberOfChildren{ nodeAtIndex.numberOfChildren };

	// If the node has no children, then it is a leave, so, a condition/variable/terminal
	// And we just nee to read the value from the source input value
	if (NumberOfChildren::zero == numberOfChildren)
	{
		switch (nodeAtIndex.tokenWithAttribute.token)
		{
		// Can be the positive form
		case Token::ID:
			nodeAtIndex.value = (0U != (sourceValue & nodeAtIndex.tokenWithAttribute.sourceMask)); // Mask and copy source value
			nodeAtIndex.notEvaluated = false;   // Used for boolean short cut evaluation. This value will be used
			break;
		// Or the negated form
		case Token::IDNOT:
			nodeAtIndex.value = (0U == (sourceValue & nodeAtIndex.tokenWithAttribute.sourceMask));// Mask and copy negated source value
			nodeAtIndex.notEvaluated = false;	// Used for boolean short cut evaluation. This value will be used
			break;
		default:
			// Should never happen. Error. Set value to false and
			// boolean shortcut evaluation flag to true, meaning: don't use			nodeAtIndex.value = false;
			nodeAtIndex.notEvaluated = true;
			break;
#pragma warning(suppress: 4061)
		}
	}
	// This node has one child, e.g. this node is a END or a NOT or a closing bracket
	else if (NumberOfChildren::one == numberOfChildren)
	{
		evaluateTreeRecursive(nodeAtIndex.childLeftID);
		nodeAtIndex.notEvaluated = false;	// Used for boolean short cut evaluation. This value will be used
		switch (nodeAtIndex.tokenWithAttribute.token)
		{
		case Token::BCLOSE:  //fallthrough
		case Token::END:
			// Simply copy the value. No operation necessary
			nodeAtIndex.value = ast[nodeAtIndex.childLeftID].value;
			break;
		case Token::NOT:
			nodeAtIndex.value = !ast[nodeAtIndex.childLeftID].value;
			break;
		default:
			// Should never happen. Error. Set value to false and
			// boolean shortcut evaluation flag to true, meaning: don't use			nodeAtIndex.value = false;
			nodeAtIndex.notEvaluated = true;
			break;
#pragma warning(suppress: 4061)
		}
	}
	// This node has 2 children. It is a boolean operation: AND, XOR, Or
	else if (NumberOfChildren::two == numberOfChildren)
	{
		// Before we can do the calculation for this node, we need the result of our children
		evaluateTreeRecursive(nodeAtIndex.childLeftID);
		evaluateTreeRecursive(nodeAtIndex.childRightID);
		nodeAtIndex.notEvaluated = false;	// Used for boolean short cut evaluation. This value will be used

		// Perfrom the requested operation on the values of our children
		switch (nodeAtIndex.tokenWithAttribute.token)
		{
		case Token::AND:
			nodeAtIndex.value = ast[nodeAtIndex.childLeftID].value && ast[nodeAtIndex.childRightID].value;
			break;
		case Token::OR:
			nodeAtIndex.value = ast[nodeAtIndex.childLeftID].value || ast[nodeAtIndex.childRightID].value;
			break;
		case Token::XOR:
			nodeAtIndex.value = ast[nodeAtIndex.childLeftID].value ^ ast[nodeAtIndex.childRightID].value;
			break;

		default:
			// Should never happen. Error. Set value to false and
			// boolean shortcut evaluation flag to true, meaning: don't use
			nodeAtIndex.value = false;
			nodeAtIndex.notEvaluated = true;
			break;
#pragma warning(suppress: 4061)
		}
	} // else nothing


	// If boolean short cut evaluation is requested by program options
	if (programOption.option[ProgramOption::bse].optionSelected)
	{
		// Now we are checking for the boolean "Annihilation"
		// This is needed for boolean short cut evaluation
		// If the left side of the AND is FALSE, then no need to evaluate the right side
		// If the left side of the OR is TRUE, then no need to evaluate the right side
		bool isNotEvaluated{ false };
		// Boolean short circuit evaluation
		switch (nodeAtIndex.tokenWithAttribute.token)
		{
		case Token::AND:
			if (!ast[nodeAtIndex.childLeftID].value)
			{
				// Right side of tree is notEvaluated
				isNotEvaluated = true;
			}
			break;
		case Token::OR:
			if (ast[nodeAtIndex.childLeftID].value)
			{
				// Right side of tree is notEvaluated
				isNotEvaluated = true;
			}
			break;
		default:
			break;
#pragma warning(suppress: 4061)
		}
		// If there is a short cut, either for and or for or
		if (isNotEvaluated)
		{
			// Set the whole right subtree to "not evealuated" or "masked"
			setSubTreetoMasked(nodeAtIndex.childRightID);
		}
	}
}

// Traverse subtree in preorder and set all flags for boolean short cut evaluation
// to "do not evaluate" or "masked" or "deactivated" or whatever you wanna call it
void VirtualMachineForAST::setSubTreetoMasked(uint index)
{
	// Set flage
	ast[index].notEvaluated = true;

	// Traverse the tree
	if (NumberOfChildren::zero == ast[index].numberOfChildren) {}
	else if (NumberOfChildren ::one == ast[index].numberOfChildren)
	{
		setSubTreetoMasked(ast[index].childLeftID);
	}
	else if (NumberOfChildren::two == ast[index].numberOfChildren)
	{
		setSubTreetoMasked(ast[index].childLeftID);
		setSubTreetoMasked(ast[index].childRightID);
	}
}

