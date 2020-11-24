#include <cassert>
#include <iostream>

#include "calculator.h"

std::string run_calculator(std::string_view expr);
void run_test_ok();
void run_test_fail();

int main()
{
    run_test_ok();
    run_test_fail();
    return 0;
}

void run_test_ok()
{
    assert(run_calculator("-1 + 5 - 3") == "1.00");
    assert(run_calculator("-10 + (8 * 2.5) - ( 3 / 1,5)") == "8.00");
    assert(run_calculator("1 + (2 * (2.5 + 2.5 + (3 - 2))) - (3 / 1.5)") == "11.00");
    assert(run_calculator("0") == "0.00");
    assert(run_calculator("-1 - -2 - -3 - -4 - -5") == "13.00");
    assert(run_calculator("-1 * -2 * -3 * -4 * -5") == "-120.00");
    assert(run_calculator("5 + (5 * 5)") == "30.00");
    assert(run_calculator("500,0 / 3.0") == "166.67");
    assert(run_calculator("-1") == "-1.00");
    assert(run_calculator("5000000 + 5000000") == "10000000.00");

    std::cout << "Test 1 : OK \n";
}

void run_test_fail()
{
    assert(run_calculator("1.1 + 2.1 + abc") == "Некорректный ввод, строка содержит недопустимое выражение abc");
    assert(run_calculator("1 / 0") == "Запрещено делить на ноль");
    assert(run_calculator("(5 + 5") == "Отсутствует ')'");
    assert(run_calculator("Hello") == "Некорректный ввод, строка содержит недопустимое выражение Hello");
    assert(run_calculator("b * 5 + a") == "Некорректный ввод, строка содержит недопустимое выражение b");
    assert(run_calculator("5,,0") == "Некорректный ввод, строка содержит недопустимое выражение 5,,0");
    assert(run_calculator("5..0") == "Некорректный ввод, строка содержит недопустимое выражение 5..0");
    assert(run_calculator("5,.0") == "Некорректный ввод, строка содержит недопустимое выражение 5,.0");

    std::cout << "Test 2 : OK \n";
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