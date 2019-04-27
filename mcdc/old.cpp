#include "pch.h"



//std::string source("a(b^(c+d))+ac+ad"); //== 	std::string source("ab+ac+ad");

//std::string source("ab+ac+ad");
//std::string source("a+bc");

//std::string source("a(b+c)");
//std::string source("ab+ac");
//std::string source("abcdefghij");
//std::string source("a+b+c+d+e+f+g+h+i+j");
//std::string source("ABCDEFGHI");
//std::string source("ab+ac+bc");
//std::string source("abef + cdf + bcd + ab + (ab + bc)(ce + ef)(def + ade) + a + bcdef ^ ABCDEF + abcdef(fca(bd + cef ^ ac & !be)(c + d + e + f)(((a + b + c))))");
//std::string source("(ade^CDE(F + A) + bdf(CD + EF)g + a(b(c(d(eh + fg) ^ ab) ^ bc)(c + dg)) + abd + cf + ae + cdfg + af + acfh + bcf + abc ^ def + c(d(e(f + abc + def + abh))) || ac + cde ^ abc(ab + ef)!(cd + ef) + ((a(b(c(d + e))))))");
//std::string source("ABCD + ABcD + ABcd + AbCD + AbCd + AbcD + Abcd + aBCD + aBCd + aBcD + aBcd + abCD + abCd");  // cyclic core
//std::string source("ab+ac");
//std::string source("a+b");
//std::string source("");
//std::string source("a+b+c");


#if 0


class TeeStream : public std::ostream
{
	struct TeeStreamBuffer : std::streambuf
	{
		TeeStreamBuffer(std::streambuf* sb1, std::streambuf* sb2, std::streambuf* sb3 = nullptr) : streamBufferPath1(sb1), streamBufferPath2(sb2), streamBufferPath3(sb3) {}

		TeeStreamBuffer() : streamBufferPath1(nullptr), streamBufferPath2(nullptr), streamBufferPath3(nullptr) {}
		void setStreamBuffer(std::streambuf* sb1, std::streambuf* sb2, std::streambuf* sb3 = nullptr) { streamBufferPath1 = sb1; streamBufferPath2 = sb2; streamBufferPath3 = sb3; }

		sint overflow(sint c);
		sint sync();

		std::streambuf* streamBufferPath1;
		std::streambuf* streamBufferPath2;
		std::streambuf* streamBufferPath3;
	};

public:
	TeeStream(std::string &filename, bool appendMode) : std::ostream(&teeStreamBuffer), fileOut1(), fileOut2(), teeStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf())
	{
		fileOut1.open(filename, (appendMode ? IosModeAppend : IosModeOverwrite));
	}
	TeeStream(std::string &filename1, bool appendMode1, std::string &filename2, bool appendMode2) : std::ostream(&teeStreamBuffer), fileOut1(), fileOut2(), teeStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf(), fileOut2.rdbuf())
	{
		fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
	}
	TeeStream(const cchar *filename, bool appendMode) : std::ostream(&teeStreamBuffer), fileOut1(), fileOut2(), teeStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf())
	{
		fileOut1.open(filename, (appendMode ? IosModeAppend : IosModeOverwrite));
	}
	TeeStream(const cchar *filename1, bool appendMode1, const cchar *filename2, bool appendMode2) : std::ostream(&teeStreamBuffer), fileOut1(), fileOut2(), teeStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf(), fileOut2.rdbuf())
	{
		fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
	}


	TeeStream() : std::ostream(&teeStreamBuffer), fileOut1(), fileOut2(), teeStreamBuffer() {}

	void setFileAndMode(std::string &filename, bool appendMode)
	{
		fileOut1.open(filename, (appendMode ? IosModeAppend : IosModeOverwrite));
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf());
	}
	void setFileAndMode(std::string &filename1, bool appendMode1, std::string &filename2, bool appendMode2)
	{
		fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf(), fileOut2.rdbuf());
	}

	void setFileAndMode(const cchar *filename, bool appendMode)
	{
		fileOut1.open(filename, (appendMode ? IosModeAppend : IosModeOverwrite));
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf());
	}
	void setFileAndMode(const cchar *filename1, bool appendMode1, const cchar *filename2, bool appendMode2)
	{
		fileOut1.open(filename1, (appendMode1 ? IosModeAppend : IosModeOverwrite));
		fileOut2.open(filename2, (appendMode2 ? IosModeAppend : IosModeOverwrite));
		teeStreamBuffer.setStreamBuffer(std::cout.rdbuf(), fileOut1.rdbuf(), fileOut2.rdbuf());
	}


	std::ofstream fileOut1;
	std::ofstream fileOut2;
	TeeStreamBuffer teeStreamBuffer;
};








primeimplicnatselection

HPP


#if 0

// The row of of a qm table for one bit count
struct QMTableRowBase
{
	MinTermNumber maskForEliminatedCondition{ 0 };
	MintermSet mintermSet{};

	std::string toString(const SymbolTable &symbolTable) const;
	NumberOfBitsCountType numberOfTrues() const;
};

struct QMTableRow : public QMTableRowBase
{
	mutable bool matchFound{ false };
};





struct QMTableRowCompare
{
	bool operator() (const QMTableRowBase &lhs, const QMTableRowBase &rhs)
	{
		if ((*lhs.mintermSet.rbegin() == *rhs.mintermSet.rbegin()) &&
			(*lhs.mintermSet.begin() == *rhs.mintermSet.begin()) &&
			(lhs.maskForEliminatedCondition == rhs.maskForEliminatedCondition))
		{
			return false;
		}
		return lhs.mintermSet < rhs.mintermSet;
	}
};

//QM Table with rows for one bit _countof
//using QMTableForOneBitCount = std::vector<QMTableRow>;
using QMTableForOneBitCount = std::set<QMTableRow, QMTableRowCompare>;
using QMTableForOneBitCountIterator = QMTableForOneBitCount::iterator;


// On QM Table sorted by number of bits
// Each bit count number has a table with rows
using QMTable = std::map<uint8_t, QMTableForOneBitCount>;
using QMTableIterator = QMTable::iterator;


// All QM minimization tables
using QMMinimizeTable = std::vector<QMTable>;



using QMPrimeImplicantSet = std::set<QMTableRowBase, QMTableRowCompare>;

using QMPrimeImplicantTable = std::map<MinTermNumber, QMPrimeImplicantSet>;

extern void printMinizeTable(const QMMinimizeTable &qmMinimizeTable, const SymbolTable &symbolTable);




struct PrimeImplicanTableCell
{
	PrimeImplicanTableCell() {}
	PrimeImplicanTableCell(MinTermNumber cr) : crossReference(cr) {}

