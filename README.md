# Evaluation of Boolean Expressions and MCDC

An application for gaining a better understanding of MCDC

Brief Introduction to MCDC
==========================

In automotive Functional Safety (FuSa) relevant projects you will often
hear the term MC/DC. MC/DC is the abbreviation for Modified Condition /
Decision Coverage. It is a part of the family of Structural Coverage
metrics that are used to check the thoroughness of tests, for example:
requirements-based tests. With that more failures shall be detected
early and consequently prevented.

In most FuSa trainings you will just learn about one type of MC/DC, the
so called "unique cause" MCDC. But this works in only very few cases,
because often you have strongly or weakly coupled conditions. See the
simple C-Example statement "if ((a && b) \|\| (a && c)). Here you have 2
times condition "a" in the boolean expression. A "unique cause"-MC/DC as
per the original definition is not possible. And the basic problem here
is the Blackbox view, where you definitely need a Whitebox view (know
the source code) to be able to measure structural coverage.

To resolve this problematic of strongly (weakly) coupled conditions in
boolean expressions for MC/DC, additional types of MC/DC have been
defined:

-   "Masking MC/DC"

-   "Unique Cause + Masking MC/DC".

Here the Definition for all types of MC/DC:

-   Unique Cause MC/DC: For 2 test cases / test vectors / values of
    conditions for an expression, exactly one condition changed and the
    result of the boolean expression (the decision) changed also its
    value. The modification of this one input condition had an influence
    on the decision.

-   Masking MC/DC: For 2 test cases / test vectors / values of
    conditions for an expression, one condition changed with the result
    of changing the decision. Other conditions may have also changed,
    but they had no influence on the decision, because their effect is
    masked by other boolean expressions.

-   Unique Cause + Masking MC/DC: Here a unique cause for all uncoupled
    conditions is required. For strongly coupled conditions, masking is
    allowed (see above).

Applied Method
==============

For many people, the MC/DC definitions are hard to understand. The
presented software helps to illustrate the construction of MC/DC test
cases. The main algorithms are based on:

"An Investigation of Three Forms of the Modified Condition Decision
Coverage (MCDC) Criterion", "DOT/FAA/AR-01/18", "U.S. Department of
Transportation, Federal Aviation Administration"

In this work, "colored" or "attributed" Abstract Syntax Trees (AST) are
used to create a so called "Influencing AST"

First a boolean expression is analyzed and compiled into an Abstract
Syntax Tree. Attributes for a "boolean value" and a "not evaluated"-tag,
in case of boolean short cut evaluation, are added. To evaluate the
MC/DC property, we need at least to test values (2 condition sets, or 2
input variable values) and then check with these test values, if the pre
conditions for MC/DC are fulfilled. In the case of unique cause MC/DC
only one condition is allowed to change, and this must have an influence
on the resulting decision. Example for boolean expression "a+b" and test
vector 0,1, so for condition b:

Found Unique Cause MCDC Test pair for condition: b Test Pair: 0 1

\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--
AST for value: 0

0 ID a (0)

1 OR 0,2 (0)

2 ID b (0)

\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--
AST for value: 1

0 ID a (0)

1 OR 0,2 (1)

2 ID b (1)

\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--
AST for influencing condition check

0 ID a (0)

1 OR 0,2 (1)

2 ID b (1)

Please note: The "AST for influencing condition check" is a tree XOR of
the previous 2 ASTs. The Or operation result here is not the result of
"a+b", but an XOR of the OR value in the first AST and OR value of the
second AST. To find out, if a variable is influencing, only one leaf
must have changed and from this changed leaf the complete path up to the
root of the AST, every value must be 1 (This is valid for unique cause
MC/DS only).

For Masking MC/DC there must be again only one path with all trues from
the leaf to the root.

Example for Masking MC/DC for \"a+b\^c!(d+e)\" for test vectors 0, 15
(meaning 4 variables changed), but still it is MC/DC for condition b.
All other conditions are masked by boolean logic:

Masking MCDC Test pair for condition: b Test Pair: 0 15
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--
AST for value: 0

0 ID a (0)

1

2 OR 0,4 (0) ID b (0)

3

4 XOR 2,6 (0) ID c (0)

5

6 AND 4,8 (0) ID d (0)

7 OR 6,8 (0)

8 NOT 7 (1) ID e (0)

9

\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--
AST for value: 15

0 ID a (0)

1

2 OR 0,4 (1) ID b (1)

3

4 XOR 2,6 (1) ID c (1)

5

6 AND 4,8 (0) ID d (1)

7 OR 6,8 (1)

8 NOT 7 (0) ID e (1)

9

\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--
AST for influencing condition check

0 ID a (0)

1

