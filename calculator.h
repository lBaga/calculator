#ifndef CALCULATER_H
#define CALCULATER_H
#pragma once
#include <string_view>
#include <algorithm>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <cmath>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <sstream>

namespace details {

    inline const double g_pi = 4.0 * std::atan(1.0);

    struct Expression {
        Expression(std::string token);
        Expression(std::string token, Expression a);
        Expression(std::string token, Expression a, Expression b);
        std::string token;
        std::vector<Expression> args;
    };

    class Parser {
    public:
        explicit Parser(std::string_view str);
        Expression parse();
        double eval(const Expression& e);
    private:
        int get_priority(std::string_view binary_op);
        std::pair<std::string, bool> parse_token();
        Expression parse_expression();
        Expression parse_binary_expression(int min_priority);
        std::string m_str;
        const char* current_char;
    };
}

inline std::string calculator(std::string_view expr)
{
    std::ostringstream oss;
    oss.precision(2);

    details::Parser parser(expr);
    oss << std::fixed << parser.eval(parser.parse());

    return oss.str();
}

namespace details {

    inline constexpr std::string_view tokens[]
    {
        "+", "-", "^", "*", "/", "mod", "abs", "sin", "arcsin", "arccos",
        "cos","tg", "ctg", "arctg","arcctg","e","sqrt", "lg","ln", "(", ")"
    };

    inline const std::unordered_map<std::string_view, int> priorities
    {
        {"+",   1},
        {"-",   1},
        {"*",   2},
        {"/",   2},
        {"mod", 2},
        {"^",   3},
    };

    inline const std::unordered_map<std::string_view, std::function<double(double, double)>> binary_operations
    {
        {"+",   [](double a, double b) { return a + b;                      }},
        {"-",   [](double a, double b) { return a - b;                      }},
        {"*",   [](double a, double b) { return a * b;                      }},
        {"/",   [](double a, double b) { return a / b;                      }},
        {"^",   [](double a, double b) { return std::pow(a, b);             }},
        {"mod", [](double a, double b) { return (double)((int)a % (int)b);  }}
    };

    inline const std::unordered_map<std::string_view, std::function<double(double)>> unary_operations
    {
        {"+",       [](double a) { return +a;                           }},
        {"-",       [](double a) { return -a;                           }},
        {"sqrt",    [](double a) { return std::sqrt(a);                 }},
        {"abs",     [](double a) { return std::abs(a);                  }},
        {"sin",     [](double a) { return std::sin(a);                  }},
        {"arcsin",  [](double a) { return std::asin(a);                 }},
        {"cos",     [](double a) { return std::cos(a);                  }},
        {"arccos",  [](double a) { return std::acos(a);                 }},
        {"tg",      [](double a) { return std::tan(a);                  }},
        {"arctg",   [](double a) { return std::atan(a);                 }},
        {"ctg",     [](double a) { return 1.0 / tan(a);                 }},
        {"arcctg",  [](double a) { return g_pi / 2.0 - std::atan(a);    }},
        {"e",       [](double a) { return std::exp(a);                  }},
        {"ln",      [](double a) { return std::log(a);                  }},
        {"lg",      [](double a) { return std::log10(a);                }}
    };

    // Expression impl
    Expression::Expression(std::string token)
        : token(std::move(token)) {}

    Expression::Expression(std::string token, Expression a)
        : token(std::move(token)), args{ a } {}

    Expression::Expression(std::string token, Expression a, Expression b)
        : token(std::move(token)), args{ a, b } {}

    // Parser impl
    Parser::Parser(std::string_view str)
        : m_str(std::string(str)), current_char(m_str.c_str()) {}

    Expression Parser::parse()
    {
        return parse_binary_expression(0);
    }

    std::pair<std::string, bool> Parser::parse_token()
    {
        while (std::isspace(*current_char)) ++current_char;

        if (std::isdigit(*current_char))
        {
            std::string expr_number;
            while (std::isdigit(*current_char) || *current_char == '.' || *current_char == ',')
                expr_number.push_back(*current_char++);

            std::string number = expr_number;
            std::replace(std::begin(number), std::end(number), ',', '.');

            if (std::count(std::begin(number), std::end(number), '.') > 1)
                throw std::runtime_error("Некорректный ввод, строка содержит недопустимое выражение " + expr_number);

            return { number , true };
        }

        if (*current_char == 'p' && *(current_char + 1) == 'i')
        {
            std::string number = std::to_string(g_pi);
            current_char += 2;
            return { number, true };
        }


        for (auto t : tokens)
        {
            if (std::strncmp(current_char, t.data(), t.size()) == 0)
            {
                current_char += t.size();
                return { std::string(t), true };
            }
        }

        std::string wrong_expr;
        while (!std::isdigit(*current_char)
            && *current_char != '.'
            && *current_char != ','
            && !std::isspace(*current_char)
            && *current_char != '\0') wrong_expr.push_back(*current_char++);

        return { wrong_expr, false };
    }

    Expression Parser::parse_expression()
    {
        auto token = parse_token();

        if (!token.second)
        {
            throw std::runtime_error("Некорректный ввод, строка содержит недопустимое выражение " + token.first);
        }

        if (token.first == "(")
        {
            auto result = parse();
            if (parse_token().first != ")")
            {
                throw std::runtime_error("Отсутствует ')'");
            }
            return result;
        }

        if (std::isdigit(token.first.back()))
        {
            return Expression(token.first);
        }

        return Expression(token.first, parse_expression());
    }

    int Parser::get_priority(std::string_view binary_op)
    {
        if (priorities.find(binary_op) != std::end(priorities))
            return priorities.at(binary_op);
        else
            return 0;
    }

    Expression Parser::parse_binary_expression(int min_priority)
    {
        auto left_expr = parse_expression();

        while (true)
        {
            auto op = parse_token();
            auto priority = get_priority(op.first);

            if (priority <= min_priority)
            {
                current_char -= op.first.size();
                return left_expr;
            }
            auto right_expr = parse_binary_expression(priority);
            left_expr = Expression(op.first, left_expr, right_expr);
        }
    }

    double Parser::eval(const Expression& e)
    {
        switch (e.args.size())
        {
        case 0:
        {
            return std::stod(e.token.data());
        }
        case 1:
        {
            if (unary_operations.find(e.token) != std::end(unary_operations))
            {
                return unary_operations.at(e.token)(eval(e.args[0]));
            }
            else throw std::runtime_error("Неизвестная унарная операция");
        }
        case 2:
        {
            if (e.token == "/" && eval(e.args[1]) == 0.0)
                throw std::runtime_error("Запрещено делить на ноль");

            if (binary_operations.find(e.token) != std::end(binary_operations))
            {
                return binary_operations.at(e.token)(eval(e.args[0]), eval(e.args[1]));
            }
            else throw std::runtime_error("Неизвестная бинарная операция");
        }
        default:
            throw std::runtime_error("Неверный тип выражения");
        }
    }
}
#endif // CALCULATER_H
