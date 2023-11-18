#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "anchor.hh"

int main(int argc, char *argv[])
{
    std::string input = "";
    if (argc == 1)
    {
        for (std::string line; std::getline(std::cin, line);)
        {
            input += line + '\n';
        }
    }
    else
    {
        if (!std::filesystem::exists(argv[1])) 
        {
            std::cout << "Could not find file with name " << argv[1] << std::endl;
            return 1;
        }

        std::ifstream t(argv[1]);
        std::stringstream buffer;
        buffer << t.rdbuf();
        input = buffer.str();
    }

    std::string llvmOutput = anchor::compile(input);
    std::cout << llvmOutput << std::endl;

    return 0;
}