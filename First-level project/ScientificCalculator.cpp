/*4. 科学计算器

项目需求概括：扩展简单计算器，支持sin、cos、pow、sqrt等数学函数。用户输入函数名和参数，计算输出。处理无效输入。
预期目标：程序像计算器一样处理科学运算，显示结果。
可以用到的知识点：数学库（<cmath>）、switch处理函数选择、变量（double）、循环重复计算、异常基本处理（if检查）。
简单测试：
输入：sin 90
输出：结果：1
输入：sqrt 16
输出：结果：4
输入：pow 2 3
输出：结果：8*/

#include"ScientificCalculator.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <cmath>
#include <cctype>  // for tolower

// 辅助函数：标准化输入（去除多余空格）
std::string normalize(const std::string& input) {
    std::stringstream ss;
    bool space = false;
    for (char c : input) {
        if (std::isspace(c)) {
            if (!space) {
                ss << ' ';
                space = true;
            }
        }
        else {
            ss << c;
            space = false;
        }
    }
    std::string result = ss.str();
    if (!result.empty() && std::isspace(result.back())) {
        result.pop_back();
    }
    return result;
}

// 辅助函数：分词（按空格）
std::vector<std::string> tokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// 简单算术执行（仅支持两个操作数，如"2+3"）
double evaluate_arithmetic(const std::string& expr) {
    std::regex pattern(R"(\s*([+-]?\d*\.?\d+)\s*([+\-*/])\s*([+-]?\d*\.?\d+)\s*)");
    std::smatch matches;
    if (std::regex_match(expr, matches, pattern)) {
        double a = std::stod(matches[1].str());
        char op = matches[2].str()[0];
        double b = std::stod(matches[3].str());
        switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : NAN;
        default: return NAN;
        }
    }
    return NAN;  // 无效
}

constexpr double M_PI = 3.14159265358979323846;
// 函数执行
double evaluate_function(const std::string& func_name, const std::vector<std::string>& args) {
    if (args.empty()) return NAN;
    double arg1 = std::stod(args[0]);
    if (func_name == "sin") {
        return std::sin(arg1 * M_PI / 180.0);  // 度数转弧度
    }
    else if (func_name == "cos") {
        return std::cos(arg1 * M_PI / 180.0);
    }
    else if (func_name == "pow" && args.size() >= 2) {
        double arg2 = std::stod(args[1]);
        return std::pow(arg1, arg2);
    }
    else if (func_name == "sqrt" && args.size() >= 1) {
        return std::sqrt(arg1);
    }
    return NAN;  // 未支持
}

// 主解析函数
std::string parse_and_compute(const std::string& input_str) {
    std::string normalized = normalize(input_str);
    std::vector<std::string> tokens = tokenize(normalized);

    if (tokens.empty()) return "空输入";

    if (tokens.size() == 1) {
        std::string expr = tokens[0];
        // 检查函数(参数)，如 sin(30)
        std::regex func_regex(R"(^([a-zA-Z]+)\(\s*([+-]?\d*\.?\d+)\s*\)$)");
        std::smatch matches;
        if (std::regex_match(expr, matches, func_regex)) {
            std::string func_name = matches[1].str();
            std::transform(func_name.begin(), func_name.end(), func_name.begin(), ::tolower);
            double arg = std::stod(matches[2].str());
            double result = evaluate_function(func_name, { std::to_string(arg) });
            if (!std::isnan(result)) {
                return std::to_string(result);
            }
            return "不支持的函数: " + func_name;
        }

        // 检查算术表达式
        if (std::regex_search(expr, std::regex("[+\\-*/]"))) {
            double result = evaluate_arithmetic(expr);
            if (!std::isnan(result)) {
                return std::to_string(result);
            }
            return "无效算术表达式";
        }
        return "单值或未知";
    }
    else if (tokens.size() >= 2) {
        std::string first = tokens[0];
        std::transform(first.begin(), first.end(), first.begin(), ::tolower);
        if (first == "sin" || first == "cos" || first == "pow" || first == "sqrt") {
            std::vector<std::string> args(tokens.begin() + 1, tokens.end());
            // 验证args为数字
            bool valid = true;
            for (const auto& arg : args) {
                try {
                    std::stod(arg);
                }
                catch (...) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                double result = evaluate_function(first, args);
                if (!std::isnan(result)) {
                    return std::to_string(result);
                }
            }
            return "无效函数参数";
        }
        else {
            // 检查带空格的算术，如 2 + 3
            std::string full_expr;
            for (const auto& t : tokens) full_expr += t;
            double result = evaluate_arithmetic(full_expr);
            if (!std::isnan(result)) {
                return std::to_string(result);
            }
            return "无效表达式";
        }
    }
    return "未知输入";
}

void ScientificCalculator(void){
    std::string input;
    std::cout << "输入表达式 (如 2+3, sin 30, pow 2 3): ";
    while (std::getline(std::cin, input)) {
        if (input == "q") break;
        std::string result = parse_and_compute(input);
        std::cout << "结果: " << result << std::endl;
        std::cout << "输入下一个: ";
    }
}