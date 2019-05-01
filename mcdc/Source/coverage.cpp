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

#include "coverage.hpp"
#include "petrick.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>



// Print the table that shows, which elements in a column cover what elements in a row
// Will also show dropped (redundant) elements
void Coverage::printTable(std::ostream& os)
{
	// If reduction table is empty, then do nothing
	if (isTableEmpty())
	{
		os << "\nTable is empty\n\n";
	}
	else
	{

		// For formatting issues we need the biggest size of a column and row header
		// Get the biggest size / number of  characters of a column header
		TableVectorIterator maxTVI{ std::max_element(tableColumns.begin(), tableColumns.end(), [](const TableCellVector & tcvLeft, const TableCellVector & tcvRight) noexcept {return tcvLeft.cellVectorHeader.textInfo.size() < tcvRight.cellVectorHeader.textInfo.size(); }) };
#pragma warning(suppress: 26489)
		const uint maxColumnHeaderLength{ (maxTVI != tableColumns.end()) ? narrow_cast<uint>(maxTVI->cellVectorHeader.textInfo.size()) : 1 };
		// Get the biggest size / number of  characters of a column header
		maxTVI = std::max_element(tableRows.begin(), tableRows.end(), [](const TableCellVector &tcvLeft, const TableCellVector &tcvRight) noexcept {return tcvLeft.cellVectorHeader.textInfo.size() < tcvRight.cellVectorHeader.textInfo.size(); });
#pragma warning(suppress: 26489)
		const uint maxRowHeaderLength{ (maxTVI != tableRows.end()) ? narrow_cast<uint>(maxTVI->cellVectorHeader.textInfo.size()) : 1 };


		// Print Column Headers
		// Print spaces long enough for the row headers
		os << '\n' << std::setw(static_cast<std::streamsize>(maxRowHeaderLength) + 2) << ' ';
		for (uint c = 0; c < tableColumns.size(); ++c)
		{
			os << std::setw(maxColumnHeaderLength) << tableColumns[c].cellVectorHeader.textInfo << ' ';
		}
		os << '\n';


		// Print complete rows
		for (TableCellVector tcvRow : tableRows)
		{
			// First print the row header
			os << std::setw(maxRowHeaderLength) << tcvRow.cellVectorHeader.textInfo << "  ";
			// Now the rest of the row. X marks coverage, '-' makrks a dropped/redundant cell
			for (uint c = 0; c < tableColumns.size(); ++c)
			{
				// There are 3 states: Coveraged, Dropped, or Nothing
				bool found{ false }; // Checks, if there is/was a coverage 
				for (const TableCell& tc : tcvRow.cell)
				{
					// COverage/dorpped coverage existing?
					if (tc.crossReference == c)
					{
						found = true; // Remeneber that COverage/dorpped coverage existing?
						// Mark to print
						cchar mark{ ' ' };
						// Dropped or coverage
						if (tc.dropped)
						{
							mark = '-';
						}
						else
						{
							mark = 'X';

						}
						// And print . . .
						os << std::setw(maxColumnHeaderLength) << mark << ' ';
					}
				}
				if (!found)
				{
					// No coverage and not dropped. So nothing
					os << std::setw(maxColumnHeaderLength) << ' ' << ' ';
				}
			}
			os << '\n';
		}
		os << '\n';
	}
}

// This routine will be called in the very beginning. It will be used to initialize the coverage table with first values
// Mark a specific row and column as a Cover
void Coverage::setCellAsCover(Index row, Index column)
{
	// Boundary Check
	if (row < tableRows.size() && column < tableColumns.size())
	{
		// Add a cell to the table row vector and the table column vector.
		
		// Do not add double columns in the row vector. Search, if that column is already existing
		if (tableRows[row].cell.end() == std::find_if(tableRows[row].cell.begin(), tableRows[row].cell.end(), [column](const TableCell& tc) {return tc.crossReference == column;}))
		{
			// Column did not yet exist. Add new cell to the row vector with this column
			tableRows[row].cell.emplace_back(TableCell(column));
		}
		// Do not add doubles rows to the column vector. Search, if that row is already existing
		if (tableColumns[column].cell.end() == std::find_if(tableColumns[column].cell.begin(), tableColumns[column].cell.end(), [row](const TableCell& tc) {return tc.crossReference == row; }))
		{
			// Row did not yet exist. Add new cell to the column vector with this row
			tableColumns[column].cell.emplace_back(TableCell(row));
		}
	}
}


