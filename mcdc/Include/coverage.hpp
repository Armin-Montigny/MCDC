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
#ifndef COVERAGE_HPP
#define COVERAGE_HPP

// Solving the 
//
// Unate Covering Problem UCP
//
// Set Cover Problem
//
// Additionally usage of Petricks Method for the resolution of a possible remaining cyclic core.
//
// As you can read about the coverage problems, they have the property ov being NP-Nomplete.
// NP is " "nondeterministic polynomial time" ang gives us a major headache in regards to
// computation time and memory space. However, I implemented a space and speed optimized
// solution, by not storing a complete matrics but only row and column vector with none
// empty cells. The whole stuff is somehow complex and not so easy to understand. I try 
// to do my best to explain everything.
//
// In this software the coverage resolution algoritm is used for 2 problems:
// 1. Matching prime implicants to minterms and find the minimum coverage sets including
// essential prime implicants
// 2. Find a minimum cover of MCDC testpairs to all input variables
//
// First the initial coverage elements are set, then it will be tried to reduce the number
// of elements. Essential columns will be identified. Then boolen implication will be used
// to calculate dominating columns and dminating rows.
// Rows/columns with essential values will be reduced. Also dominated columns and dominating
// rows will be eliminated. At the end the elements are completely reduced or a cyclic core
// is left that cannot be reduced further.
//
// If there is a cyclic core the Petricks Method is used to calculate all possible coverage
// sets. The resulting coverage sets are combined with the essential elements to form
// possible results. In case of several results, some mini heuristic is used to select
// exactly one minimum coverage set.
//
// More detailed explanation for the reduction of a prime implicant table.

// Example can be found in the internet everywhere.
//
// After the analysis and calculation of the prime implicants using The Quine and McCluskey
// tabluar method we will get the following Prime Impicant table for the boolean expression:
// ABCD + ABcD + ABcd + AbCD + AbCd + AbcD + Abcd + aBCD + aBCd + aBcD + aBcd + abCD + abCd
// (capital letter mean negated variables)
// Prime implicants:  AD  BD  CD  Ac  Bc  Ab  bC  aB  aC  
// Prime implicant table (What Primimplicant cover what Minterm)
//
//      AD BD CD Ac Bc Ab bC aB aC 
//   0   X  X  X
//   2   X  X     X  X
//   3            X  X
//   4   X     X        X  X
//   5                  X  X
//   6   X        X     X
//   7            X     X
//   8      X  X              X  X
//   9                        X  X
//  10      X        X        X
//  11               X        X
//  12         X           X     X
//  13                     X     X
//
// In the beginning we cannot find any essential prime implicant, An essential prime implicant
// would be a singular cross in one row, meaning this implicant would be needed always to 
// cover this minterm.
//
// Then we check for dominating columns. One column dominates the other if it has a cross
// at least at the same positions as the other, or more crosses. Mathematically we are using the
// set theory "inclusion". In case of proper inclusion or strict inclusion, the decision is 
// easy. The dominated column will be eliminated. Because the dominating column covers already
// more mintermsthan the dominated column. In case of none proper subsets, heuristic may be
// used to select a column to eliminate
//
// The next check is for dominating rows. But here we delete the dominated row, because the
// leftover results in more covers of the minterm.
// Applying row dominance we get as next table:

