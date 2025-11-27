/*5. 登录与注册系统

项目需求概括：程序允许用户注册（输入用户名、密码，保存到文件），然后登录验证。支持多次尝试，文件持久化。
预期目标：模拟简单用户系统，数据存文件，下次运行可读取。
可以用到的知识点：字符串（<string>）、文件I/O（<fstream>）、if-else验证、循环尝试、基本结构体存储用户数据。
简单测试：
输入：注册 用户名：admin 密码：123
输出：注册成功
输入：登录 用户名：admin 密码：123
输出：登录成功
输入：登录 用户名：admin 密码：456
输出：密码错误，请重试*/

#include <iostream>
#include<fstream>
#include<vector>
#include"ManagementSystem.h"
#include <string>


void ManagementSystem(void) {
	std::cout << "欢迎使用登录与注册系统！" << std::endl;
	// 这里可以添加更多的管理系统功能
	std::string Username;
	std::string Password;
	std::string input;
	int choice;

	while (true) {
		std::cout << "菜单：1.注册 2.登录 3.退出" << std::endl;
		std::cin >> input;
		try {
			choice = std::stoi(input);
		}catch (...) {
			std::cout << "请选择菜单中提供的选项" << std::endl;
			continue;
		}
		std::ifstream RFile("UserData.txt");
		std::vector<std::string> lines;
		std::string line;
		bool isFind = false;
		std::string FindWord;
		bool success = false; //登录是否成功

 		switch (choice) {
		case 1: //注册
			std::cout << "请输入用户名：" << std::endl;
			std::cin >> Username;
			std::cout << "请输入密码：" << std::endl;
			std::cin >> Password;
			isFind = false;

			//读取文件中数据
			if (RFile.is_open()) {
				while (std::getline(RFile, line)) {
					lines.push_back(line);
				}
			}
			RFile.close();

			//检查用户名是否存在
			for (int i = 0; i < lines.size(); i++) {
				if (lines[i].find(Username) != std::string::npos) {
					std::cout << "用户名已存在，请重新注册" << std::endl;
					isFind = true;
				}
			}
			
			//写入用户名和密码
			if (!isFind) {
				std::ofstream WFile("UserData.txt",std::ios::app);
				if (WFile.is_open()) {
					WFile << Username <<":" << Password << "\n";
					std::cout << "注册成功" << std::endl;
					WFile.close();
				}
				else {
					std::cout << "文件未知错误" << std::endl;
					return;
				}
			}
			break;
			
		case 2: //登录
			std::cout << "请输入用户名：" << std::endl;
			std::cin >> Username;
			std::cout << "请输入密码：" << std::endl;
			std::cin >> Password;
			success = false;

			//拼合查找词汇
			FindWord = Username + ":" + Password;

			//读取文件中数据
			if (RFile.is_open()) {
				while (std::getline(RFile, line)) {
					lines.push_back(line);
				}
			}
			RFile.close();

			for (int i = 0; i < lines.size(); i++) {
				if (lines[i].find(FindWord) != std::string::npos) {
					success = true;
					std::cout << "登录成功" << std::endl;
				}
			}
			if (!success) {
				std::cout << "登录失败" << std::endl;
			}
			break;

		case 3: //退出
			std::cout << "感谢使用，再见！" << std::endl;
			return;
		default:
			std::cout << "请选择菜单中提供的选项" << std::endl;
			break;
		}
	}
}