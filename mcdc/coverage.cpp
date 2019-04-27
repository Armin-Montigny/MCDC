#include "pch.h"
#include "coverage.hpp"
#include "petrick.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>

	// We will not use a full table here. Only the cells are stored. But that in way
	// that we can later easily operate on them
	// We create row and column vectrors for the cell
	// One cell is part of a row and part of a column




void Coverage::printTable(std::ostream& os)
{
	if (isTableEmpty())
	{
		os << "\nTable is empty\n\n";
	}
	else
	{

		// For formatting issues we need the biggest size of a column and row header
		// Get the biggest size / number of  characters of a column header
		TableVectorIterator maxTVI = std::max_element(tableColumns.begin(), tableColumns.end(), [](const TableCellVector &tcvLeft, const TableCellVector &tcvRight) noexcept {return tcvLeft.cellVectorHeader.textInfo.size() < tcvRight.cellVectorHeader.textInfo.size(); });
#pragma warning(suppress: 26489)
		const uint maxColumnHeaderLength = (maxTVI != tableColumns.end()) ? narrow_cast<uint>(maxTVI->cellVectorHeader.textInfo.size()): 1;
		// Get the biggest size / number of  characters of a column header
		maxTVI = std::max_element(tableRows.begin(), tableRows.end(), [](const TableCellVector &tcvLeft, const TableCellVector &tcvRight) noexcept {return tcvLeft.cellVectorHeader.textInfo.size() < tcvRight.cellVectorHeader.textInfo.size(); });
#pragma warning(suppress: 26489)
		const uint maxRowHeaderLength = (maxTVI != tableRows.end()) ? narrow_cast<uint>(maxTVI->cellVectorHeader.textInfo.size()) : 1;


		// Print Column Headers
		// Print spaces long enough for the row headers
		os << '\n' << std::setw(static_cast<std::streamsize>(maxRowHeaderLength) + 2) << ' ';
		for (uint c = 0; c < tableColumns.size(); ++c)
		{
			os << std::setw(maxColumnHeaderLength) << tableColumns[c].cellVectorHeader.textInfo << ' ';
		}
		os << '\n';


		// print rows
		for (TableCellVector tcvRow : tableRows)
		{
			// First print the row header
			os << std::setw(maxRowHeaderLength) << tcvRow.cellVectorHeader.textInfo << "  ";
			for (uint c = 0; c < tableColumns.size(); ++c)
			{
				bool found{ false };
				for (const TableCell& tc : tcvRow.cell)
				{
					if (tc.crossReference == c)
					{
						found = true;
						cchar mark{ ' ' };
						if (tc.dropped)
						{
							mark = '-';
						}
						else
						{
							mark = 'X';

						}
						os << std::setw(maxColumnHeaderLength) << mark << ' ';
					}
				}
				if (!found)
				{
					os << std::setw(maxColumnHeaderLength) << ' ' << ' ';
				}
			}
			os << '\n';
		}
		os << '\n';
	}
}


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
	
	

	// Drop a row or column. The first dimension is the defines what we want to drop
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

	void Coverage::findDominatingVector(TableVector &firstDimension, TableVector &secondDimension, BestCostFunction &bcf, const bool dropOuterAndNotInner)
	{
		std::set<Index> vectorsToDrop;
		for (Index outer = 0; outer < firstDimension.size(); ++outer)
		{
			if (!firstDimension[outer].dropped)
			{
				for (Index inner = outer + 1; inner < firstDimension.size(); ++inner)
				{
					if (!firstDimension[inner].dropped)
					{
						const bool outerImpliesInner = checkForImplication(firstDimension[outer], firstDimension[inner]);
						const bool innerImpliesOuter = checkForImplication(firstDimension[inner], firstDimension[outer]);
						sint bestCostFUnctionResult{ 0 };
						if (outerImpliesInner && outerImpliesInner && bcf)
						{
							bestCostFUnctionResult = bcf(firstDimension[outer], firstDimension[inner]);
						}
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
		for (const ColumnIndex &ci : vectorsToDrop)
		{
			dropTableCellVector(firstDimension, secondDimension, ci);
		}
	}

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


	ulong Coverage::countNotDroppedTableElements() noexcept
	{
		ulong counter{ 0 };
		for (TableCellVector &tcv : tableRows)
		{
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
				TableCellCIterator tci = std::find_if(second.cell.begin(), second.cell.end(), [first, riFirst](const TableCell &tc) { return tc.crossReference == first.cell[riFirst].crossReference; });
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
				TableCellCIterator tci = std::find_if(first.cell.begin(), first.cell.end(), [second, riSecond](const TableCell &tc) { return tc.crossReference == second.cell[riSecond].crossReference; });
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

	void Coverage::eraseTableCellVector(TableVector &firstDimension, TableVector &secondDimension, Index index)
	{

		// If the a column shall be deleted, then we will check its row elements
		// If the a row shall be deleted, then we will check its column elements

		// if their is a reference in the adjectant cell to our "index to delete", we delete the adjectant element as well

		// Go through the whole adjectant vector and check all entreies in it. So, go through all rows (or columns)
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

	void  Coverage::eraseAllDroppedTableCellVectors(TableVector &firstDimension, TableVector &secondDimension)
	{
		sint index = narrow_cast<sint>(firstDimension.size()) - 1;
		while (index >= 0)
		{
			if (firstDimension[static_cast<uint>(index)].dropped ||
				
				(null<CellVector::size_type>() == firstDimension[static_cast<uint>(index)].cell.size()))
			{
				eraseTableCellVector(firstDimension, secondDimension,static_cast<uint>(index));
			}
			--index;
		}
	}
	CoverageResult Coverage::getCoverageResultWithoutReduction()
	{
		// Compile Coverage Result
		coverageResult.clear();
		CellVectorHeaderSet cvhs;
		for (auto& th : tableColumns)
		{
			cvhs.insert(th.cellVectorHeader);
		}
		coverageResult.push_back(cvhs);
		return coverageResult;
	}


	
	CoverageResult Coverage::reduce(std::ostream& os, bool fullMode)
	{
		ulong oldNumberOfNoneDroppedCells{ 0 };
		ulong newNumberOfNoneDroppedCells{ countNotDroppedTableElements() };
		uint counter{ 1 };

		os << "------------------ Analysis of coverage and reduction\n\n";
		printTable(os);


		do
		{
			os << "\nReduction Loop " << counter++ << "-----------------------------\n\n";

			oldNumberOfNoneDroppedCells = newNumberOfNoneDroppedCells;


			if (fullMode && !isTableEmpty() )
			{

				os << "Checking for essential columns:       ";
				const ulong numberOfNoneDroppedCellsBeforeOperation = countNotDroppedTableElements();
				findEssentialColumn();
				if (countNotDroppedTableElements() == numberOfNoneDroppedCellsBeforeOperation)
				{
					os << "Nothing new found.\n";
				}
				else
				{
					os << "Found!  Essential Columns now:    ";
					for (CellVectorHeader columnHeader : essentialColumn)
					{
						os << columnHeader.textInfo << ' ';
					}
					os << '\n';

					printTable(os);
					os << "\nCompacting . . .\n";
					eraseAllDroppedCells();
					printTable(os);
	
				}
			}
	
			if (!isTableEmpty())
			{
				os << "Checking for dominating columns:      ";
				const ulong numberOfNoneDroppedCellsBeforeOperation = countNotDroppedTableElements();
				findDominatingColumn();
				if (countNotDroppedTableElements() == numberOfNoneDroppedCellsBeforeOperation)
				{
					os << "Nothing new found.\n";
				}
				else
				{
					os << "Found!  \n";

					printTable(os);
					os << "\nCompacting . . .\n";
					eraseAllDroppedCells();
					printTable(os);
				}
			}

			if (fullMode && !isTableEmpty())
			{
				os << "Checking for dominating rows:         ";
				const ulong numberOfNoneDroppedCellsBeforeOperation = countNotDroppedTableElements();
				findDominatingRow();
				if (countNotDroppedTableElements() == numberOfNoneDroppedCellsBeforeOperation)
				{
					os << "Nothing new found.\n";
				}
				else
				{
					os << "Found!  \n";

					printTable(os);
					os << "\nCompacting . . .\n";
					eraseAllDroppedCells();
					printTable(os);
				}
			}
			newNumberOfNoneDroppedCells = countNotDroppedTableElements();

		} while ((null<ulong>() != newNumberOfNoneDroppedCells) && (newNumberOfNoneDroppedCells != oldNumberOfNoneDroppedCells));

		
		//Petricks method
		if (null<ulong>() != newNumberOfNoneDroppedCells)
		{
			os << "\n\n------------------ Could not reduce Tables further.   Cyclic Core Left\n";
			printTable(os);
			os << "------------------ Using Petricks Method to Calculate Coverage Sets\n";


			// Initialize Input for Petricks method
			// Build a CNF
			CNF cnf;
			// Fill maxterm with Primimplicnats that cover a minterm
			// Iterate over rows
			for (Index indexRow = 0; indexRow < tableRows.size(); ++indexRow)
			{
				if (!tableRows[indexRow].dropped)
				{
					DNF dnf;
					for (Index indexColumn = 0; indexColumn < tableRows[indexRow].cell.size(); ++indexColumn)
					{
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
				// First add the essential columns
				if (essentialColumn.size() > 0)
				{
					cvhs.insert(essentialColumn.begin(), essentialColumn.end());
				}
				for (const BooleanVariable b : coverageSet[i])
				{
					cvhs.insert(tableColumns[b].cellVectorHeader);
				}
				coverageResult.push_back(cvhs);
			}
		}
		else
		{
			if (essentialColumn.size() > 0)
			{
				coverageResult.push_back(essentialColumn);
			}
		}



		os << "\n\n------------------ Result of Coverage Analysis\n\n";

		const uint numberOfEssentialColumns = narrow_cast<uint>(essentialColumn.size());
		const uint numberOfCoverageSets = narrow_cast<uint>(coverageSet.size());
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
					const CellVectorHeader& cvh = tableColumns[b].cellVectorHeader;
					//for (const CellVectorHeader & cvh : coverageResult[i])
					{
						os << cvh.textInfo << ' ';
					}
					
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




	void Coverage::checkAllCellsForCover()
	{
		for (uint r = 0; r < tableRows.size(); ++r)
		{
			for (uint c = 0; c < tableColumns.size(); ++c)
			{
				if (checkForCover(tableRows[r].cellVectorHeader, tableColumns[c].cellVectorHeader))
				{
					setCellAsCover(r,c);
				}
			}
		}
	}