//    AD BD CD Ac Bc Ab bC aB aC
//   0   X  X  X
//   3            X  X
//   5                  X  X
//   7            X     X
//   9                        X  X
//  11               X        X
//  13                     X     X
//
// We deleted rows 2,4,6,10,12
// Reduction loop will be started again. And again we do not see a essential prime implicant.
// But we see dominating column AD. AD dmoniates BD and CD. Heuristic selects BC and CD to be eliminated
// Result:
//  AD Ac Bc Ab bC aB aC
//   0   X
//   3      X  X
//   5            X  X
//   7      X     X
//   9                  X  X
//  11         X        X
//  13               X     X
// 
// In the next reduction loop we check again for essential prime implicants and find AD.
// That means AD must be in all coverage sets. AS and all related columns and rows will be elimiated
// leading to:
// 
//     Ac Bc Ab bC aB aC 
//  3   X  X
//  5         X  X
//  7   X     X
//  9               X  X
// 11      X        X
// 13            X     X
//
// This cannot be reduced further. The result is called "cyclic core". That means, we will have
// more than one coverage sets, all to be combined with the essential prime implicant AD
//
// To resolve the cyclic core we will use Petricks Method
// 
// Every Prime Implicant will recieve a number, a index starting with 1. Then we add a P for Prime implicant
// before that index. Then we create an artificial boolean expression in conjunctive normal form (CNF) in that way, 
// thas the resulting CNF will cover all minterms:
// So Ac=P1 Bc=P2 Ab=P3 bC=P4 aB=P5 aC=P6
// We build the Maxterms for each row: 
// 3: P1+P2  5: P3+4  7: P1+P3  9: P5+P6  11: P2+P5  13:  P4+P6
// CNF:  PALL = (P1+P2)(P3+P4)(P1+P3)(P5+P6)(P2+P5)(P4+P6)
// If Pall = 1 then we found the coverage sets. We transorm this CNF to a DNF by using monotone laws of boolean algebra.
// Multiplying out (Distributive law) though simple concatenation of literals
// and applying the idempotence and Absorption laws by using STL-sets which are sorted and conatin unique elements
// More details can be found in the module "petrick.*pp"
//
// As a result we get many product terms (DNFs) which all would fullfill the coverage of the minterms
// We will select (the heuristic her is the shortes product terms) product terms and can build the minimum
// solution for the coverage. This is deterministic.
//
// So there are 2 minimum results: P1P4P5 and P2P3P6
//
// 1.:   Ac bC aB
// 2.:   Bc Ab aC
// Combined with the essential prime implicant AD we will get 2 equal cost minimum DNFs as solution of the coverage problem
//
//    1:   AD+Ac+aB+bC 
//    2:   AD+Ab+Bc+aC
//
// These are 2 possible minimum DNFs for the original boolean expression
// Please note, there are more coverage sets, because we selected one implicant while eliminating dominated columns
// But this does not give us a better or worse result.
//
// Eliminating rows and columsn before applying Petricks method will drastically reduce the runtime and memory
// consumption of the program.
//
// Many functions are existing, becuase we do not use a full table, but just use covering cells
// For ease of use we are using row vectors and column vectors with cross references to the adjectant columns and rows.
// Basically it is very simple, bit for the reader more compley than a 2 dimensional table
//
// In the below classes we use the term "table," to make the understanding a little bit easier

#include "types.hpp"
#include "cloption.hpp"

#include <string>
#include <functional>
#include <any>


using Index = uint;



// One cell in the table which is marked as covering
// This can be a part of a row or a column. Only cells with a cover will be stored
// No "empty" celss will be stored.
struct TableCell
{
	TableCell(Index cr) noexcept : crossReference(cr), dropped(false) {}
	// Cross reference to the column or the row, where this cell is related to
	// It is not the row or column. If this cell is part of a row vector, the the cross reference 
	// points to the respective column and vice versa
	Index crossReference;
	// During the reduction of the table, cells will be dropped. This fiels marks dropped cells
	bool dropped{ false };
};

// This is vector of cells. Can be cells for a row or cells for a column
using CellVector = std::vector<TableCell>;
using TableCellCIterator = CellVector::const_iterator;

// We want to give each Row Or Column (a cell vector) a header to store some additional information
// necessary to identify the row or column later and necessary for printing out meaningful information.
struct CellVectorHeader
{
	CellVectorHeader() : index(0U), textInfo(), userData() {}
	CellVectorHeader(uint i)  : index(i), textInfo(), userData() {}
	CellVectorHeader(uint i, std::string& s) : index(i), textInfo(s), userData() {}
	CellVectorHeader(uint i, std::string& s, std::any ud) : index(i), textInfo(s), userData(ud) {}

	// Some index ID
	uint index;
	// A text that describes the contents of the row or column
	std::string textInfo;
	// Any other data that you would like to store
	std::any userData;
};

// Since we will store the data in a set, we need a custom comperator function. The text is the sort criterium
struct CellVectorHeaderComparator
{
	bool operator ()(const CellVectorHeader& left, const CellVectorHeader& right) const noexcept {return (left.textInfo < right.textInfo);}
};


