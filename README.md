# Evaluation of Boolean Expressions and MCDC test cases

An application for gaining a better understanding of MCDC

- Minimization of boolean expressions
- Creation of MCDC test cases

# Brief Introduction to MCDC

# Unit Testing / Verification

Unit testing/verification strategies often contain requirements regarding
structural coverage. Metrics are used to evaluate the completeness of test 
cases and to demonstrate that there is no unintended functionality.
Analysis of structural coverage can reveal shortcomings in requirement-based 
test cases, inadequacies in requirements, dead code, deactivated code or 
unintended functionality. If the measured and achieved structural coverage 
is considered insufficient, either additional test cases shall be specified 
or a rationale shall be provided.

Software Unit Test/Verification is necessary to verify software units and
to provide evidence that for the compliance of the software units with the
software detailed design and with non-functional software requirements.

Methods for software uint testing are:
 - Requirements based test
 - Interface test
 - Fault injection test
 - Resource usage test
 - Back-to-back comparison between models and code
 - and more . . .

From the methods above, test cases need to be derived. Methods for deriving 
tests case are:
 - Analysis of requirements
 - Generation and analysis of equivalence classes
 - Analysis of boundary values
 - Error guessing
 - and more . . .

# Structural Coverage

As mentioned above, the completeness of tests can be shown by structural
coverage methods. There are many different procedures available. And the
more complex operations subsume the easier ones. Here a brief overview.