	MinTermNumber crossReference{ 0 };
	mutable bool dropped{ false };
};


//using PrimeImplicant = QMTableRowBase;


class PrimeImplicantSelection
{
public:

	using RowColumnEntry = std::vector<PrimeImplicanTableCell>;
	using RowColumnEntryIterator = RowColumnEntry::iterator;


	using ColumnEntry = RowColumnEntry;
	using RowEntry = RowColumnEntry;

	struct Row
	{
		bool rowIsDisabled{ false };
		MinTermNumber minTerm{ 0 };
		RowEntry rowEntry;
	};

	struct Column
	{
		bool columnIsDisabled{ false };
		PrimeImplicantType primeImplicant;
		ColumnEntry columnEntry;
	};


	using TableRow = std::vector<Row>;
	using TableColumn = std::vector<Column>;

	using EssentialPrimeImplicant = std::set<PrimeImplicantType, PrimeImplicantTypeCompare>;
	using TableRowIterator = TableRow::iterator;

	void initiate(const MintermVector &mintermVector, const PrimeImplicantSet &qmPrimeImplicant, const SymbolTable &st);
	void reduce();


protected:
	void printTable();

	void identifyEssentialPrimeImplicant();
	bool checkForImplication(const RowColumnEntry &first, const RowColumnEntry &second);
	void findDominatingColumns();
	void findDominatingRows();

	void dropColumn(ColumnIndex columnIndex);
	void dropColumnAndAllRowsInColumn(ColumnIndex columnIndex);
	void dropRow(RowIndex rowIndex);
	ulong countActiveTableElements();

	void eraseColumn(ColumnIndex rowIndex);
	void eraseRow(RowIndex rowIndex);
	void eraseAllDroppedRows();
	void eraseAllDroppedColumns();

	TableRow tableRowForMinterm;
	TableColumn tableColumnForPrimeImplicant;

	EssentialPrimeImplicant essentialPrimeImplicant;


	SymbolTable symbolTable;

	void printEssentialPrimeImplicants();


	ConjunctiveNormalForm conjunctiveNormalForm;


};


#endif













#endif












#if 0





#if 0

VirtualMachineForAST ast2 = ast;
VirtualMachineForAST ast3 = ast;


printTruthTable(mv, mc.getSymbolTable());




uint counter = 0;

uint counterUniqueCauseMCDC = 0;
uint counterUniqueCauseMaskingMCDC = 0;
uint counterMaskingMCDC = 0;






std::set<uint> testSetUniqueCauseMCDC;
std::set<uint> testSetUniqueCauseMaskingMCDC;
std::set<uint> testSetMaskingMCDC;

uint maxLoop = mc.getSymbolTable().numberOfSymbols();

for (uint outer = 0; outer < static_cast<uint>((1 << (maxLoop)) - 1); ++outer)
{
	for (uint inner = outer + 1; inner < static_cast<uint>(1 << (maxLoop)); ++inner)
	{
#define FULL_OUTPUT 0
#define FULL_OUTPUT_SHOWALL 0

		ast.evaluateTree(outer);
		ast2.evaluateTree(inner);
		ast3 = ast ^ ast2;

#if FULL_OUTPUT_SHOWALL 
		std::cout << "\n--------------------------------------------------\n---------Outer " << outer << "\n\n";

		ast.printTree();
		std::cout << "\n---------Inner " << inner << "\n\n";
		ast2.printTree();

		std::cout << "\n--------------Influencing Tree\n\n";
		ast3.printTree();
		std::cout << '\n';
#endif


		auto[isMCDC, uniqueCauseMCDC, uniqueCauseMaskingMCDC, maskingMCDC, influencingCondition] = ast3.isMCDC();
		if (isMCDC)
		{
			if (uniqueCauseMCDC)
			{
				++counter;
				++counterUniqueCauseMCDC;
				//std::cout << std::setw(3)<< counter << "----->>> UNIQUE CAUSE MCDC for : '" << influencingCondition <<"'  " << std::setw(3) << counterUniqueCauseMCDC << "    Independence pair: " << outer << ' ' << inner << "\n\n\n";
#if FULL_OUTPUT
				std::cout << "---------Outer " << outer << "\n\n";

				ast.printTree();
				std::cout << "\n---------Inner " << inner << "\n\n";
				ast2.printTree();

				std::cout << "\n--------------Influencing Tree\n\n";
				ast3.printTree();
				std::cout << '\n';
#endif
				testSetUniqueCauseMCDC.insert(outer);
				testSetUniqueCauseMCDC.insert(inner);
				mcdc.add(McdcIndependencePair(McdcType::UniqueCause, outer, inner, influencingCondition));
			}
			if (uniqueCauseMaskingMCDC)
			{
				++counter;
				++counterUniqueCauseMaskingMCDC;

				//std::cout <<  std::setw(3) << counter << "----->>> UNIQUE CAUSE MASKING MCDC for : '" << influencingCondition << "'  " << std::setw(3) << counterUniqueCauseMaskingMCDC << "    Independence pair: " << outer << ' ' << inner << "\n\n\n";
#if FULL_OUTPUT
				std::cout << "---------Outer " << outer << "\n\n";

				ast.printTree();
				std::cout << "\n---------Inner " << inner << "\n\n";
				ast2.printTree();

				std::cout << "\n--------------Influencing Tree\n\n";
				ast3.printTree();
				std::cout << '\n';
#endif
				testSetUniqueCauseMaskingMCDC.insert(outer);
				testSetUniqueCauseMaskingMCDC.insert(inner);
				mcdc.add(McdcIndependencePair(McdcType::UniqueCauseMasking, outer, inner, influencingCondition));


			}
			if (maskingMCDC)
			{
				++counter;
				++counterMaskingMCDC;
				//std::cout << std::setw(3) << counter << "----->>> MASKING MCDC for : '" << influencingCondition << "'  " << std::setw(3) << counterMaskingMCDC <<  "    Independence pair: " << outer << ' ' << inner << "\n\n\n";
#if FULL_OUTPUT

				std::cout << "---------Outer " << outer << "\n\n";

				ast.printTree();
				std::cout << "\n---------Inner " << inner << "\n\n";
				ast2.printTree();

				std::cout << "\n--------------Influencing Tree\n\n";
				ast3.printTree();
				std::cout << '\n';
#endif
				testSetMaskingMCDC.insert(outer);
				testSetMaskingMCDC.insert(inner);
				mcdc.add(McdcIndependencePair(McdcType::Masking, outer, inner, influencingCondition));

			}


		}
	}
}
std::cout << "\n\n--------------------------------------------------------------Truth Table\n\n";
printTruthTable(mv, mc.getSymbolTable());
std::cout << "\n\n----------------------\nUnique Cause MCDC Test Set\n";
for (auto ts : testSetUniqueCauseMCDC)
{
	std::cout << ts << ' ';
}
std::cout << "\n\n----------------------\nUnique Cause + Masking MCDC Test Set\n";
for (auto ts : testSetUniqueCauseMaskingMCDC)
{
	std::cout << ts << ' ';
}
std::cout << "\n\n----------------------\nMasking MCDC Test Set\n";
for (auto ts : testSetMaskingMCDC)
{
	std::cout << ts << ' ';
}