// This is the complete Table Cell Vector, so a row or a column
// consisting of the header and the cells
struct TableCellVector
{
	TableCellVector()  : cellVectorHeader(), cell(), dropped(false) {}
	TableCellVector(uint i)  : cellVectorHeader(i), cell(), dropped(false) {}
	TableCellVector(uint i, std::string& s) : cellVectorHeader(i, s), cell(), dropped(false) {}
	TableCellVector(uint i, std::string& s, std::any ud) : cellVectorHeader(i, s, ud), cell(), dropped(false) {}

	CellVectorHeader cellVectorHeader;	// Header
	CellVector cell;					// Vector of Cells
	bool dropped{ false };				// Indicator, if all cells in this Vecotr are dropped. Means, the complete row or column is dropped
};



// One solution of the set covering problem will be stored here. It is a set of the column headers
// Cells of the vector are not interesting any longer
using CellVectorHeaderSet = std::set<CellVectorHeader, CellVectorHeaderComparator>;

// The set cover problem may have mor than one result. In the example above
// 1.:   Ac bC aB
// 2.:   Bc Ab aC
// The Coverage result is a vector fo one or more CellVectorHeaderSet
using CoverageResult = std::vector<CellVectorHeaderSet>;



// If the TableCellVector (consiting of a header and cells) is a row,
// then the TableVector are all rows of the table
// if the TableCellVector (consiting of a header and cells) is a column,
// then the TableVector are all columns of the table
using TableVector = std::vector<TableCellVector>;
using TableVectorIterator = TableVector::iterator;


// Function pointers

// This is used to store a function, that wiil return true, if a column covers a row.
// Used in the initialization phase
using CheckForCoverFunction = std::function<bool(CellVectorHeader&, CellVectorHeader&)>;

// This is a heuristic best cost function. During reduction of the covering table, so for
// row and column dominance, it may happen, taht 2 compared rows or columns are same.
// In this case, the function that is stored her is called and returns either 0, or a positive number or a negative
// number. It is a from of left - right
using BestCostFunction = std::function <sint(const TableCellVector& left, const TableCellVector& right)>;



// Columns cover rows
// Find out minimum set of columns to cover all rows
class Coverage
{
public:

	// These are functions for filling the tables in the beginning of the process
	// Basically we are adding row and column headers here. Additioal data can be attached
	void addRow(uint rowInfoIndex) { std::string s(std::to_string(rowInfoIndex));  tableRows.emplace_back(TableCellVector(rowInfoIndex, s)); }
	void addRow(uint rowInfoIndex, std::string s) { tableRows.emplace_back(TableCellVector(rowInfoIndex, s)); }
	void addRow(uint rowInfoIndex, std::string s, std::any ud) { tableRows.emplace_back(TableCellVector(rowInfoIndex, s, ud)); }

	void addColumn(uint columnInfoIndex) { std::string s(std::to_string(columnInfoIndex)); tableColumns.emplace_back(TableCellVector(columnInfoIndex, s)); }
	void addColumn(uint columnInfoIndex, std::string s) { tableColumns.emplace_back(TableCellVector(columnInfoIndex, s)); }
	void addColumn(uint columnInfoIndex, std::string s, std::any ud) { tableColumns.emplace_back(TableCellVector(columnInfoIndex, s, ud)); }

	// After row and columns have been added, we need to inform the algorithm on the initial cover
	// This is a crossing point between rows and columns
	// Meaning what column entry covers what row entry.
	// There are 2 possibilities to do so:
	// 1.: Set  cover directly via   void setCellAsCover(Index row, Index column);
	// 2.: a.) Set a callback function, which can calculate the initial cover
	//     b.) Let the system call the callback function and set the initial coverage internally
	void setCellAsCover(Index row, Index column);

	void setCheckForCoverFunction(CheckForCoverFunction cfcf) { checkForCover = cfcf; }
	void checkAllCellsForCover();

	// Main working horse: Reduce the initial coverage table
	CoverageResult reduce(std::ostream& os);
	// And in case of simple tables, get the coverage result directly (Without reduction)
	CoverageResult getCoverageResultWithoutReduction();