![Overview Coverage Types](https://i.stack.imgur.com/LfOAn.png) 

There are additional coverage types like Loop Coverage or Function coverage.
Also this methods will add some additional value in the area of 15-20%.

However, there is a recomendation for safety relavant software in the
avionics and the automotive industry, for example in DO178 or ISO 26262.

The metric that should be used and that is sufficient, even for safety critical
projects, is MD/DC:  Modified Condition Decision Coverage

That is the most effective method and reduces computation time and memory
consumption compared to a full MCC (Multiple Condition Coverage). Where
MCC (based on the number of conditions n) needs 2^n testcases, MCDC needs only 
n+1 test cases. With growing number of conditions, this difference is significant.

Additionally, both standards do not mention other Loop Coverage or Function
coverage or others. So, basically MCDC is sufficient.

# MCDC

Unfortunately not many test folks are understanding MCDC or structural coverage
in general and how it works. The reason for that is many the existince of many 
scattered, complicated, insufficent, partly contradicting and not up to date
documentation. Even Wikipedia is not fully complete.

A good starting point is 

[An Investigation of Three Forms of the Modified Condition Decision
Coverage (MCDC) Criterion](http://www.tc.faa.gov/its/worldpac/techrpt/ar01-18.pdf),  "DOT/FAA/AR-01/18", "U.S. Department of
Transportation, Federal Aviation Administration"

# MCDC Definitions

In most FuSa trainings you will just learn about just one type of MC/DC, the
so called "Unique Cause" MCDC. But this works in only very few cases,
because often you have strongly or weakly coupled conditions. See the
simple C-Example statement "if ((a && b) \|\| (a && c)). Here you have 2
times condition "a" in the boolean expression. A "unique cause"-MC/DC as
per the original definition is not possible. And the basic problem here
is the Blackbox approach, where you should definitely use a Whitebox view 
(know the source code) to be able to measure structural coverage.

To resolve the problematic of strongly (weakly) coupled conditions in
boolean expressions for MC/DC, additional types of MC/DC have been
defined

-   "Masking MC/DC"
-   "Unique Cause + Masking MC/DC".

Additionally, a lot of valid MCDC test pairs will not be found by looking
at truth tables. And for decisions (boolean expressions) with more than
4 variables, this is anyway not any longer a proctical approach.


# MCDC Types

To achieve MCDC you need to find at least one Test Pair for each condition 
in your boolean expression that fulfills the MCDC criterion.

At the moment there are 3 types of MCDC defined and approved by certification 
bodies (e.g. FAA):
 - "Unique Cause" – MCDC (original definition): Only one, specifically the 
 influencing condition may change between the two test values of the test pair. 
 The resulting decision, the result of the boolean expression, must also be 
 different for the 2 test values of the test pair. All other conditions in the 
 test pair must be fixed and unchanged.
 - "Masking" – MCDC”: Only one condition of the boolean expression having an
 influence on the outcome of the decision may change. Other conditions may 
 change as well, but must be masked. Masked means, using the boolean logic in 
 the expression, they will not have an influence on the outcome. If the left 
 side of an AND is FALSE, then the complete rightside  expression and sub expressions
 do not matter. They are masked. Masking is a relaxation of  the “Unique Cause”MCDC.
 - "Unique Cause + Masking" – MCDC. This is a hybrid and especially needed for boolean 
 expressions with strongly coupled conditions like for example in “ab+ac”. It is not 
 possible to find a test pair for condition “a” the fulfills "Unique Cause" – MCDC. 
 So, we can relax the original definition, and allow masking for strongly coupled conditions.

With these 2 additional definitions much more test pairs can be found.

Please note additionally that, when using languages with a boolean short cut evaluation 
strategy (like Java, C, C++), there are even more test pairs fulfilling MCDC criteria.

Very important and as already mentioned, you must understand that a BlackBox view on a truth
table does not allow to find any kind of Masking or boolean short cut evaluation test pairs.

MCDC is a structural coverage metric and so a WhiteBox View on the boolean expression 
is absolutely mandatory.


# Applied Method

To gain a better understanding on MCDC, this software hase been devolped. 
The presented software helps to illustrate the construction of MC/DC test
cases. 

Again. The main algorithms are based on:

[An Investigation of Three Forms of the Modified Condition Decision
Coverage (MCDC) Criterion](http://www.tc.faa.gov/its/worldpac/techrpt/ar01-18.pdf),  "DOT/FAA/AR-01/18", "U.S. Department of
Transportation, Federal Aviation Administration"

In this work, "colored" or "attributed" Abstract Syntax Trees (AST) are
used to create a so called "Influencing AST"

First a boolean expression is analyzed and compiled into an Abstract
Syntax Tree. The AST (abstract syntax tree) will be shown to the user.
Attributes for a "boolean value" and a "not evaluated"-tag,
in case of boolean short cut evaluation, are added. To evaluate the
MC/DC property, we need at least to test 2 values, a test pair (2 condition 
sets, or 2 input variable values) and then check with these test values, if the pre
conditions for MC/DC are fulfilled. So, we will evaluate the AST with
one value of the test pair, then we will evaluate the AST with the second value 
from the test pair. Having both AST results available, we will do a tree XOR
operation to find out, what changed. This means, we will not XOR the final results
only, but the result of each calculated node. Then we see, what changed between
both analyzed ASTs. The resulting AST (XORed) is the so called influencing tree.

To find the influencing condition, we will check the following. All nodes in the path, 
starting from the leaf to the root, must have the value 1 (TRUE). And there must be
only one path having all TRUEs. Otherwise that condition would not be "influencing".

In the case of unique cause MC/DC only one condition is allowed to change, 
and this must have an influence on the resulting decision. So no other condition in 
the influencing tree must have a value of 1.

For masking, also other conditions in the influencing tree could be 1 (meaning
changed between AST 1 and AST 2) but they must have no influence. They are masked.
for "Unique Cause + Masking" – MCDC, masking is only allowed for the influencing
condition. E.g. condition x exists in more than once in the AST, but only one
is influencing. All other conditions are 0 in the influencing tree (no change).
This always happens for strongly coupled conditions.

To emphasize again. A WhiteBox view is mandatory. This should be understood by now.

Example for boolean expression "a+b" and test vector 0,1 for condition b:

````
Found Unique Cause MCDC Test pair for condition: b Test Pair: 0 1
    ------------------------------------------------------------------------------
    AST for value: 0
    
    0 ID a (0)
    1            OR 0,2 (0)
    2 ID b (0)
    ------------------------------------------------------------------------------
    AST for value: 1

    0 ID a (0)
    1             OR 0,2 (1)
    2 ID b (1)

    ------------------------------------------------------------------------------
    AST for influencing condition check
    0 ID a (0)
    1             OR 0,2 (1)
    2 ID b (1)
````
Please note again: The "AST for influencing condition check" is a tree XOR of
the previous 2 ASTs. The Or operation result here is not the result of
"a+b", but an XOR of the OR value in the first AST and OR value of the
second AST. To find out, if a variable is influencing, only one leaf
must have changed and from this changed leaf the complete path up to the
root of the AST, every value must be 1 (This is valid for unique cause
MC/DC only).

For Masking MC/DC there must be again only one path with all trues from
the leaf to the root.

Example for Masking MC/DC for \"a+b\^c!(d+e)\" for test vectors 0, 15
(meaning 4 variables changed), but still it is MC/DC for condition b.
All other conditions are masked by boolean logic:

````
    Masking MCDC Test pair for condition: b Test Pair: 0 15
    ------------------------------------------------------------------------------
    AST for value: 0
    0         ID a (0)
    1
    2 OR 0,4 (0)       ID b (0)
    3
    4         XOR 2,6 (0)      ID c (0)
    5
    6                  AND 4,8 (0)              ID d (0)
    7                                  OR 6,8 (0)
    8                          NOT 7 (1)        ID e (0)
    9

    ------------------------------------------------------------------------------
    AST for value: 15

    0          ID a (0)
    1
    2 OR 0,4 (1)           ID b (1)
    3
    4          XOR 2,6 (1)         ID c (1)
    5
    6                     AND 4,8 (0)                  ID d (1)
    7                                        OR 6,8 (1)
    8                               NOT 7 (0)          ID e (1)
    9

    ------------------------------------------------------------------------------
    AST for influencing condition check
    0          ID a (0)
    1
    2 OR 0,4 (1)         ID b (1)
    3
    4          XOR 2,6 (1)         ID c (1)
    5
    6                    AND 4,8 (0)                 ID d (1)
    7                                      OR 6,8 (1)
    8                              NOT 7 (1)         ID e (1)
    9
````
Numbers following operations are line numbers.

Please note: Although 4 conditions changed between AST 0 and AST 15,
there is only one path with all true from leaf (condition b) to root.
This is the influencing variable. All others are masked by boolean
logic. Therefor this test vector (0, 15) is MCDC for condition b.

The developed software shows all this ASTs and helps to understand the
MC/DC logic.

# Mode of Operation


Special Settings can be used to control the mode of operation.

-   Boolean Short Cut Evaluation

-   Usage of minimum irredundant DNFs

# Boolean Short Cut Evaluation

In Languages like C and C++, boolean short cut evaluation is used for
the evaluation of boolean expressions. For example, if in "a+b+c"
condition a is true, then the complete decision is true, and b and c do
not need to be evaluated. This has of course an influence on MC/DC
considerations.

# Usage of minimum irredundant DNFs

Sometimes complex boolean expressions are given. There are often shorter
and easier equivalent expressions existing, which make evaluation for
MC/DC substantially faster and easier. The software incorporates the
Quine & McCluskey method (a modified optimized version) to minimize
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

# Caveat

The software *could* be used to create test vectors and test suites. This
should **NOT** be done.

It is important to understand that you basically should never use the
source code or a boolean expression and then derive a test vector out of
that. With that you just check, if the compiler is working correctly.
You need to understand the difference between structural test and
structural coverage. What you should do is: At first create test cases
based on requirements or whatever and then check, if the test vectors
fulfill the required metrics (defined by you) for structural coverage
and especially for the recommended MC/DC coverage. There are tools on
the market to measure different types of structural coverage.



# Program usage


The software runs as windows console application. The software must be
invoked via mcdc \<options\>

If no options are given, then a help file will be shown. All options can
(and should) be put in a text file, e.g. "options.txt". Then the program
can be invoked via mcdc --opt "options.txt"

# Syntax for boolean expressions

A boolean expressions consist of variables (conditions) in the form of
letters 'a' through 'z' and operators. If you want to enter the negated
form, then use the uppercase letters 'A' though 'Z'. Meaning 'A' is 'NOT a'. 
Possible operations are

-   OR (binary, left associative), Operator: "+", "\|", "\|\|" all
    equivalent

-   XOR (binary, left associative), Operator: "\^"

-   AND (binary, left associative), Operator: "\*", "&", "&&",
    concatenation, all equivalent

-   NOT (unary, right associative), Operator: "!", "\~" all equivalent

-   Brackets, Operator: "(", ")"

Operator precedence is AND \--\> XOR \--\> OR. Can be overruled by
brackets.

Variable concatenation is equal to an AND operation, so "ab" is "a AND b".

Example: \"a+b\^c!(d+ef)\" or "abc(a+c)!(a\^de)"

In case of wrongly formed expression, a syntax error will be shown.

Please note: Only 16 different variables are allowed. But because of the
exponential growth of processing time and memory consumption in relation
to the number of variables, the reasonable amount of input conditions is
10 or 12. Everything else will result in ultra-long and unreasonable
calculation durations.

Program Options
===============

See below an example of an options file.

Most important options are
````
-s "boolean expression"        # Define boolean expression to evaluate
-umdnf                         # Use minimum DNF resulting from QuineMcLuskey Reduction for MCDC 
-bse                           # Use boolean short cut evaluation in abstract syntax trees

-fo                            # Overwrite all files with new data. Default, priority over -fa
-fafwsfn                                            # Append files with same given filenames. If for the below files a double 
````

The "\#" character can be used for inserting comments. Everything
including and after \# until end of line will be ignored.

Most options are related to directing the output.
````
#
#------------------------------------------------------------------------------------------------------------
# 1. General Options
# -s "boolean expression"                           # Define boolean expression to evaluate
#-sa                                                # Ask for boolean expression in program
-umdnf                                              # Use minimum DNF resulting from QuineMcLuskey Reduction for MCDC calculation
# -bse                                              # Use boolean short cut evaluation in abstract syntax trees
# -opt "filename"                                   # Use file "filename" to read options
#
#------------------------------------------------------------------------------------------------------------
# 2. File Handling related Options
-fd \"R:\"                                          # Base directory, where all resulting files will bewritten to. Default is current directory
# -fp "prefix"                                      # Prefix which will be prepended before all filenames. Default is no prefix
-fo                                                 # Overwrite all files with new data. Default, priority over -fa
# -fa                                               # Append new files to existing data. Will be overwritten by -fo
-fafwsfn                                            # Append files with same given filenames. If for the below files a double name is given, then the data will go into one file. in the case of -fo, the initial file is overwritten.
#
#------------------------------------------------------------------------------------------------------------
# 3. Output controlling Options
# -paast                                            # Print all abstract syntax trees in MCDC test. Not only those with positive MCDC outcome
# -ppst                                             # Print parse stack to console
# -sfqmt                                            # Show full Quine&McCluskey Reduction Table (will be a bit slower)
# -nomcdc                                           # Do Not calculate MCDC test vectors
# -dnpast                                           # Do not print ASTs while searching for test pairs
#
#------------------------------------------------------------------------------------------------------------
# 3.1 Definition for all other output Options
# -pallc                                            # Write all Data to console
-pallf "all.txt"                                    # Write all Data to file "filename"
# -palla                                            # Write all Data to file with automatic created filename
# -pallfauto "filename"                             # Automatically write big data with lots of lines to file "filename", the rest to console
# -pallfautoa                                       # Automatically write big data with lots of lines to file with automatic created filename, the rest to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.2 Object code dump
# -pcoc                                             # Print compiled object code to console
# -pcof "filename"                                  # Print compiled object code to file "filename"
# -pcofa                                            # Print compiled object code to file with automatic created filename
# -pcofauto "filename"                              # Automatically write big compiled object code to file "filename", write small data to console
# -pcofautoa                                        # Automatically write big compiled object code to file with automatic created filename, write small data to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.3 Truth table output
# -pttc                                             # Print truth table to console
# -pttf "filename"                                  # Print truth table to file "filename"
# -pttfa                                            # Print truth table to file with automatic created filename
# -pttfauto "filename"                              # Automatically write big truth table to file "filename", write small data to console
# -pttfautoa                                        # Automatically write big truth table to file with automatic created filename, write small data to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.4 Show Quine Mccluskey reduction table
# -pqmtc                                            # Print Quine Mcluskey Reduction Tables to console
# -pqmtf "filename"                                 # Print Quine Mcluskey Reduction Tables to file "filename"
# -pqmtfa                                           # Print Quine Mcluskey Reduction Tables to file with automatic created filename
# -pqmtfauto "filename"                             # Automatically write big Quine Mcluskey Reduction Tables to file "filename", write small data to console
# -pqmtfautoa                                       # Automatically write big Quine Mcluskey Reduction Tables to file with automatic created filename, write samll data to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.5 Show Prime Impicant reduction table
# -ppirtc                                           # Print Prime Implicant Reduction Table to console
# -ppirtf "filename"                                # Print Prime Implicant Reduction Table to file "filename"
# -ppirtfa                                          # Print Prime Implicant Reduction Table to file with automatic created filename
# -ppirtfauto "filename"                            # Automatically write big Prime Implicant Reduction Table to file "filename", write small data to console
# -ppirtfautoa                                      # Automatically write big Prime Implicant Reduction Table to file file with automatic created filename, write small data to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.6 Show Abstract Syntax Tree
# -pastc                                            # Print Abtract Syntax Tree to console
# -pastf "filename"                                 # Print Abtract Syntax Tree to file "filename"
# -pasta                                            # Print Abtract Syntax Tree to file with automatic created filename
# -pastfauto "filename"                             # Automatically write big Abtract Syntax Tree to file "filename", write small data to console
# -pastfautoa                                       # Automatically write big Abtract Syntax Tree to file with automatic created filename, write small data to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.7 Show Abstract Syntax Tree for MC/DC relevant values
# -pmastc                                           # Print only MCDC relevant abtract syntac trees during brute force evaluation to console
# -pmastf "filename"                                # Print only MCDC relevant abtract syntac trees during brute force evaluation to file "filename"
# -pmasta                                           # Print only MCDC relevant abtract syntac trees during brute force evaluation to file with automatic created filename
# -pmastfauto "filename"                            # Automatically write big data for all MCDC relevant abtract syntac trees to file "filename", write small data to console
# -pmastfautoa                                      # Automatically write big data for all MCDC relevant abtract syntac trees to file with automatic created filename, write small data to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.8 Show Resulting MC/DC test pairs
# -pmtpc                                             # Print all MC/DC testpairs to console
# -pmtpf "filename"                                  # Print all MC/DC testpairs to file "filename"
# -pmtpa                                             # Print all MC/DC testpairs to file with automatic created filename
# -pmtpfauto "filename"                              # Automatically write big data for all MC/DC testpairs to file "filename", write small data to console
# -pmtpfautoa                                        # Automatically write big data for all MC/DC testpairs to file with automatic created filename, write small data to console. Default
#
#------------------------------------------------------------------------------------------------------------
# 3.9 Show Resulting test coverage sets
# -pmcsc                                            # Print all MC/DC coverage sets to console
# -pmcsf "filename"                                 # Print all MC/DC coverage sets to file filename
# -pmcsa                                            # Print all MC/DC coverage sets to console file with automatic created filename
# -pmcsfauto "filename"                             # Automatically write big data for all MC/DC coverage sets to file "filename", write small data to console
# -pmcsfautoa                                       # Automatically write big data for all MC/DC coverage sets to file with automatic created filename, write small data to console. Default
````

Documentation of Software
=========================

The source code contains detailed explanations and many comments, to
give a detailed understanding.

The Software has been build with Microsoft Visual Studio 2019 community edition. This can be downloaded and used for free for non commercial usage. The compiler options are stored in the project files, which are also attached.

The language is C++ 17.

There is also a version available that has been tested on a Raspberry PI 3B+. Compiled with g++ 8.x . Also C++ 17.

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
object code. A classical shift-reduce parser is used to analyze the syntax
of the source code (boolean expression). The compiler consists of a scanner,
a parser and a code geenerator. The compiler emits object code. Basically 
the compiler converts the booelan expression from infix to postfix.

If there is an error, the program issues an error message and tops.

The object code is loaded/linked into a virtual machine. The machine code can
be shown to the user. It is a four-byte code per line, consisting of the
operation and up to 3 parameters.

A result, based on the boolean expression, is calculated for all
possible combinations of input conditions. So, for 2^n values. 
(n is the number of conditions). The outputs with true will be
collected in a Minterm table. The minterm table is checked for a
tautology (all true) or for a contradiction (empty table). In such a
case, the further program execution is stopped. Example: "a+A", always
true, tautology. "a && A" is always false, contradiction.

A full truth table can be shown to the user, but maybe lengthy.

After the full Minterm table is available, the algorithm defined by Quine
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
eliminated, or a cyclic core is left. 

The resolution of the cyclic core is a smart implementation of the 
set cover / unate coverage problem solver. Basically, an optimized 
version of Petrick's method is used to find all possible coverage sets. 
After that some heuristic algorithms are used to detect a minimum 
irredundant disjunctive normal form (DNF).

The next step can be controlled by the flag "-umdnf". This means, we can
continue further operations with the minimized DNF or use the original
boolean expression.

It should be noted, that none minimized expressions will result in longer
calculation times and higher memory consumption.

The new or original source code (boolean expression), will be compiled
again. This time into an abstract syntax tree, which can be shown to the
user. The compiler uses the same front end, so the, same scanner and
parser. Just the code geenerator is different and creates the AST
(abstract syntax tree), stored in a linear respresentation, in a vector
but with linked elements.

If MC/DC test case generation is intended (default, can be switched off
with flag "-nomcdc"), a (long running) brute force algorithm is started,
which evaluates the AST for all possible pairs of test values. An attributed
AST for all possible input values is calculated (2^n). 

Then, an influencing tree for all possible combinations of the existing 
ASTs will be calculated. Since we need to find test pairs, every AST is
combined with all other ASTs to create an influencing tree. Then the result
is checked for MC/DC criteria and MC/DC type. The overall number of
combinations is 2^n(2^(n-1)-1)/2. So, also here we have a goemetrical
growth in calculation time depending on the number of conditions n.

All this will often result in many possible MC/DC test pairs per
condition. To find the minimum set of test pairs and with that a minimum
test vector, we employ again the set cover/unate coverage solver.
Every condition must be covered by at least one value from a test pair.

With many heuristic functions, a resulting minimum test set will be 
generated and recommended.