mcdc.checkForCoverage();
mcdc.generateTestSets();

std::cout << "\n\n";
#endif

std::cout << "\n\n\n";


endTime = std::chrono::high_resolution_clock::now();
std::cout << "Program Run Time:   " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTimeOverall).count() << " ms\n\n";


return 0;



#endif



























#if 0




//std::string source("ABCD + ABcD + ABcd + AbCD + AbCd + AbcD + Abcd + aBCD + aBCd + aBcD + aBcd + abCD + abCd");
//std::string source("ab+ac+bc");
std::string source("a(b^(c+d))");
//std::string source("ab+c");
VirtualMachineForAST ast;
CompilerForAST cfa(source, ast);
cfa.compile();
//ast.printTree();



VirtualMachineForAST ast2 = ast;
VirtualMachineForAST ast3 = ast;

for (uint outer = 0; outer < 15; ++outer)
{
	for (uint inner = outer + 1; inner < 16; ++inner)
	{
		ast.evaluateTree(outer);
		ast2.evaluateTree(inner);
		ast3 = ast ^ ast2;
		if (ast3.hasIndependentCondition())
		{

			std::cout << "--------------------------------- Evaluate   '" << source << "'   with outer: " << outer << "   and inner " << inner << '\n';
			std::cout << "---------Outer " << outer << '\n';

			ast.printTree();
			std::cout << "---------Inner " << inner << '\n';
			ast2.printTree();

			std::cout << "--------------Indepence Tree\n";
			ast3.printTree();
			std::cout << '\n';
		}
	}
}






/*
CNF cnf = {
				{{{0}},{{1}}},		{{{0}},{{3}}},		{{{1}},{{2}}},		{{{2}},{{3}}},		{{{1}}},{{{2}},{{3}},{{4}},{{5}}},
				{{{1}},{{4}}},		{{{3}},{{4}}},		{{{1}},{{4}}},		{{{0}},{{4}}},
				{{{0}},{{5}}},		{{{1}},{{5}}},		{{{2}},{{5}}},		{{{3}},{{5}}},
				{{{4}},{{5}}},		{{{6}},{{1}}},		{{{2}},{{6}}},		{{{3}},{{6}}}
			};

PetricksMethod petricksMethod;
ProductTermVector ptv = petricksMethod.run(cnf);
petricksMethod.printDNF();
petricksMethod.printProductTermVector(ptv);
return 0;
*/
ObjectCode objectCode;

//std::string sourceCode("aB+Bc(de+cf)!(aj+!ba)"); // == aB

//std::string sourceCode("(aC+bcd || !(eg)) && m ^abcefg");

//std::string sourceCode("(a+b)(c+d)(a+c)(e+f)(b+e)(d+f)"); // petrick
//std::string sourceCode("ABCD + AbCD + AbCd + AbcD + Abcd + aBCD + aBCd + aBcD + abCd + abcd"); //excel
//std::string sourceCode("ABCD + ABcD + ABcd + AbCD + AbCd + AbcD + Abcd + aBCD + aBCd + aBcD + aBcd + abCD + abCd");  // cyclic core
//std::string sourceCode("abcdefghijklmno+a");  
//std::string sourceCode("a+b+c+d+e+f+g+h+i+j+k+l");  
//std::string sourceCode("abCD + abcd + AbcD + AbCd + aBCd + aBcD + ABcd + ABCD");
//std::string sourceCode("aA");

//std::string sourceCode("a+b+c");
//std::string sourceCode("a + bcdefghijklmnopqrstuvwxyz ^ ABCDEFGHIJKLMNOPQRSTUVWXYZ +abcdefghijklmnopqrstuvwyxz (fghi (jk + lmn ^ opq & !r)(st)(((xyz))))");
//std::string sourceCode("aklmn + bcdefghijklmnop ^ ABCDEFGHIJKLMNOP +abcdefghijklmnop (fghi (jk + lmn ^ op & !b)(m+n)(((a+b+c))))");
//std::string sourceCode("abefg+cdfgh+hnbcd+abmn+(ab+bc)(ce+ef)(dlk+ajk)+aklmn + bcdefghijklmn ^ ABCDEFGHIJKLMNop +abcdefghijklmn (fghi (jk + lmn ^ ak & !b)(m+n)(((a+b+c))))");
//std::string sourceCode("abefg+cdfg+bcd+ab+(ab+bc)(ce+ef)(def+ade)+a + bcdefghi ^ ABCDEFGHI +abcdefghi (fgca (bd + cef ^ ak & !b)(c+d+e+f)(((a+b+c))))");
//std::string sourceCode("ABCDEFGHIJKLMNop + bcdefghijklmn + aklmn");
//std::string sourceCode("ab || ac");
//std::string sourceCode("ab || ac + cde ^ abc (ab+ef)!(cd+ef)+((a(b(c(d+e)))))");
//std::string sourceCode("ade^CDE(F+A)+bdf(CD+EF)+a(b(c(d(e+f)^ab)^bc)(c+d))+abd+cf+ae+cdf+af+acf+bcf+abc^def+c(d(e(f+abc+def+ab))) || ac + cde ^ abc (ab+ef)!(cd+ef)+((a(b(c(d+e)))))");
//std::string sourceCode("a  A");
std::string sourceCode(source);
std::cout << "Compiling source code   '" << sourceCode << "'   to -->\n\n";
CompilerForVM compiler(sourceCode, objectCode);



