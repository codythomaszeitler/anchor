#include <gtest/gtest.h>
#include "src/anchor.hh"
#include <string>
#include <fstream>
#include <stdio.h>

std::string runAnchor(const std::string& llvmAnchor)
{
    static int counter = 0;
    std::string filename = "/tmp/" + std::to_string(counter) + ".test.anchor";
    std::cout << filename << std::endl;
    counter++;
    std::ofstream testFile(filename);

    testFile << llvmAnchor;
    testFile.close();

    std::string anchorRunCommand = "/usr/local/opt/llvm/bin/lli " + filename;
    std::cout << anchorRunCommand << std::endl;

    char path[PATH_MAX];
    FILE *fp = popen(anchorRunCommand.c_str(), "r");
    if (fp == NULL)
    {
        return "Could not open subprocess to run anchor.";
    }

    std::string output = "";
    while (fgets(path, PATH_MAX, fp) != NULL)
    {
        output += std::string(path);
    }

    int status = pclose(fp);
    if (status)
    {
        return std::to_string(status) + " was returned from run anchor command";
    }
    else
    {
        return output;
    }
}

TEST(AnchorTest, ItShouldBeAbleToPrintHelloWorld)
{
    std::string sourceCode =
        R"(function integer main() {
    print("Hello, World!");
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "Hello, World!");
}

TEST(AnchorTest, ItShouldPrintStringThatIsNotHelloWorld)
{
    std::string sourceCode =
        R"(function integer main() {
    print("This is another test string!");
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "This is another test string!");
}

TEST(AnchorTest, ItShouldPrintInteger)
{
    std::string sourceCode =
        R"(function integer main() {
    print(5);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "5");
}

TEST(AnchorTest, ItShouldPrintResultOfSimpleAddition)
{
    std::string sourceCode =
        R"(function integer main() {
    print(5 + 3);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "8");
}

TEST(AnchorTest, ItShouldPrintResultOfSimpleSubtraction)
{
    std::string sourceCode =
        R"(function integer main() {
    print(5 - 3);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldPrintResultOfSimpleMultiplication)
{
    std::string sourceCode =
        R"(function integer main() {
    print(5 * 3);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "15");
}

TEST(AnchorTest, ItShouldPrintResultOfFunctionWithNonComplexInteger)
{
    std::string sourceCode = R"(

function integer foo() {
    return 1;
};

function integer main() {
    print(foo());
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "1");
}