// If only one element in a column covers a row, then this column is essential
// It must be part of all coverage sets
// Try to find such column. If so, store it and delete related columns and rows
void Coverage::findEssentialColumn()
{
	// Check all rows, if it is covered by one column only
	for (TableCellVector& tcv : tableRows)
	{
		// Do only something, if the row is still active
		if (!tcv.dropped)
		{
			// Counter for columns that cover that row
			uint columnCoverageCounter{ 0 };
			// Here we store the last evaluated column in the row vector. This value is taken from the
			// crossReference entry that denotes the column
			Index lastEvaluatedNotDroppedColumn{ 0 };
			// Iterate through all table cells for this row
			for (const TableCell &tc : tcv.cell)
			{
				// Only if cell is still active
				if (!tc.dropped)
				{
					// Remember the column. The crossReference of a row points to the column
					lastEvaluatedNotDroppedColumn = tc.crossReference;
					// We found a new covering column. Count it
					++columnCoverageCounter;
					
				}
			}
			// If there is only 1 Covering Column, then the column is essential. It has to be in all coverage sets
			if (1 == columnCoverageCounter)
			{
				// Store essential column
				essentialColumn.insert(tableColumns[lastEvaluatedNotDroppedColumn].cellVectorHeader);
				// Drop covered rows and columns
				dropColumnAndAllRowsInColumn(lastEvaluatedNotDroppedColumn);
			}
		}
	}
}
	
	
// Redundant 
// Drop a row or column. The first dimension is what defines what we want to drop
void Coverage::dropTableCellVector(TableVector &firstDimension, TableVector &secondDimension, Index index)
{
	// Drop the complete row or column
	firstDimension[index].dropped = true;
	// Drop all cells in this row or column
	for (TableCell& tc : firstDimension[index].cell)
	{
		// Drop one cell in the row or column vector
		tc.dropped = true;
		// Now go through the 2nd dimension (the column or the row) that was reference by the currently evaluated cell
		for (TableCell& tc2 : secondDimension[tc.crossReference].cell)
		{
			// if the adjectant cell references the index, then drop it as well
			if (tc2.crossReference == index)
			{
				tc2.dropped = true;
			}
		}
	}
}