bool compilerReturnCode = compiler.compile();
if (compilerReturnCode)
{
	VirtualMachineForBooleanExpressions vm;
	vm.load(objectCode);

	uint symbolCount = objectCode.symbolTable.numberOfSymbols();
	uint maxEvaluations = 1 << symbolCount;
	uint maxLengthDecimalEquivalent = static_cast<uint>(std::log10(maxEvaluations)) + 1;
	std::cout << "start    " << maxEvaluations << "\n";
	std::cout << "\nTruth table for:\n\n" << sourceCode << "\n\n";

	for (uint i = 0; i < (1 + maxLengthDecimalEquivalent + 1); ++i) { std::cout << ' '; }
	for (cchar c : objectCode.symbolTable.symbol) { std::cout << c; }
	std::cout << '\n';

	for (uint i = 0; i < (1 + maxLengthDecimalEquivalent + 1 + symbolCount + 2); ++i) { std::cout << '-'; }
	std::cout << "+--\n";

	std::bitset<26> b;




	// This is one QM table.
	// It is at this point in time the first of many QM minimizer table. So the level 0 QM table
	QMTable qmTable;

	MintermVector mintermVector;
	for (uint i = 0; i < maxEvaluations; ++i)
	{
		sint result = vm.run(i);


		// We will now fill a qm table row
		QMTableRow qmTableRow;
		// Only if the result is true, this term is a minterm
		if (result)
		{
			mintermVector.push_back(i);
			// The minterm list consits in level 0 only of one minterm
			qmTableRow.mintermSet.insert(i);
			// Now add this QM table with a one element minterm list to the QM table, grouped by bit counts
			qmTable[numberOfSetBits(i)].insert(qmTableRow);
		}
		b = i;
		std::cout << std::setw(maxLengthDecimalEquivalent + 1) << i << " " << b.to_string().substr(26 - symbolCount) << "    " << result << '\n';
	}
	std::cout << "end\n";

	std::cout << "start    " << maxEvaluations << "\n";

	// Instantiate the complete vector of all minimize tebales for all levels
	QMMinimizeTable qmMinimizeTable;
	qmMinimizeTable.reserve(symbolCount);
	// And fill in the just created qmTable for levele 0
	qmMinimizeTable.push_back(qmTable);


#if 1
	// Iterate over the qmTable for Level 0 and output the number of bitcounts and the minterm lists
	for (const auto &[numberOfBits, qmTableForOneBitCount] : qmTable)
	{
		std::cout << "\n\nBit count: " << static_cast<uint>(numberOfBits) << "    Count: " << qmTableForOneBitCount.size() << "\n----------------------------\n";
		for (const QMTableRow &qmTableRow : qmTableForOneBitCount)
		{
			std::cout << *qmTableRow.mintermSet.begin() << ' ';
		}

	}
	std::cout << "\n\n";
#endif
	PrimeImplicantSet qmPrimeImplicantSet;

	uint currentTableIndex = 0;
	do
	{
		// We create an empty QM table and add this to the minimizer table vector
		QMTable qmTableNext;
		qmMinimizeTable.push_back(qmTableNext);

		//std::cout << "------- Minimize Table Level: " << (currentTableIndex+1) << '\n';


		// Iterate over all maps of the current table under evaluation
		// The maps contain minterm lists with different numbers of bits
		// So iterate over all bit Groups in this table
		// Because we want to compare between 2 bit groups, we need at least 2 entries in the map
		if (qmMinimizeTable[currentTableIndex].size() > 1)
			for (QMTableIterator	qmTableIterator = qmMinimizeTable[currentTableIndex].begin();
				qmTableIterator != std::prev(qmMinimizeTable[currentTableIndex].end());
				++qmTableIterator)
		{
			uint8_t numberOfBitsLower = qmTableIterator->first;
			uint8_t numberOfBitsUpper = std::next(qmTableIterator)->first;
			if ((numberOfBitsUpper - numberOfBitsLower) == 1)
			{
				//std::cout << "Comparing Bitcount: " << static_cast<sint>(numberOfBitsUpper) << "  with  " << static_cast<sint>(numberOfBitsLower) << '\n';
				for (const QMTableRow& qmTableRowUpper : std::next(qmTableIterator)->second)
				{
					for (const QMTableRow& qmTableRowLower : qmTableIterator->second)
					{
						if (qmTableRowLower.maskForEliminatedCondition == qmTableRowUpper.maskForEliminatedCondition)
						{
							MinTermNumber mtn = (*qmTableRowUpper.mintermSet.begin() ^ *qmTableRowLower.mintermSet.begin());
							if (!(mtn & (mtn - 1)))
							{
								// We found a match
								qmTableRowLower.matchFound = true;
								qmTableRowUpper.matchFound = true;

								// Get number of bits of the lower Minterm of the Minterms groups that we compare
								NumberOfBitsCountType numberOfBitsOfLowerMinterm = numberOfSetBits(*qmTableRowLower.mintermSet.begin());
								QMTableForOneBitCount &qmTableForOneBitCount = qmMinimizeTable[currentTableIndex + 1][numberOfBitsOfLowerMinterm];

								QMTableRow qmTableRowForNextMinimizeTable;
								qmTableRowForNextMinimizeTable.maskForEliminatedCondition = mtn + qmTableRowLower.maskForEliminatedCondition;
								MinTermNumber minTermNumber = *qmTableRowLower.mintermSet.begin();
								//qmTableRowForNextMinimizeTable.mintermSet.insert(*qmTableRowLower.mintermSet.begin());
								//qmTableRowForNextMinimizeTable.mintermSet.insert(*qmTableRowUpper.mintermSet.rbegin());

								qmTableRowForNextMinimizeTable.mintermSet.insert(qmTableRowLower.mintermSet.begin(), qmTableRowLower.mintermSet.end());
								qmTableRowForNextMinimizeTable.mintermSet.insert(qmTableRowUpper.mintermSet.begin(), qmTableRowUpper.mintermSet.end());

								qmTableForOneBitCount.insert(std::move(qmTableRowForNextMinimizeTable));

							}
						}
					}
				}
			}
		}



		// Get Prime Implicants
		for (auto &[noOfBits, qmTableForOneBitCountCheck] : qmMinimizeTable[currentTableIndex])
		{
			for (const QMTableRow &qmTableRowCheck : qmTableForOneBitCountCheck)
			{
				if (!qmTableRowCheck.matchFound)
				{

					PrimeImplicantType primeImplicant(*qmTableRowCheck.mintermSet.begin(), qmTableRowCheck.maskForEliminatedCondition);
					qmPrimeImplicantSet.insert(std::move(primeImplicant));

				}
			}
		}




		++currentTableIndex;
	} while (qmMinimizeTable[currentTableIndex].size());

	printMinizeTable(qmMinimizeTable, objectCode.symbolTable);

	std::cout << "\n\nSource: " << sourceCode << "\n\nDNF with Prime Implicants: \n\n";



	for (const PrimeImplicantType& resultingPrimeImplicant : qmPrimeImplicantSet)
	{
		std::cout << resultingPrimeImplicant.toString(objectCode.symbolTable) << "  ";
	}
	std::cout << "\n\n";


	PrimeImplicantSelection primeImplicantSelection;
	primeImplicantSelection.initiate(mintermVector, qmPrimeImplicantSet, objectCode.symbolTable);
	primeImplicantSelection.reduce();


	std::cout << "\n\n\n";


	std::cout << '\n';
	}