2 OR 0,4 (1) ID b (1)

3

4 XOR 2,6 (1) ID c (1)

5

6 AND 4,8 (0) ID d (1)

7 OR 6,8 (1)

8 NOT 7 (1) ID e (1)

9

Numbers following operations are line numbers.

Please note: Although 4 conditions changed between AST 0 and AST 15,
there is only one path with all true from leaf (condition b) to root.
This is the influencing variable. All others are masked by boolean
logic. Therefor this test vector (0, 15) is MCDC for condition b.

The developed software shows all this ASTs and helps to understand the
MC/DC logic.

Mode of Operation
=================

Special Settings can be used to control the mode of operation.

-   Boolean Short Cut Evaluation

-   Usage of minimum irredundant DNFs

Boolean Short Cut Evaluation
----------------------------

In Languages like C and C++, boolean short cut evaluation is used for
the evaluation of boolean expressions. For example, if in "a+b+c"
condition a is true, then the complete decision is true, and b and c do
not need to be evaluated. This has of course an influence on MC/DC
considerations.

Usage of minimum irredundant DNFs
---------------------------------

Sometime complex boolean expressions are given. There are often shorter
and easier equivalent expressions existing, which make evaluation for
MC/DC substantially faster and easier. The software incorporates the
Quine & McCluskey method (modified optimized version) to minimize
boolean expression. Since the result of this method may produce more
than one solution, the set cover or unate covering problem is used, to
identify all possible outcomes. Heuristic ist used to speed up the
process and to come to one final solution.

Example:

"(ade\^CDE(F + A) + bdf(CD + EF)g + a(b(c(d(eh + fg) \^ ab) \^ bc)(c +
dg)) + abd + cf + ae + cdfg + af + acfh + bcf + abc \^ def + c(d(e(f +
abc + def + abh))) \|\| ac + cde \^ abc(ab + ef)!(cd + ef) + ((a(b(c(d +
e))))))"

can be minimized to

"CDE + ab + ac + ae + af + cde + cf + def"

Using the minimized example leads to a tremendous saving in calculation
time and memory space. E.g. output file size reduced from 364MB to
2.8MB.

The software provides flags to control the evaluation method.

Caveat
------

The software could be used to create test vectors and test suites. This
should not be done.

It is important to understand that you basically should never use the
source code or a boolean expression and then derive a test vector out of
that. With that you just check, if the compiler is working correctly.
You need to understand the difference between structural test and
structural coverage. What you should do is: At first create test cases
based on requirements or whatever and then check, if the test vectors
fulfill the required metrics (defined by you) for structural coverage
and especially for the recommended MC/DC coverage. There are tools on
the market to measure different types of structural coverage.

Program usage
=============

The software runs as windows console application. The software must be
invoked via mcdc \<options\>

If no options are given, then a help file will be shown. All options can
(and should) be put in a text file, e.g. "options.txt". Then the program
can be invoked via mcdc --opt "options.txt"

Syntax for boolean expressions
------------------------------

A boolean expressions consists of variables (conditions) in the form of
letters 'a' through 'z' and operators. If you want to enter the negated
form, then use the uppercase letters 'A' though 'Z'. Meaning 'A' is not
'a'. Possible operations are

-   OR (binary, left associative), Operator: "+", "\|", "\|\|" all
    equivalent

-   XOR (binary, left associative), Operator: "\^"

-   AND (binary, left associative), Operator: "\*", "&", "&&",
    concatenation, all equivalent

-   NOT (unary, right associative), Operator: "!", "\~" all equivalent

-   Brackets, Operator: "(", ")"

Operator precedence is AND \--\> XOR \--\> OR. Can be overruled by
brackets.

Variable concatenation is equal to an AND operation, so "ab" is "a" AND
"b".

Example: \"a+b\^c!(d+ef)\" or "abc(a+c)!(a\^de)"

In case of wrongly formed expression, a syntax error will be shown.

Please note: Only 16 different variables are allowed. But because of the
exponential growth of processing time and memory consumption in relation
to the number of variables, the reasonable amount of input conditions is
10 or 12. Everything else will result in ultra-long and unreasonable
calculation durations.

Program Options
===============

The "\#" character can be used for inserting comments. Everything
including and after \# until end of line will be ignored.

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 1. General Options

\# -s \"boolean expression\" \# Define boolean expression to evaluate

\#-sa \# Ask for boolean expression in program

-umdnf \# Use minimum DNF resulting from QuineMcLuskey Reduction for
MCDC calculation

\# -bse \# Use boolean short cut evaluation in abstract syntax trees

\# -opt \"filename\" \# Use file \"filename\" to read options

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 2. File Handling related Options