	// Table rdeuctions will be done with calculating row and column dominance. This is done by checking implications
	// There may be the case when there is no 100% strict implication. Conrete: Rows or columns are equal to other rows or columns
	// This function can then help to decide, which to eliminate
	void setBestCostFunctionForColumn(BestCostFunction bcf) { bestCostCalculatorForEqualImplicationColumn = bcf; }
	void setBestCostFunctionForRow(BestCostFunction bcf) { bestCostCalculatorForEqualImplicationColumn = bcf; }

	// Shows, how many cells are still in the table and not reduced. Can be used as complexity metrics
	ulong countNotDroppedTableElements() noexcept;	
protected:

	void printTable(std::ostream& os);

	// Callback function: see above
	CheckForCoverFunction checkForCover;

	// Functions to reduce the initial table
	// If only one element in a column covers a row and no othe element from any other columns covers that row, the
	// the column is essential. It is a part of all coverage sets. This functions searches for such columns
	void findEssentialColumn();

	// For table reduction. If one row or column implies another row or column, then we can eliminate one of them
	bool checkForImplication(TableCellVector& first, TableCellVector& second);

	// Finds a dominating row or columns. The last parameter says, if the dominating or the dominated row shall be eliminated
	void findDominatingVector(TableVector& firstDimension, TableVector& secondDimension, const BestCostFunction& bcf, const bool dropOuterAndNotInner);
	// Specialising the above function for row or column
	void findDominatingColumn() { findDominatingVector(tableColumns, tableRows, bestCostCalculatorForEqualImplicationColumn, true); }
	void findDominatingRow() { findDominatingVector(tableRows, tableColumns, bestCostCalculatorForEqualImplicationRow, false); }

	// if a cell a complete row or complete column has been identified to be redundant, it will be marked as such
	// So, it will be dropped. But not yet elimintated. This function can be used for either a row or a column
	void dropTableCellVector(TableVector& firstDimension, TableVector& secondDimension, Index index);
	// Specialsing the above function for row or column
	void dropRow(Index row) { dropTableCellVector(tableRows, tableColumns, row); };
	void dropColumn(Index column) { dropTableCellVector(tableColumns, tableRows, column); };
	void dropColumnAndAllRowsInColumn(Index column);

	// if a cell a row or column is marked as redundant then it can be physically eliminated from the table
	// This function removes one row or one column from the table
	void eraseTableCellVector(TableVector& firstDimension, TableVector& secondDimension, Index index);
	// Specialising for row, column
	void eraseColumn(Index column) { eraseTableCellVector(tableColumns, tableRows, column); }
	void eraseRow(Index row) { eraseTableCellVector(tableRows, tableColumns, row); }

	// This function erases all marked redundant rows or columns from the table
	void eraseAllDroppedTableCellVectors(TableVector& firstDimension, TableVector& secondDimension);
	// Specialising for row or column
	void eraseAllDroppedRows() { eraseAllDroppedTableCellVectors(tableRows, tableColumns); }
	void eraseAllDroppedColumns() { eraseAllDroppedTableCellVectors(tableColumns, tableRows); }
	// Erase everything redundant
	void eraseAllDroppedCells() { eraseAllDroppedRows(); eraseAllDroppedColumns(); }

	// Check, if there are still entries in the table or if everything has be reduced
	bool isTableEmpty() noexcept { return (null<TableVector::size_type>() == tableRows.size()) || (null<TableVector::size_type>() == tableColumns.size());	}

	// As written above. In reality we will not store a table
	// We will store rows and columns
	TableVector tableRows;
	TableVector tableColumns;

	// This is one result of the reduction. The set with the essential columns
	CellVectorHeaderSet essentialColumn;
	// The complete coverage result
	CoverageResult coverageResult;

	// Result of Petricks method. One or more coverage results
	ProductTermVector coverageSet;

	// Call back function pointers for best cost calculation in case of double rows or columns
	BestCostFunction bestCostCalculatorForEqualImplicationColumn{};
	BestCostFunction bestCostCalculatorForEqualImplicationRow{};
};


// Special functions for setting the initial covering cells for prime implicant tables
extern bool checkCoverForCell(const CellVectorHeader& minTermRowHeader, CellVectorHeader& primeImplicantColumnHeader);

#endif