std::cout << '\n';

endTime = std::chrono::high_resolution_clock::now();
std::cout << "Program Run Time:   " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTimeOverall).count() << " ms\n\n";


return 0;




















/*
void VirtualMachineForAST::addInfluencingVariable(uint index)
{
	cchar symbol{ ' ' };
	if (ast[index].nodeType == Token::ID)
	{
		symbol = ast[index].tokenWithAttribute.inputTerminalSymbol;
		InfluencingVariables::Detail id{ symbol, ast[index].value, false };
		influencingVariables.variables[symbol].emplace_back(std::move(id));
	}
	else
	{
		symbol = ast[index].tokenWithAttribute.inputTerminalSymbol + ('a' - 'A');
		InfluencingVariables::Detail id{ symbol, ast[index].value, true };
		influencingVariables.variables[symbol].emplace_back(std::move(id));
	}
}

void VirtualMachineForAST::countInfluencingVariables(uint index)
{
	cchar symbol{ ' ' };
	if (ast[index].nodeType== Token::ID)
	{
		symbol = ast[index].tokenWithAttribute.inputTerminalSymbol;
		InfluencingVariables::Detail id{ symbol, ast[index].value, false };
		influencingVariables.variables[symbol].emplace_back(std::move(id));
	}
	else
	{
		symbol = ast[index].tokenWithAttribute.inputTerminalSymbol + ('a'-'A');
		InfluencingVariables::Detail id{ symbol, ast[index].value, true };
		influencingVariables.variables[symbol].emplace_back(std::move(id));
	}

}
*/






#if 0



Coverage::Coverage coverage;




coverage.addRow(std::string("A"));
coverage.addRow(std::string("B"));
coverage.addRow(std::string("C"));

coverage.addColumn(std::string("1"));
coverage.addColumn(std::string("2"));
coverage.addColumn(std::string("3"));
coverage.addColumn(std::string("4"));
coverage.addColumn(std::string("5"));
coverage.addColumn(std::string("6"));
coverage.addColumn(std::string("7"));


coverage.setCellAsCover(0, 0);
coverage.setCellAsCover(0, 1);
coverage.setCellAsCover(0, 2);
coverage.setCellAsCover(0, 4);
coverage.setCellAsCover(0, 5);
coverage.setCellAsCover(0, 6);
coverage.setCellAsCover(1, 3);
coverage.setCellAsCover(1, 5);
coverage.setCellAsCover(2, 3);
coverage.setCellAsCover(2, 4);


coverage.reduce();

#endif





#endif


#if 0


void printMinizeTable(const QMMinimizeTable &qmMinimizeTable, const SymbolTable &symbolTable)
{
	uint maxNumberOfBits = static_cast<uint>(symbolTable.symbol.size());

	for (uint currentTableIndex = 0; currentTableIndex < qmMinimizeTable.size(); ++currentTableIndex)
	{
		std::cout << "Table index: " << currentTableIndex + 1 << "-----------------------------\n";
		for (auto &[numberOfBits, qmTableForOneBitCount] : qmMinimizeTable[currentTableIndex])
		{
			for (const QMTableRow &qmTableRow : qmTableForOneBitCount)
			{
				std::cout << std::setw(3) << static_cast<uint>(numberOfBits) << "  ";

				MinTermNumber mtn = *qmTableRow.mintermSet.begin();
				MinTermNumber deletedPos = qmTableRow.maskForEliminatedCondition;
				MinTermNumber bitMask = 1 << (maxNumberOfBits - 1);

				for (uint bitIndex = 0; bitIndex < maxNumberOfBits; ++bitIndex)
				{
					cchar output = '0';
					if (deletedPos & bitMask)
					{
						output = '-';
					}
					else
					{
						if (mtn & bitMask)
						{
							output = '1';
						}
					}
					bitMask = bitMask >> 1;
					std::cout << output << ' ';
				}
				std::cout << ' ' << (qmTableRow.matchFound ? 'X' : '_') << ' ' << std::setw(4) << deletedPos << ' ';
				for (const MinTermNumber &matchedMinterm : qmTableRow.mintermSet)
				{
					std::cout << matchedMinterm << ' ';
				}
				std::cout << '\n';
			}
		}
	}
}





std::string QMTableRowBase::toString(const SymbolTable &symbolTable) const
{
	std::ostringstream result;
	uint symbolCount = symbolTable.numberOfSymbols();
	MinTermNumber mask = 1 << (symbolCount - 1);
	std::set<cchar>::iterator symbolIterator = symbolTable.symbol.begin();

	for (uint ui = 0; ui < symbolCount; ++ui)
	{
		if (!(mask & maskForEliminatedCondition))
		{
			MinTermNumber mt = *mintermSet.begin();
			MinTermNumber mtMasked = mt & mask;
			result << (mtMasked ? *symbolIterator : static_cast<cchar>((*symbolIterator) - ('a' - 'A')));
		}
		mask = mask >> 1;
		++symbolIterator;
	}
	return result.str();
}

NumberOfBitsCountType QMTableRowBase::numberOfTrues() const
{
	MinTermNumber mask = ~maskForEliminatedCondition;
	return (*mintermSet.begin()  &  mask);
}



// *************************************************************************************************************************************
// *************************************************************************************************************************************
// *************************************************************************************************************************************
// *************************************************************************************************************************************
// *************************************************************************************************************************************



void PrimeImplicantSelection::initiate(const MintermVector &mintermVector, const PrimeImplicantSet &qmPrimeImplicantSet, const SymbolTable &st)
{
	symbolTable = st;

	Column column;

	// Build prime implicant columns
	for (const PrimeImplicantType& resultingPrimeImplicant : qmPrimeImplicantSet)
	{
		column.primeImplicant = resultingPrimeImplicant;
		tableColumnForPrimeImplicant.push_back(column);
	}

	RowIndex rowIndex{ 0 };
	// Iterate over all minterms
	for (MinTermNumber mtnMinTerm : mintermVector)
	{
		Row row;
		row.minTerm = mtnMinTerm;

		ColumnIndex columnIndex{ 0 };

		// Iterate ove all prime implicantes
		for (const PrimeImplicantType& resultingPrimeImplicant : qmPrimeImplicantSet)
		{
			// Check, what minterm are covered
			MinTermNumber mtnPrimeImplicant = resultingPrimeImplicant.term;
			MinTermNumber primeImplicantMask = resultingPrimeImplicant.mask;
			MinTermNumber primeImplicantMaskNegated = ~resultingPrimeImplicant.mask;
			MinTermNumber mtnMinTermMasked = mtnMinTerm & primeImplicantMaskNegated;
			MinTermNumber mtnPrimeImplicantMasked = mtnPrimeImplicant & primeImplicantMaskNegated;

			if (mtnMinTermMasked == mtnPrimeImplicantMasked)
			{
				row.rowEntry.push_back(PrimeImplicanTableCell(columnIndex));
				tableColumnForPrimeImplicant[columnIndex].columnEntry.push_back(PrimeImplicanTableCell(rowIndex));
			}
			++columnIndex;
		}
		++rowIndex;
		tableRowForMinterm.push_back(row);
	}
	printTable();
}