-fd \"R:\\\" \# Base directory, where all resulting files will be
written to. Default is current directory

\# -fp \"prefix\" \# Prefix which will be prepended before all
filenames. Default is no prefix

-fo \# Overwrite all files with new data. Default, priority over -fa

\# -fa \# Append new files to existing data. Will be overwritten by -fo

-fafwsfn \# Append files with same given filenames. If for the below
files a double name is given, then the data will go into one file. in
the case of -fo, the initial file is overwritten.

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3. Output controlling Options

\# -paast \# Print all abstract syntax trees in MCDC test. Not only
those with positive MCDC outcome

\# -ppst \# Print parse stack to console

\# -sfqmt \# Show full Quine&McCluskey Reduction Table (will be a bit
slower)

\# -nomcdc \# Do Not calculate MCDC test vectors

\# -dnpast \# Do not print ASTs while searching for test pairs

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.1 Definition for all other output Options

\# -pallc \# Write all Data to console

-pallf \"all.txt\" \# Write all Data to file \"filename\"

\# -palla \# Write all Data to file with automatic created filename

\# -pallfauto \"filename\" \# Automatically write big data with lots of
lines to file \"filename\", the rest to console

\# -pallfautoa \# Automatically write big data with lots of lines to
file with automatic created filename, the rest to console. Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.2 Object code dump

\# -pcoc \# Print compiled object code to console

\# -pcof \"filename\" \# Print compiled object code to file \"filename\"

\# -pcofa \# Print compiled object code to file with automatic created
filename

\# -pcofauto \"filename\" \# Automatically write big compiled object
code to file \"filename\", write small data to console

\# -pcofautoa \# Automatically write big compiled object code to file
with automatic created filename, write small data to console. Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.3 Truth table output

\# -pttc \# Print truth table to console

\# -pttf \"filename\" \# Print truth table to file \"filename\"

\# -pttfa \# Print truth table to file with automatic created filename

\# -pttfauto \"filename\" \# Automatically write big truth table to file
\"filename\", write small data to console

\# -pttfautoa \# Automatically write big truth table to file with
automatic created filename, write small data to console. Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.4 Show Quine Mccluskey reduction table

\# -pqmtc \# Print Quine Mcluskey Reduction Tables to console

\# -pqmtf \"filename\" \# Print Quine Mcluskey Reduction Tables to file
\"filename\"

\# -pqmtfa \# Print Quine Mcluskey Reduction Tables to file with
automatic created filename

\# -pqmtfauto \"filename\" \# Automatically write big Quine Mcluskey
Reduction Tables to file \"filename\", write samll data to console

\# -pqmtfautoa \# Automatically write big Quine Mcluskey Reduction
Tables to file with automatic created filename, write samll data to
console. Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.5 Show Prime Impicant reduction table

\# -ppirtc \# Print Prime Implicant Reduction Table to console

\# -ppirtf \"filename\" \# Print Prime Implicant Reduction Table to file
\"filename\"

\# -ppirtfa \# Print Prime Implicant Reduction Table to file with
automatic created filename

\# -ppirtfauto \"filename\" \# Automatically write big Prime Implicant
Reduction Table to file \"filename\", write small data to console

\# -ppirtfautoa \# Automatically write big Prime Implicant Reduction
Table to file file with automatic created filename, write small data to
console. Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.6 Show Abstract Syntax Tree

\# -pastc \# Print Abtract Syntax Tree to console

\# -pastf \"filename\" \# Print Abtract Syntax Tree to file \"filename\"

\# -pasta \# Print Abtract Syntax Tree to file with automatic created
filename

\# -pastfauto \"filename\" \# Automatically write big Abtract Syntax
Tree to file \"filename\", write small data to console

\# -pastfautoa \# Automatically write big Abtract Syntax Tree to file
with automatic created filename, write small data to console. Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.7 Show Abstract Syntax Tree for MC/DC relevant values

\# -pmastc \# Print only MCDC relevant abtract syntac trees during brute
force evaluation to console

\# -pmastf \"filename\" \# Print only MCDC relevant abtract syntac trees
during brute force evaluation to file \"filename\"

\# -pmasta \# Print only MCDC relevant abtract syntac trees during brute
force evaluation to file with automatic created filename

\# -pmastfauto \"filename\" \# Automatically write big data for all MCDC
relevant abtract syntac trees to file \"filename\", write small data to
console

\# -pmastfautoa \# Automatically write big data for all MCDC relevant
abtract syntac trees to file with automatic created filename, write
small data to console. Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.8 Show Resulting MC/DC test pairs

\# -pmtpc \# Print all MC/DC testpairs to console

\# -pmtpf \"filename\" \# Print all MC/DC testpairs to file \"filename\"