// This function tries to eliminate redundant dominiating columns or rows
// It checks if one vector dominates the other. If both are equal, a best cost function is used, to determine what vector
// should be eliminated. There is an additional parameter, if the dominating or the dominated vector shall be deleted.
// The the case of primeimplcante tables, the dominated column will be deleted, but for rows, the dominating row wiill be deleted
// SInce we are the same dtructures for rows and colums, sometimes the firstDimension is a row and sometimes a column
void Coverage::findDominatingVector(TableVector &firstDimension, TableVector &secondDimension, const BestCostFunction &bcf, const bool dropOuterAndNotInner)
{
	// Here we will collect which vectors shall be dropped (can be rows, can be columns)
	// The actual "dropping" will be done afterwards
	std::set<Index> vectorsToDrop;
	// Go through all rows/columns
	for (Index outer = 0; outer < firstDimension.size(); ++outer)
	{
		// if not already dropped. Otherweise skip
		if (!firstDimension[outer].dropped)
		{
			// Now compare with all other rows/columns
			for (Index inner = outer + 1; inner < firstDimension.size(); ++inner)
			{
				// And again, if the next row/column is still active
				if (!firstDimension[inner].dropped)
				{
					// CHeck, who implies whowm. Maybe right to left or left to right
					const bool outerImpliesInner{ checkForImplication(firstDimension[outer], firstDimension[inner]) };
					const bool innerImpliesOuter{ checkForImplication(firstDimension[inner], firstDimension[outer])};
					sint bestCostFUnctionResult{ 0 };
					// If both vectors are equal and a best cost function is existing then
					if (outerImpliesInner && innerImpliesOuter && bcf)
					{
						// Calculate the best cost
						bestCostFUnctionResult = bcf(firstDimension[outer], firstDimension[inner]);
					}
					// Depending on best cost function result and on flag, if dominating or dominated vector shall be deleted
					// Then drop either the cvector with the inner or outer index
					if ((bestCostFUnctionResult > 0) || ((bestCostFUnctionResult == 0) && ((dropOuterAndNotInner && outerImpliesInner) || (!dropOuterAndNotInner && !outerImpliesInner && innerImpliesOuter))))
					{
						vectorsToDrop.insert(inner);
					}
					if ((bestCostFUnctionResult < 0) || ((bestCostFUnctionResult == 0) && ((!dropOuterAndNotInner && outerImpliesInner) || (dropOuterAndNotInner && !outerImpliesInner && innerImpliesOuter))))
					{
						vectorsToDrop.insert(outer);
					}
				}
			}
		}
	}
	// And now do the actual dropping
	for (const ColumnIndex &ci : vectorsToDrop)
	{
		dropTableCellVector(firstDimension, secondDimension, ci);
	}
}

// Special case, after an essential column has been found. Drop the column
// with the essential column and all referenced rows in this coulmn
void Coverage::dropColumnAndAllRowsInColumn(Index column)
{
	// For all row entries in this column
	for (const TableCell& tcRow : tableColumns[column].cell)
	{
		// Drop the referenced rows
		dropRow(tcRow.crossReference);
	}
	// And finally drop the column
	dropColumn(column);
}

// Simple function, to check, if there are still dropped rows and columns in the table
ulong Coverage::countNotDroppedTableElements() noexcept
{
	ulong counter{ 0 };
	// Iterate over all rows
	for (TableCellVector &tcv : tableRows)
	{
		// And count non dropped elements
		if (!tcv.dropped)
		for (const TableCell &tc : tcv.cell)
		{
			if (!tc.dropped)
			{
				++counter;
			}
		}
	}
	return counter;
}

// One vector may imply the other vector. One row/coulmn may imply the other row/column
bool  Coverage::checkForImplication( TableCellVector &first,  TableCellVector &second)
{
	// We assume Implication
	bool result{ true };
	// Now we iterate over the cells of a complete row or column
	for (Index riFirst = 0; riFirst < first.cell.size() && result; ++riFirst)
	{
		// If this has been dropped, then we need to check the other side 
		// This (left) dropped and other side (second) is covering (and not dropped) means: No implication   
		if (first.cell[riFirst].dropped)
		{
			TableCellCIterator tci{ std::find_if(second.cell.begin(), second.cell.end(), [first, riFirst](const TableCell & tc) { return tc.crossReference == first.cell[riFirst].crossReference; }) };
			if ((second.cell.end() != tci) && (!tci->dropped))
			{
				result = false;
			}
		}
	}

	// Do next loop only if implication has not yet been evaluated as false
	// Check from the viewpoint of the parameter second
	for (Index riSecond = 0; riSecond < second.cell.size() && result; ++riSecond)
	{
		// if the right side (second) is not dropped
		if (!second.cell[riSecond].dropped)
		{
			// Check the left side (first)
			TableCellCIterator tci{ std::find_if(first.cell.begin(), first.cell.end(), [second, riSecond](const TableCell & tc) { return tc.crossReference == second.cell[riSecond].crossReference; }) };
			// If there is nothing in that row or column and if the first is dropped
			// Then no implication
			if ((first.cell.end() == tci) || (tci->dropped))
			{
				result = false;
			}
		}
	}
	return result;
}


