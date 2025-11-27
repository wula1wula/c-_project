/*2. 银行ATM模拟器

项目需求概括：模拟ATM，支持存款、取款、查询余额和转账。使用类表示不同账户类型（如储蓄账户），处理无效操作。
预期目标：用户登录后，通过菜单进行银行操作，余额实时更新并显示交易历史。
可以用到的知识点：继承（基类Account，派生SavingsAccount）、多态（虚函数如withdraw()）、指针或引用传递、异常处理基础（if检查余额）。
简单测试：
输入：存款 100
输出：余额：100
输入：取款 50
输出：余额：50
输入：转账 无效账户
输出：错误：账户不存在
*/

#include <iostream>
#include"ATMSimulate.h"
#include<string>

class Account {
protected:
	int 
};
void ATMSimulate(void) {
	std::cout << "欢迎使用ATM模拟器！" << std::endl;

	while (true) {
		std::cout << "菜单：1.存款 2.取款 3.查询余额 4.转账 5.退出" << std::endl;
		std::string input;
		int choice;
		std::cin >> input;
		try {
			choice = std::stoi(input);
		}
		catch (...) {
			std::cout << "无效输入，请输入数字选项。" << std::endl;
			continue;
		}

		switch (choice) {
		case 1://存款
			
			break;
		case 2://取款

			break;
		case 3://查询余额

			break;
		case 4://转账

			break;
		case 5:
			std::cout<< "谢谢使用，再见！" << std::endl;
			return;
		default:
			std::cout << "无效选项，请重新选择。" << std::endl;
			break;
		}
	}
}