\# -pmtpa \# Print all MC/DC testpairs to file with automatic created
filename

\# -pmtpfauto \"filename\" \# Automatically write big data for all MC/DC
testpairs to file \"filename\", write small data to console

\# -pmtpfautoa \# Automatically write big data for all MC/DC testpairs
to file with automatic created filename, write small data to console.
Default

\#
\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\-\--

\# 3.9 Show Resulting test coverage sets

\# -pmcsc \# Print all MC/DC coverage sets to console

\# -pmcsf \"filename\" \# Print all MC/DC coverage sets to file filename

\# -pmcsa \# Print all MC/DC coverage sets to console file with
automatic created filename

\# -pmcsfauto \"filename\" \# Automatically write big data for all MC/DC
coverage sets to file \"filename\", write small data to console

\# -pmcsfautoa \# Automatically write big data for all MC/DC coverage
sets to file with automatic created filename, write small data to
console. Default

Documentation of Software
=========================

The source code contains detailed explanations and many comments, to
give a detailed understanding.

Functional Parts and structure
------------------------------

The software contains the following major functional parts:

-   Compiler for Boolean Expressions

    -   Scanner

    -   Grammar LR(1) / LALR(1)

    -   Parser

    -   Code Generator

    -   Object File

    -   Symbol Table

-   Virtual Machine

-   Minterm List

-   Truth table

-   Prime Implicant Extraction - Quine/McCluskey Method

-   Coverage of Prime Implicants to Minterms

-   Prime Implicand Reduction / Selection

-   Resolving of Cyclic Core / Solving Set Cover or Unate Coverage
    Problem

-   Petrick's Method

-   Irredundant minimal Disjunctive Normal Form

-   Compiler for building the Abstract Syntax Tree

    -   Scanner

    -   Grammar LR(1) / LALR(1)

    -   Parser

    -   Code Generator for generating an Abstract Syntax Tree AST

    -   Abstract Syntax Tree AST

    -   Symbol Table

-   Definition of MCDC

-   Brute Force Checking of all possible MC/DC Test Vectors

-   Determination of Type of MC/DC

-   Resolving Set Cover Problem for Independent Test Pairs vs Conditions

-   Selection of minimum Test Vectors for MC/DC

The source C++ and header files are named accordingly.

Program execution
-----------------

In the beginning all command line options will be read, evaluated and
the program will be configured accordingly.

The boolean expression, the "source code", will be read and compiled to
object code. If there is an error, the program issues an error message
and tops.

The object code is loaded into a virtual machine. The machine code can
be shown to the user. It is a four-byte code per line, consisting of the
operation and up to 3 parameters.

A result, based on the boolean expression, is calculated for all
possible combinations of input conditions. The outputs with true will be
collected in the minterm table. The minterm table is checked for a
tautology (all true) or for a contradiction (empty table). In such a
case, the further program execution is stopped. Example: "a+A", always
true, tautology. "a && A" is always false, contradiction.

After the full minterm table is available the algorithm defined by Quine
and McCluskey is used to find the Prime Implicants of the boolean
expression. We will use here a modified version of the classical Quine &
McCluskey method. Only the largest and the smallest minterm will be
stored as a source indicator for 2 combined other minterms. This saves
drastically computation time and memory consumptions. However, if the
user wants to see the full table, the oprion "-sfqmt" may be used. For
beginners, this may be easier to understand.

Having extracted all the prime implicants, there may be redundant terms
available. These terms will be identified and eliminated in the next
step. First, essential prime implicants will be extracted, then we will
look for dominating columns and rows. Redundant stuff will be deleted.
The method will be applied iteratively until everything could be
eliminated, or a cyclic core is left. The resolution of the cyclic core
is a smart implementation of the set cover / unate coverage problem
solver. Basically, an optimized version of Petrick's method is used to
find all possible coverage sets. After that some heuristic algorithms
are used to detect a minimum irredundant disjunctive normal form (DNF).

The next step can be controlled by the flag "-umdnf". This means, we can
continue further operations with this minimized DNF or use the original
boolean expression.

It should be noted, that none minimized expression will result in longer
calculation times and higher memory consumption.

The new or original source code (boolean expression), will be compiled
again. This time into an abstract syntax tree, which can be shown to the
user.

If MC/DC test case generation is intended (default, can be switched off
with flag "-nomcdc"), a (long running) brute force algorithm is started,
which evaluates the AST for all possible pairs of test values. The
influencing tree is constructed and checked for MC/DC criteria and MC/DC
type. This will often result in many possible MC/DC test pairs per
condition. To find the minimum set of test vectors, we employ again the
set cover/unate coverage solver. With many heuristic functions, a
resulting minimum test set will be generated and recommended.