void PrimeImplicantSelection::printTable()
{
	return;
	// get the biggest size of a Prime Implicant 
	uint maxSizePrimeImplicant{ 0 };
	for (auto &c : tableColumnForPrimeImplicant)
	{
		uint s = static_cast<uint>(c.primeImplicant.toString(symbolTable).size());
		if (s > maxSizePrimeImplicant)
		{
			maxSizePrimeImplicant = s;
		}
	}
	// get the biggest size of a minterm
	uint maxSizeMinTerm{ 0 };
	for (auto &r : tableRowForMinterm)
	{
		uint s = r.minTerm;
		if (s > maxSizeMinTerm)
		{
			maxSizeMinTerm = s;
		}
	}
	maxSizeMinTerm = static_cast<uint>(std::log10(maxSizeMinTerm)) + 1;
	//std::cout << '\n';
	// print Header
	std::cout << '\n' << std::setw(maxSizeMinTerm + 2) << ' ';
	for (uint c = 0; c < tableColumnForPrimeImplicant.size(); ++c)
	{
		std::cout << std::setw(maxSizePrimeImplicant) << tableColumnForPrimeImplicant[c].primeImplicant.toString(symbolTable) << ' ';
	}
	std::cout << '\n';
	// print rows
	for (auto &r : tableRowForMinterm)
	{
		std::cout << std::setw(maxSizeMinTerm) << r.minTerm << "  ";
		for (uint c = 0; c < tableColumnForPrimeImplicant.size(); ++c)
		{
			bool found{ false };
			for (const PrimeImplicanTableCell& pie : r.rowEntry)
			{
				if (pie.crossReference == c)
				{
					found = true;
					cchar mark{ ' ' };
					if (pie.dropped)
					{
						mark = '-';
					}
					else
					{
						mark = 'X';

					}
					std::cout << std::setw(maxSizePrimeImplicant) << mark << ' ';
				}
			}
			if (!found)
			{
				std::cout << std::setw(maxSizePrimeImplicant) << ' ' << ' ';
			}
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}


void PrimeImplicantSelection::printEssentialPrimeImplicants()
{
	std::cout << "Essential Prime Implicants:  ";
	for (auto epi : essentialPrimeImplicant)
	{
		std::cout << epi.toString(symbolTable) << "  ";
	}
	std::cout << '\n';
}

void PrimeImplicantSelection::identifyEssentialPrimeImplicant()
{
	for (auto &r : tableRowForMinterm)
	{
		uint primeImplicantCount{ 0 };
		RowIndex positionLastPositivPrimeImplicant{ 0 };
		if (!r.rowIsDisabled)
		{
			for (RowIndex ri = 0; ri < r.rowEntry.size(); ++ri)
			{
				if (!r.rowEntry[ri].dropped)
				{
					++primeImplicantCount;
					positionLastPositivPrimeImplicant = r.rowEntry[ri].crossReference;
				}
			}
		}
		if (1 == primeImplicantCount)
		{
			std::cout << "Found new essential prime implicant: " << tableColumnForPrimeImplicant[positionLastPositivPrimeImplicant].primeImplicant.toString(symbolTable) << '\n';
			essentialPrimeImplicant.insert(tableColumnForPrimeImplicant[positionLastPositivPrimeImplicant].primeImplicant);
			dropColumnAndAllRowsInColumn(positionLastPositivPrimeImplicant);
		}
	}
}
bool PrimeImplicantSelection::checkForImplication(const RowColumnEntry &first, const RowColumnEntry &second)
{
	bool result{ true };
	for (RowIndex riFirst = 0; riFirst < first.size() && result; ++riFirst)
	{
		if (first[riFirst].dropped)
		{
			RowColumnEntry::const_iterator rcei = std::find_if(second.begin(), second.end(), [first, riFirst](const PrimeImplicanTableCell &pite) { return pite.crossReference == first[riFirst].crossReference; });
			if ((rcei != second.end()) && (!rcei->dropped))
			{
				result = false;
			}
		}
	}
	for (RowIndex riSecond = 0; riSecond < second.size() && result; ++riSecond)
	{
		if (!second[riSecond].dropped)
		{
			RowColumnEntry::const_iterator rcei = std::find_if(first.begin(), first.end(), [second, riSecond](const PrimeImplicanTableCell &pite) { return pite.crossReference == second[riSecond].crossReference; });
			if ((rcei == first.end()) || (rcei->dropped))
			{
				result = false;
			}
		}
	}
	return result;
}


void PrimeImplicantSelection::findDominatingRows()
{
	std::set<RowIndex> rowsToDrop;
	for (RowIndex outer = 0; outer < tableRowForMinterm.size(); ++outer)
	{
		if (!tableRowForMinterm[outer].rowIsDisabled)
		{
			for (RowIndex inner = outer + 1; inner < tableRowForMinterm.size(); ++inner)
			{
				if (!tableRowForMinterm[inner].rowIsDisabled)
				{
					bool outerImpliesInner = checkForImplication(tableRowForMinterm[outer].rowEntry, tableRowForMinterm[inner].rowEntry);
					bool innerImpliesOuter = checkForImplication(tableRowForMinterm[inner].rowEntry, tableRowForMinterm[outer].rowEntry);
					if (outerImpliesInner && innerImpliesOuter)
					{
						std::cout << "Identical rows: " << tableRowForMinterm[outer].minTerm << " and " << tableRowForMinterm[inner].minTerm << "    Dropping row: " << tableRowForMinterm[inner].minTerm << '\n';
						rowsToDrop.insert(inner);
					}
					else if (outerImpliesInner)
					{
						std::cout << "Outer row implies inner: " << tableRowForMinterm[outer].minTerm << " --> " << tableRowForMinterm[inner].minTerm << "    Dropping row: " << tableRowForMinterm[outer].minTerm << '\n';
						rowsToDrop.insert(outer);
					}
					else if (innerImpliesOuter)
					{
						std::cout << "Inner row implies outer: " << tableRowForMinterm[inner].minTerm << " --> " << tableRowForMinterm[outer].minTerm << "    Dropping row: " << tableRowForMinterm[inner].minTerm << '\n';
						rowsToDrop.insert(inner);
					}
				}
			}
		}
	}
	for (const RowIndex &ri : rowsToDrop)
	{
		dropRow(ri);
	}
}


void PrimeImplicantSelection::findDominatingColumns()
{
	std::set<ColumnIndex> columnsToDrop;
	for (ColumnIndex outer = 0; outer < tableColumnForPrimeImplicant.size(); ++outer)
	{
		if (!tableColumnForPrimeImplicant[outer].columnIsDisabled)
		{
			for (ColumnIndex inner = outer + 1; inner < tableColumnForPrimeImplicant.size(); ++inner)
			{
				if (!tableColumnForPrimeImplicant[inner].columnIsDisabled)
				{
					bool outerImpliesInner = checkForImplication(tableColumnForPrimeImplicant[outer].columnEntry, tableColumnForPrimeImplicant[inner].columnEntry);
					bool innerImpliesOuter = checkForImplication(tableColumnForPrimeImplicant[inner].columnEntry, tableColumnForPrimeImplicant[outer].columnEntry);
					// // check for equality
					if (outerImpliesInner && innerImpliesOuter)
					{
						// drop more expensiv column
						// more expensiv means: more negations
						std::cout << "Identical columns: " << tableColumnForPrimeImplicant[outer].primeImplicant.toString(symbolTable) << " and " << tableColumnForPrimeImplicant[inner].primeImplicant.toString(symbolTable) << "    Dropping column: ";
						if (tableColumnForPrimeImplicant[outer].primeImplicant.numberOfTrues() == tableColumnForPrimeImplicant[inner].primeImplicant.numberOfTrues())
						{
							std::cout << tableColumnForPrimeImplicant[inner].primeImplicant.toString(symbolTable) << '\n';
							columnsToDrop.insert(inner);
						}
						else if (tableColumnForPrimeImplicant[outer].primeImplicant.numberOfTrues() > tableColumnForPrimeImplicant[inner].primeImplicant.numberOfTrues())
						{
							std::cout << tableColumnForPrimeImplicant[inner].primeImplicant.toString(symbolTable) << '\n';
							columnsToDrop.insert(inner);
						}
						else
						{
							std::cout << tableColumnForPrimeImplicant[outer].primeImplicant.toString(symbolTable) << '\n';
							columnsToDrop.insert(outer);
						}
					}
					else if (outerImpliesInner)
					{
						columnsToDrop.insert(inner);
						std::cout << "Outer Implies inner: " << tableColumnForPrimeImplicant[outer].primeImplicant.toString(symbolTable) << " --> " << tableColumnForPrimeImplicant[inner].primeImplicant.toString(symbolTable) << "    Dropping column: " << tableColumnForPrimeImplicant[inner].primeImplicant.toString(symbolTable) << '\n';
					}
					else if (innerImpliesOuter)
					{
						columnsToDrop.insert(outer);
						std::cout << "Inner Implies outer: " << tableColumnForPrimeImplicant[inner].primeImplicant.toString(symbolTable) << " --> " << tableColumnForPrimeImplicant[outer].primeImplicant.toString(symbolTable) << "    Dropping column: " << tableColumnForPrimeImplicant[outer].primeImplicant.toString(symbolTable) << '\n';
					}
				}
			}
		}
	}
	for (const ColumnIndex &ci : columnsToDrop)
	{
		dropColumn(ci);
	}
}

void PrimeImplicantSelection::dropColumn(ColumnIndex columnIndex)
{
	// Disable complete column
	tableColumnForPrimeImplicant[columnIndex].columnIsDisabled = true;
	// Set all entries in the column to dropped
	for (auto &ce : tableColumnForPrimeImplicant[columnIndex].columnEntry)
	{
		ce.dropped = true;
		// Go through all rows and look if a row enty has a reference to this column and set it also to dropped
		for (auto &re : tableRowForMinterm[ce.crossReference].rowEntry)
		{
			if (re.crossReference == columnIndex)
			{
				re.dropped = true;
			}
		}

	}
}
void PrimeImplicantSelection::dropRow(RowIndex rowIndex)
{
	// Disable complete row
	tableRowForMinterm[rowIndex].rowIsDisabled = true;
	// Set all entries in the row to dropped
	for (auto &re : tableRowForMinterm[rowIndex].rowEntry)
	{
		re.dropped = true;
		// Go through all columns and look if a row enty has a reference to this row and set it also to dropped
		for (auto &ce : tableColumnForPrimeImplicant[re.crossReference].columnEntry)
		{
			if (ce.crossReference == rowIndex)
			{
				ce.dropped = true;
			}
		}
	}
}

void PrimeImplicantSelection::dropColumnAndAllRowsInColumn(ColumnIndex columnIndex)
{
	for (auto &ce : tableColumnForPrimeImplicant[columnIndex].columnEntry)
	{
		dropRow(ce.crossReference);
	}
	dropColumn(columnIndex);

}




void PrimeImplicantSelection::eraseRow(RowIndex rowIndex)
{
	// Go through all columns and remove the entries with a reference to this row
	for (ColumnIndex ci = 0; ci < tableColumnForPrimeImplicant.size(); ++ci)
	{
		// Check the rows in this column
		for (RowIndex ri = 0; ri < tableColumnForPrimeImplicant[ci].columnEntry.size(); )
		{
			// If there is an entry wit the row-to-delete as reference
			if (tableColumnForPrimeImplicant[ci].columnEntry[ri].crossReference == rowIndex)
			{
				// then erase this entry
				tableColumnForPrimeImplicant[ci].columnEntry.erase(tableColumnForPrimeImplicant[ci].columnEntry.begin() + ri);
				// Do not increase the loop index
			}
			else
			{
				if (tableColumnForPrimeImplicant[ci].columnEntry[ri].crossReference > rowIndex)
				{
					--tableColumnForPrimeImplicant[ci].columnEntry[ri].crossReference;
				}
				++ri;
			}
		}
	}
	tableRowForMinterm.erase(tableRowForMinterm.begin() + rowIndex);
}


void PrimeImplicantSelection::eraseColumn(ColumnIndex columnIndex)
{
	// Go through all rows and remove the entries with a reference to this column
	for (RowIndex ri = 0; ri < tableRowForMinterm.size(); ++ri)
	{
		// Check the columns in this row
		for (ColumnIndex ci = 0; ci < tableRowForMinterm[ri].rowEntry.size(); )
		{
			// If there is an entry wit the row-to-delete as reference
			if (tableRowForMinterm[ri].rowEntry[ci].crossReference == columnIndex)
			{
				// then erase this entry
				tableRowForMinterm[ri].rowEntry.erase(tableRowForMinterm[ri].rowEntry.begin() + ci);
				// Do not increase the loop index
			}
			else
			{
				if (tableRowForMinterm[ri].rowEntry[ci].crossReference > columnIndex)
				{
					--tableRowForMinterm[ri].rowEntry[ci].crossReference;
				}
				++ci;
			}
		}
	}
	tableColumnForPrimeImplicant.erase(tableColumnForPrimeImplicant.begin() + columnIndex);
}



void PrimeImplicantSelection::eraseAllDroppedRows()
{
	sint ri = static_cast<sint>(tableRowForMinterm.size()) - 1;
	while (ri >= 0)
	{
		if (tableRowForMinterm[ri].rowIsDisabled)
		{
			eraseRow(ri);
		}
		--ri;
	}
}

void PrimeImplicantSelection::eraseAllDroppedColumns()
{

	sint ci = static_cast<sint>(tableColumnForPrimeImplicant.size()) - 1;
	while (ci >= 0)
	{
		if (tableColumnForPrimeImplicant[ci].columnIsDisabled)
		{
			eraseColumn(ci);
		}
		--ci;
	}


}

ulong PrimeImplicantSelection::countActiveTableElements()
{
	ulong result{ 0 };
	for (const auto &r : tableRowForMinterm)
	{
		if (!r.rowIsDisabled)
			for (const auto &cell : r.rowEntry)
			{
				if (!cell.dropped)
				{
					++result;
				}
			}
	}
	return result;
}

void PrimeImplicantSelection::reduce()
{
	ulong oldNumberOfNoneDroppedCells{ 0 };
	ulong newNumberOfNoneDroppedCells{ countActiveTableElements() };
	uint reductionLoopCounter{ 0 };
	do
	{
		++reductionLoopCounter;
		std::cout << "\n\n----------------------------------------------- Reduction Loop:" << reductionLoopCounter << '\n';
		oldNumberOfNoneDroppedCells = newNumberOfNoneDroppedCells;

		std::cout << "\n\nSearch essential Prime Implicants----------------\n";
		ulong numberOfNoneDroppedCellsBeforeOperation = countActiveTableElements();
		identifyEssentialPrimeImplicant();
		if (countActiveTableElements() == numberOfNoneDroppedCellsBeforeOperation)
		{
			std::cout << "Nothing new found\n";
		}
		else
		{
			printEssentialPrimeImplicants();

			printTable();
			std::cout << "\nCompacting . . .\n";
			eraseAllDroppedRows();
			eraseAllDroppedColumns();
			printTable();
			std::cout << "List with all "; printEssentialPrimeImplicants();
		}

		std::cout << "\n\nIdentify Dominating columns--------------------\n";
		numberOfNoneDroppedCellsBeforeOperation = countActiveTableElements();
		findDominatingColumns();
		if (countActiveTableElements() == numberOfNoneDroppedCellsBeforeOperation)
		{
			std::cout << "Nothing new found\n";
		}
		else
		{
			printTable();
			std::cout << "\nCompacting . . .\n";
			eraseAllDroppedColumns();
			printTable();
			printEssentialPrimeImplicants();
		}

		std::cout << "\n\nIdentify Dominating rows-----------------------\n";
		numberOfNoneDroppedCellsBeforeOperation = countActiveTableElements();
		findDominatingRows();
		if (countActiveTableElements() == numberOfNoneDroppedCellsBeforeOperation)
		{
			std::cout << "Nothing new found\n";
		}
		else
		{
			printTable();
			std::cout << "\nCompacting . . .\n";
			eraseAllDroppedRows();
			printTable();
		}

		//printEssentialPrimeImplicants();

		newNumberOfNoneDroppedCells = countActiveTableElements();
	} while ((0 != newNumberOfNoneDroppedCells) && (newNumberOfNoneDroppedCells != oldNumberOfNoneDroppedCells));

	std::cout << "\n-------------------------------------------------------------------------------------------------------\n";

	if (0 == newNumberOfNoneDroppedCells)
	{
		std::cout << "\n\nEverything minimized. Minimal form consists of all essential prime implicants\n\n";
		printEssentialPrimeImplicants();

	}
	else
	{
		std::cout << "\n\nFound essential prime implicants plus Cyclic core \n\n";
		printEssentialPrimeImplicants();
		std::cout << "\nCyclic core: \n";
		printTable();
	}

	//Petricks method
	if (0 != newNumberOfNoneDroppedCells)
	{


		CNF cnf;
		// Fill maxterm with Primimplicnats that cover a minterm
		// Iterate over rows
		for (RowIndex ri = 0; ri < tableRowForMinterm.size(); ++ri)
		{
			if (!tableRowForMinterm[ri].rowIsDisabled)
			{
				DNF dnf;
				for (ColumnIndex ci = 0; ci < tableRowForMinterm[ri].rowEntry.size(); ++ci)
				{
					ProductTerm productTerm;
					productTerm.insert(tableRowForMinterm[ri].rowEntry[ci].crossReference);
					dnf.insert(productTerm);
				}
				cnf.push_back(dnf);
			}
		}
		//                 (a+b)      
		//cnf = { {{{0}},{{1}}}, {{{2}},{{3}}}, {{{4}},{{5}}}, {{{6}},{{7}}}, {{{8}},{{9}}}, {{{10}},{{11}}}, {{{12}},{{13}}}, {{{14}},{{15}}}, {{{16}},{{17}}} };
		//cnf = { {{{1}}}, {{{2}},{{3}}}, {{{1}},{{3}}}, {{{1}},{{2}}} };

		PetricksMethod petricksMethod;
		ProductTermVector cheapestPrimeImplicantVector = petricksMethod.evaluate(cnf);



		std::cout << "\nCheapest Prime Implicancts from the cyclic core:\n";
		for (uint i = 0; i < cheapestPrimeImplicantVector.size(); ++i)
		{
			for (BooleanVariable bv : cheapestPrimeImplicantVector[i])
			{
				std::cout << tableColumnForPrimeImplicant[bv].primeImplicant.toString(symbolTable) << ' ';
			}
			std::cout << '\n';
		}

		std::cout << "\n\n\nMinimum irredundant DNF\n\n";
		for (uint i = 0; i < cheapestPrimeImplicantVector.size(); ++i)
		{
			for (const auto &epi : essentialPrimeImplicant)
			{
				std::cout << epi.toString(symbolTable) << ' ';
			}
			for (BooleanVariable bv : cheapestPrimeImplicantVector[i])
			{
				std::cout << tableColumnForPrimeImplicant[bv].primeImplicant.toString(symbolTable) << ' ';
			}
			std::cout << '\n';
		}
		std::cout << '\n';


		sint x = 1;
	}

}


#endif