// Physically delete or row or a column. Only one.
// Rows/columns can be dropped (marked as obsolete), but are then still part of the table
// This function will erase them from the vector
void Coverage::eraseTableCellVector(TableVector &firstDimension, TableVector &secondDimension, Index index)
{

	// If the a column shall be deleted, then we will check its row elements
	// If the a row shall be deleted, then we will check its column elements

	// if their is a reference in the adjectant cell to our "index to delete", we delete the adjectant element as well

	// Go through the whole adjectant vector and check all entries in it. So, go through all rows (or columns)
	for (Index indexSecond = 0; indexSecond < secondDimension.size(); ++indexSecond)
	{
		// Go throug all elements of this one vector
		// e.g. Check the columns in this row (Check the row in this column)
		for (Index indexCell = 0; indexCell < secondDimension[indexSecond].cell.size(); )
		{
			// If there is an entry wit the vector-index-to-delete as reference
			if (secondDimension[indexSecond].cell[indexCell].crossReference == index)
			{
				// then erase this entry
				secondDimension[indexSecond].cell.erase(secondDimension[indexSecond].cell.begin() + indexCell);
				// Do not increase the loop index
			}
			else
			{
				// Correction of cross reference values after erase
				// After an erase opration, all crossreferences that are > than the erased vector index
				// will be 1 to big and need to be decremented.
				if (secondDimension[indexSecond].cell[indexCell].crossReference > index)
				{
					--secondDimension[indexSecond].cell[indexCell].crossReference;
				}
				++indexCell;
			}
		}
	}
	firstDimension.erase(firstDimension.begin() + index);
}

// Go through all rows/columns in a table. Erase all rows/columns including contained cells
void  Coverage::eraseAllDroppedTableCellVectors(TableVector &firstDimension, TableVector &secondDimension)
{
	// Iterate over complete table
	TableVector::size_type index{ firstDimension.size() };
	if (null< TableVector::size_type>() < index)
	{
		do
		{
			--index;
			if (firstDimension[index].dropped ||
				(null<CellVector::size_type>() == firstDimension[index].cell.size()))
			{
				eraseTableCellVector(firstDimension, secondDimension, narrow_cast<Index>(index));
			}
		} while (index != 0);
	}
}

// If there is only one possible solution, then the reduction procedure may not be run. No need
CoverageResult Coverage::getCoverageResultWithoutReduction()
{
	// Compile Coverage Result. Simply use data as is and put in other data structure
	coverageResult.clear();
	CellVectorHeaderSet cvhs;
	for (const TableCellVector& tableColumn : tableColumns)
	{
		cvhs.insert(tableColumn.cellVectorHeader);
	}
	coverageResult.push_back(cvhs);
	return coverageResult;
}





// Solve the set cover / unate coverage problem
// For a given table, with element in a row that cover elements in a column
// there are often redundant elements. The tebale needs to be reduced, until a minimum
// coverage set can be found. Since the procedure may take very long (It is a np complete problem))
// some heuristc is used to eliminate certain elements. 
// FOr example: There may be 2 columns with identical elements. And we need to decide which one
// can be deleted. A best cost function can be used, or we will simply delete the 1st or the 2nd