TEST(AnchorTest, ItShouldPrintResultOfFunctionWithAnotherFunctionCall)
{
    std::string sourceCode = R"(

function integer bar() {
    return 2;
};

function integer foo() {
    return bar();
};

function integer main() {
    print(foo());
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldPrintResultOfFunctionAddedWithConstantOnLeftSide)
{
    std::string sourceCode = R"(

function integer bar() {
    return 2;
};

function integer foo() {
    return 3 + bar();
};

function integer main() {
    print(foo());
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "5");
}

TEST(AnchorTest, ItShouldPrintResultOfFunctionAddedWithConstantOnRightSide)
{
    std::string sourceCode = R"(

function integer bar() {
    print("In Bar");
    return 2;
};

function integer foo() {
    return bar() + 3;
};

function integer main() {
    print(foo());
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "In Bar5");
}

TEST(AnchorTest, ItShouldUseVariableToHoldIntegerAndPrint)
{
    std::string sourceCode = R"(
function integer main() {
    integer a;
    a = 5;
    print(a);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "5");
}

TEST(AnchorTest, ItShouldUseLocalVarInAnotherFunction)
{
    std::string sourceCode = R"(

function integer bar() {
    integer a;
    a = 3;
    return a;
};

function integer main() {
    integer a;
    a = 5;
    print(a + bar());
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "8");
}

TEST(AnchorTest, ItShouldUseTwoLocalVarsInAdditionStmt)
{
    std::string sourceCode = R"(
function integer main() {
    integer a;
    a = 5;
    integer b;
    b = 4;

    print(a + b);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "9");
}

TEST(AnchorTest, ItShouldBeAbleToUseOneArgFunction)
{
    std::string sourceCode = R"(

function integer bar(integer a) {
    return 5 + a;
};

function integer main() {
    integer a;
    a = 5;

    print(a + bar(5));
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "15");
}

TEST(AnchorTest, ItShouldBeAbleToUseTwoArgFunctions)
{
    std::string sourceCode = R"(

function integer bar(integer a, integer b) {
    return 5 + a + b;
};

function integer main() {
    integer a;
    a = 5;

    print(a + bar(5, 5));
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "20");
}

TEST(AnchorTest, ItShouldPrintTrueExpression)
{
    std::string sourceCode = R"(

function integer main() {
    print(true);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "1");
}

TEST(AnchorTest, ItShouldPrintFalseExpression)
{
    std::string sourceCode = R"(

function integer main() {
    print(false);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "0");
}

TEST(AnchorTest, ItShouldDeclAndAssignBooleanToTrue)
{
    std::string sourceCode = R"(

function integer main() {
    boolean a;
    a = true;
    print(a);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::cout << llvmAnchor << std::endl;
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "1");
}

TEST(AnchorTest, ItShouldDeclAndAssignBooleanToFalse)
{
    std::string sourceCode = R"(

function integer main() {
    boolean a;
    a = false;
    print(a);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "0");
}

TEST(AnchorTest, ItShouldRunLessThanBooleanExpression)
{
    std::string sourceCode = R"(

function integer main() {
    print(3 < 5);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "1");
}

TEST(AnchorTest, ItShouldTakeIfBranchIfConditionSuccessful)
{
    std::string sourceCode = R"(

function integer main() {
    if (3 < 5) 
    {
        print ("2");
    };
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldTakeIfBranchIfConditionNotSuccessful)
{
    std::string sourceCode = R"(

function integer main() {
    if (5 < 3) 
    {
        print ("2");
    };
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "");
}

TEST(AnchorTest, ItShouldTakeWhileLoopWhileConditionTrue)
{
    std::string sourceCode = R"(

function integer main() {
    integer a;
    a = 0;
    while (a < 3) 
    {
        print (a);
        a = a + 1;
    };
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "012");
}

TEST(AnchorTest, ItShouldDoFunctionCallWithWhileLoopInIt)
{
    std::string sourceCode = R"(

function integer loopMe() {
    integer a;
    a = 0;
    while (a < 3) 
    {
        a = a + 1;
    };
    return a;
};

function integer main() {
    integer a;
    a = loopMe();
    print (a); 
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "3");
}

TEST(AnchorTest, ItShouldUseIfStmtWithGreaterThan)
{
    std::string sourceCode = R"(
function integer main() {
    if (5 > 3) 
    {
        print ("2");
    };
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldUseDoubleEqualsOnTrue)
{
    std::string sourceCode = R"(
function integer main() {
    if (3 == 3) 
    {
        print ("2");
    };
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldUseDoubleEqualsOnFalse)
{
    std::string sourceCode = R"(
function integer main() {
    if (3 == 2) 
    {
        print ("2");
    };
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "");
}

TEST(AnchorTest, ItShouldDoStringVarDeclAndAssign)
{
    std::string sourceCode = R"(

function integer bar() {
    string a;
    a = "2";
    print(a);
    return 0;
};

function integer main() {
    string a;
    a = "Hello, World!";
    print(a); 
    integer b;
    b = bar();
    print(b);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "Hello, World!20");
}

TEST(AnchorTest, ItShouldSeePrintStatementFromVoidFunction)
{
    std::string sourceCode = R"(
function void bar() {
    print("Hello, World!");
};

function integer main() {
    bar(); 
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "Hello, World!");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateTwoStrings)
{
    std::string sourceCode = R"(
function integer main() {
    string a;
    a = "2";

    string b;
    b = "3";

    print(a + b);

    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "23");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateTwoStringsWhenRhsIsEmpty)
{
    std::string sourceCode = R"(
function integer main() {
    string a;
    a = "2";

    string b;
    b = "";

    print(a + b);

    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateTwoStringsWhenLhsIsEmpty)
{
    std::string sourceCode = R"(
function integer main() {
    string a;
    a = "";

    string b;
    b = "2";

    print(a + b);

    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateTwoStringsWhenBothAreEmpty)
{
    std::string sourceCode = R"(
function integer main() {
    string a;
    a = "";

    string b;
    b = "";

    print(a + b);

    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateIntoVariable)
{
    std::string sourceCode = R"(
function integer main() {
    string a;
    a = "2";

    string b;
    b = "3";

    string c;
    c = a + b;

    print(c);

    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "23");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateWithThreeStrings)
{
    std::string sourceCode = R"(
function integer main() {
    string a;
    a = "2";

    string b;
    b = "3";

    string c;
    c = a + b;

    string d;
    d = c + "4";

    print(d);

    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "234");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateTwoLiterals)
{
    std::string sourceCode = R"(
function integer main() {
    string a;
    a = "2" + "3";
    print(a);

    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "23");
}

TEST(AnchorTest, ItShouldBeAbleToConcatenateTwoLiteralsWithinAFunction)
{
    std::string sourceCode = R"(

function void foo() {
    print("2" + "3");
};

function integer main() {
    foo();
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "23");
}

TEST(AnchorTest, ItShouldBeAbleToReturnAStringFromAFunction)
{
    std::string sourceCode = R"(

function string foo() {
    return "2";
};

function integer main() {
    string a;
    a = foo();
    print (a);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "2");
}

TEST(AnchorTest, ItShouldBeAbleToConcatStringInReturnStmt)
{
    std::string sourceCode = R"(

function string foo() {
    return "2" + "3";
};

function integer main() {
    string a;
    a = foo();
    print (a);
    return 0;
};)";

    std::string llvmAnchor = anchor::compile(sourceCode);
    std::string output = runAnchor(llvmAnchor);
    EXPECT_EQ(output, "23");
}

TEST(AnchorTest, ItShouldThrowCompileTimeErrorIfAddingStringAndIntTypes)
{
    std::string sourceCode = R"(

function integer main() {
    string a;
    a = "3";
    integer b;
    b = 2;

    print (a + b);

    return 0;
};)";
    std::string llvmAnchor = anchor::compile(sourceCode);
    EXPECT_EQ(llvmAnchor, "Type Error: Expression at line 9, column 12 had STRING on left, INTEGER on right.\n");
}