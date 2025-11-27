/*1. 简单计算器

项目需求概括：编写一个控制台程序，用户输入两个数字和一个运算符（+、-、*、/），程序计算并输出结果。需要处理除零错误，并允许用户重复计算直到退出。
预期目标：程序运行后，用户可以通过菜单选择运算，显示结果，支持多次操作。
可以用到的知识点：变量（int、double）、运算符、if-else条件、switch语句、while循环、基本I/O（<iostream>）。
简单测试：
输入：2 + 3
输出：结果：5
输入：10 / 0
输出：错误：不能除以零，请重新输入
输入：q（退出）
输出：程序结束
*/
#include<iostream>

#include<string>

#include<sstream>

#include"SimpleCalculator.h"

void SimpleCalculator(void) 
{
	std::string input;
	
	while (true) {
		getline(std::cin, input);
		if(input == "q") {
			std::cout << "程序结束" << std::endl;
			break;
		}
		double num1, num2;
		char op;
		std::istringstream ss(input);
		if (!(ss >> num1 >> op >> num2)) {
			std::cout << "输入格式错误，请重新输入`" << std::endl;
			continue;
		}
		double result;
		switch (op) {
			case '+':
				result = num1 + num2;
				std::cout << "结果：" << result << std::endl;
				break;
			case '-':
				result = num1 - num2;
				std::cout << "结果：" << result << std::endl;
				break;
			case '*':
				result = num1 * num2;
				std::cout << "结果：" << result << std::endl;
				break;
			case '/':
				if (num2 == 0) {
					std::cout << "错误：不能除以零，请重新输入" << std::endl;
					continue;
				}
				else {
					result = num1 / num2;
					std::cout << "结果：" << result << std::endl;
				}
				break;
			default:
				std::cout << "未知运算符，请重新输入" << std::endl;
				break;
		}
	}
}