#include <iostream>

#include "calculator.h"

std::string run_calculator(std::string_view expr);

int main()
{
    do
    {
        std::string str;
        std::getline(std::cin, str);
        std::string answer = run_calculator(str);
        std::cout << answer << std::endl;
    }
    while(true);
    return 0;
}

std::string run_calculator(std::string_view expr)
{
    std::string result;
    try
    {
        result = calculator(expr);
    }
    catch (const std::exception& e)
    {
        result = std::string(e.what());
    }
    return result;
}