CoverageResult Coverage::reduce(std::ostream& os)
{
	// We compare the number of dropped cell before an operation and after an operation
	// to detect, if something could be reduced or not
	ulong oldNumberOfNoneDroppedCells{ 0 };
	ulong newNumberOfNoneDroppedCells{ countNotDroppedTableElements() };

	// Simply count the reduction loops and display the number
	uint reductionLoopCounter{ 1 };

	// Show header and initial full table
	os << "------------------ Analysis of coverage and reduction\n\n";
	printTable(os);

	// Try to reduce the table iteratively, until there is nothing more to reduce
	do
	{
		// Begin of iterative reduction process
		os << "\nReduction Loop " << reductionLoopCounter++ << "-----------------------------\n\n";


		// Overall check, if something has been reduced in this reduction loop
		oldNumberOfNoneDroppedCells = newNumberOfNoneDroppedCells;

		// Operation 1
		// Try to find an esstial column
		if (!isTableEmpty())		// Sanity check. Operate only, if data is avaliable
		{
			os << "Checking for essential columns:       ";
			// Local check, if something could be reduced for this specific reduction operation
			const ulong numberOfNoneDroppedCellsBeforeOperation{ countNotDroppedTableElements() };

			findEssentialColumn();

			// Check, if something happened
			if (countNotDroppedTableElements() == numberOfNoneDroppedCellsBeforeOperation)
			{
				os << "Nothing new found.\n";
			}
			else
			{
				os << "Found!  Essential Columns now:    ";
				for (const CellVectorHeader& columnHeader : essentialColumn)
				{
					os << columnHeader.textInfo << ' ';
				}
				os << '\n';

				printTable(os);
				os << "\nCompacting . . .\n";

				// Reduce
				eraseAllDroppedCells();
				printTable(os);

			}
		}

		// Operation 2
		// Try to find a dominating column
		if (!isTableEmpty())		// Sanity check. Operate only, if data is avaliable
		{
			os << "Checking for dominating columns:      ";
			// Local check, if something could be reduced for this specific reduction operation
			const ulong numberOfNoneDroppedCellsBeforeOperation{ countNotDroppedTableElements() };

			findDominatingColumn();

			// Check, if something happened
			if (countNotDroppedTableElements() == numberOfNoneDroppedCellsBeforeOperation)
			{
				os << "Nothing new found.\n";
			}
			else
			{
				os << "Found!  \n";
				printTable(os);
				os << "\nCompacting . . .\n";

				// Reduce
				eraseAllDroppedCells();
				printTable(os);
			}
		}

		// Operation 3
		// Try to find a dominating row
		if (!isTableEmpty())		// Sanity check. Operate only, if data is avaliable
		{
			os << "Checking for dominating rows:         ";
			// Local check, if something could be reduced for this specific reduction operation
			const ulong numberOfNoneDroppedCellsBeforeOperation{ countNotDroppedTableElements() };

			findDominatingRow();

			// Check, if something happened
			if (countNotDroppedTableElements() == numberOfNoneDroppedCellsBeforeOperation)
			{
				os << "Nothing new found.\n";
			}
			else
			{
				os << "Found!  \n";

				printTable(os);
				os << "\nCompacting . . .\n";

				// Reduce
				eraseAllDroppedCells();
				printTable(os);
			}
		}
		// Could we reduce somethin?
		newNumberOfNoneDroppedCells = countNotDroppedTableElements();

	} while ((null<ulong>() != newNumberOfNoneDroppedCells) && (newNumberOfNoneDroppedCells != oldNumberOfNoneDroppedCells));


	// All conventional reduction operation performed
	// Now use Petricks methos in case of a cyclic core left
	if (null<ulong>() != newNumberOfNoneDroppedCells)
	{
		os << "\n\n------------------ Could not reduce Tables further.   Cyclic Core Left\n";
		printTable(os);
		os << "------------------ Using Petricks Method to Calculate Coverage Sets\n";


		// Initialize Input for Petricks method
		// Build a conjunctive normal form CNF
		CNF cnf;
		// Fill maxterm with Primimplicnats that cover a minterm
		// Iterate over rows
		for (Index indexRow = 0; indexRow < tableRows.size(); ++indexRow)
		{
			if (!tableRows[indexRow].dropped)
			{
				// A conjunctive form is a conjunction of disjunctive forms (product terms)
				DNF dnf;
				for (Index indexColumn = 0; indexColumn < tableRows[indexRow].cell.size(); ++indexColumn)
				{
					// Combine the index of columns that cover a row to a product term
					ProductTerm productTerm;
					productTerm.insert(tableRows[indexRow].cell[indexColumn].crossReference);
					dnf.insert(productTerm);
				}
				cnf.push_back(dnf);
			}
		}

		// Instantiate Petricks Method

		PetricksMethod petricksMethod;
		// And evaluate the result
		coverageSet = petricksMethod(cnf);

	}

	// Compile Coverage Result
	coverageResult.clear();

	// There are as many vector entries as coverage Sets
	// if there is more than one solution
	if (coverageSet.size() > 0)
	{
		for (uint i = 0; i < coverageSet.size(); ++i)
		{
			CellVectorHeaderSet cvhs;
			// The essential columns will always be a part of the solution.
			// First add the essential columns
			if (essentialColumn.size() > 0)
			{
				cvhs.insert(essentialColumn.begin(), essentialColumn.end());
			}
			// Then add the rest of the coverage set
			for (const BooleanVariable b : coverageSet[i])
			{
				cvhs.insert(tableColumns[b].cellVectorHeader);
			}
			coverageResult.push_back(cvhs);
		}
	}
	else
	{
		// There was only one solution
		// The essential columns will always be a part of the solution.
		// So, if there is any, then add it
		if (essentialColumn.size() > 0)
		{
			coverageResult.push_back(essentialColumn);
		}
	}


	// Printout the result
	os << "\n\n------------------ Result of Coverage Analysis\n\n";

	const uint numberOfEssentialColumns{ narrow_cast<uint>(essentialColumn.size()) };
	const uint numberOfCoverageSets { narrow_cast<uint>(coverageSet.size()) };
	if (numberOfEssentialColumns > 0)
	{
		os << "There are  " << numberOfEssentialColumns << "  essential columns:  ";
		for (CellVectorHeader columnHeader : essentialColumn)
		{
			os << columnHeader.textInfo << ' ';
		}
		os << "\n\n";
	}

	os << "There are  " << numberOfCoverageSets << "  Coverage Sets:\n";
	{
		
		for (uint i=0; i < coverageSet.size(); ++i)
		{
			os << std::right << std::setw(5) << (1+i) << ":   ";
			//const ProductTerm &pt = coverageSet[i];
			for (const BooleanVariable b : coverageSet[i])
			{
				const CellVectorHeader& cvh{ tableColumns[b].cellVectorHeader };
				os << cvh.textInfo << ' ';
			}
			os << '\n';
		}
	}

	if ((numberOfEssentialColumns > 0) && (numberOfCoverageSets > 0))
	{
		os << "\nOverall Reduction Results (" << numberOfCoverageSets << ") :\n";
		for (uint i = 0; i < coverageResult.size(); ++i)
		{
			os << std::right << std::setw(5) << (1 + i) << ":   ";
			for (const CellVectorHeader& cvh : coverageResult[i])
			{
				os << cvh.textInfo << ' ';
			}
			os << '\n';
		}
	}

	os << '\n';
	return coverageResult;
}



// This function will set cells as a cover during the initialization of the table
// So this will be called in the very beginning, before any reduction will take place
// A call back function will be used to do the real work
// In essence, this functions simply iterates over rows and columns and then calls
// the call back functions, which cehscks, if the cell is a cover. The the
// cell will be marked as having a cover
void Coverage::checkAllCellsForCover()
{
	// Iterate over all rows
	for (uint row = 0; row < tableRows.size(); ++row)
	{
		// Iterate over all columns in this row
		for (uint column = 0; column < tableColumns.size(); ++column)
		{
			// Call callback function, and check, if this cell chould be marked as a cover
			if (checkForCover(tableRows[row].cellVectorHeader, tableColumns[column].cellVectorHeader))
			{
				// Mark cell
				setCellAsCover(row, column);
			}
		}
	}
}





