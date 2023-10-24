#include <gtest/gtest.h>
#include "src/anchor.hh"
#include <string>
#include <fstream>
#include <stdio.h>

std::string runAnchor(std::string llvmAnchor)
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
    FILE* fp = popen(anchorRunCommand.c_str(), "r");
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