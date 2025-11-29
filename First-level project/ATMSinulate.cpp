/*2. 银行ATM模拟器

编写一个控制台程序，模拟银行ATM系统。系统支持多个用户账户（每个账户有唯一ID、密码、初始余额和账户类型）。用户先通过ID和密码登录，然后通过菜单进行存款、取款、余额查询、转账（到其他账户）和交易历史查看。
数据需持久化到文件（e.g., accounts.txt），以便程序重启后保留。处理常见错误，如密码错误、余额不足。

用户信息结构：使用类表示账户。
基类Account：包含ID（string）、密码（string）、余额（double）、交易历史（vector<string>）。
派生类SavingsAccount：继承自Account，添加年利率（double，默认0.05），存款时自动加息（简化：余额 *= 1 + 利率）。
派生类CheckingAccount：继承自Account，无额外属性，但取款有最低余额检查（e.g., 不能低于100）。

登录：用户输入ID和密码验证（最多3次尝试，失败退出）。
菜单选项（登录后）：
存款（输入金额 >0）。
取款（输入金额，确保余额足够；派生类额外检查）。
查询余额（显示当前余额）。
转账（输入目标ID、金额；验证目标账户存在，扣款/加款）。
查看历史（显示最近5笔交易，如“2023-10-01 存款 100”）。
退出（保存数据）。

文件持久化：登录时从文件加载所有账户；退出时保存所有账户（格式：ID|密码|余额|类型|历史JSON简化串）。

预期目标
程序运行后，用户可模拟完整ATM流程：多账户管理、类型特定规则、多态调用（e.g., 通过基类指针调用withdraw()），并确保数据安全持久化。整个系统菜单驱动，交互友好，错误提示明确。
可用到的知识点

继承与多态：基类Account定义虚函数（如virtual bool withdraw(double amount)），派生类override实现具体逻辑（e.g., Savings加息）。
类与成员：私有成员（余额、密码），公有getter/setter；vector存储历史。
指针/引用：用map<string, Account*>存储账户（键为ID），通过指针实现多态。
异常处理基础：用if-else或try-catch处理输入错误（e.g., 无效金额用std::invalid_argument）。
文件I/O：<fstream>和<sstream>解析/写入文件。
其他：<vector>、<map>、<string>、时间戳（<ctime>简单日期）。
*/

#include <iostream>
#include"ATMSimulate.h"
#include<string>
#include<vector>
#include<ctime>
#include <iomanip>
#include<sstream>
#include<stdexcept>
#include<fstream>
#include<nlohmann/json.hpp>
#include <memory>

class Account {
protected:
	std::string ID;
	std::string password;
	double balance;
	std::vector<std::string> history;
public:
	Account(std::string id, std::string pwd, double bal =0.0)
		: ID(id), password(pwd), balance(bal) {}
	virtual ~Account() {}
	std::string getId() const { return ID; }
	std::string getPsw() const { return password; }
	double getBalance() const { return balance; }
	std::vector<std::string> getHistory() const { return history; }	//存款
	virtual bool deposit(double amount){
		if (amount <= 0) {
			throw std::invalid_argument("金额无效");
		}
		balance += amount;
		addHistory("存款" + std::to_string(amount));
		return true;
	}
	//取款
	virtual bool withdraw(double amount) {
		if (amount <= 0) {
			throw std::invalid_argument("金额无效");
		}
		balance -= amount;
		addHistory("取款" + std::to_string(amount));
		return true;
	}
	//余额查询
	void checkBalance()  {
		std::cout << "余额：" << balance << std::endl;;
	}
	//转账
	bool transfer() {
		//TODO
		return true;
	}
	//查看交易历史
	void viewHistory() {
		std::cout << "所有交易记录：" << std::endl;
		for (int i = 0; i < history.size(); i++) {
			std::cout << history[i] <<"\n"<< std::endl;
		}
	}
	//添加交易历史记录
	void addHistory(std::string mes){
		history.push_back(getTimeStamp() + " :" + mes);
	}
	//获取当前时间戳
	std::string getTimeStamp() {
		std::time_t now = std::time(nullptr);
		std::tm* ltm = std::localtime(&now);
		std::stringstream ss;
		ss << std::put_time(ltm, "%Y-%m-%d");
		return ss.str();
	}
	//账户类型
	virtual std::string getType() {
		return "Account";
	}
};

class SavingsAccount : public Account {
private:
	double interestRate = 0.05;
public:
	SavingsAccount(std::string id,std::string password,double bal =0.0):Account(id,password,bal){}
	bool deposit(double amount) override {
		Account::deposit(amount);
		balance *= (1 + interestRate);
		addHistory("存款：" + std::to_string(balance) + "(加息后)");
		return true;
	}
	std::string getType() override{
		return "SavingsAccount";
	}
};

class CheckingAccount : public Account {
public:
	CheckingAccount(std::string id, std::string password, double bal = 0.0) :Account(id, password, bal) {}
	bool withdraw(double amount) override {
		if (balance - amount < 100) {
			throw std::invalid_argument("余额不得低于100");
		}
		else {
			return Account::withdraw(amount);
		}
	}
	std::string getType() override {
		return "CheckingAccount";
	}
};

//加载函数
void loadFile() {
	std::ifstream ifs("accounts.txt");
	if (!ifs.is_open()) {
		std::cout << "文件打开失败!" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(ifs, line)) {
		if (line.empty()) continue;


		std::stringstream ss(line);
		//（格式：ID | 密码 | 余额 | 类型 | 历史JSON简化串）。
		std::string id;
		std::string psw;
		std::string balStr;
		double bal;
		std::string type;
		std::string hisStr;
		if (
			!std::getline(ss, id, '|') ||
			!std::getline(ss, psw, '|') ||
			!std::getline(ss, balStr, '|') ||
			!std::getline(ss, type, '|') ||
			!std::getline(ss, hisStr, '|')
			) {
			std::cout << "文件格式错误!" << std::endl;
			continue;
		}

		//余额转换
		try {
			bal = std::stod(balStr);
		}
		catch (...) {
			std::cout << "余额转换失败" << std::endl;
			continue;
		}

		std::unique_ptr<Account> acc;
		if (type == "SavingsAccount") {
			acc = std::make_unique<SavingsAccount>(id, psw, bal);
		}
		else if (type == "CheckingAccount") {
			acc = std::make_unique<CheckingAccount>(id, psw, bal);
		}
		else {
			std::cout << "未知账户类型：" << type << std::endl;
			continue;
		}
		//解析历史记录
		//去除空格
		size_t start = hisStr.find_first_not_of(" \t");
		if (start == std::string::npos) start = 0;
		size_t end = hisStr.find_last_not_of(" \t");
		if (start == std::string::npos) end = hisStr.size() - 1;
		hisStr = hisStr.substr(start, end - start + 1);
		try {
			nlohmann::json j = nlohmann::json::parse(hisStr);
			std::vector<std::string> hisVec = j;
			for (const std::string& s : hisVec) {
				std::cout << s <<"\n"<< std::endl;
				acc->addHistory(s);
			}
		}catch(...){
			std::cout << "无效历史记录!" << std::endl;
			continue;
		}
	}
	ifs.close();
}

//保存函数
void saveFile(Account& acc) {
	std::ifstream ifs("accounts.txt");
	if (!ifs.is_open()) {
		std::cout << "保存数据时文件打开失败" << std::endl;
		return;
	}
	//考虑用户已存在，更新用户信息
	std::ofstream tempFile("temp.txt");
	if (!tempFile.is_open()) {
		std::cout << "临时文件创建失败" << std::endl;
		return;
	}
	std::string data;
	bool isfind = false;
	while (std::getline(ifs, data)) {
		std::string id;
		std::string psw;
		std::istringstream iss(data);
		if (!std::getline(iss, id, '|')) {
			std::cout<<"文件数据解析失败"<<std::endl;
			return;
		}
		if (id == acc.getId()) {
			//更新数据操作
			nlohmann::json j = acc.getHistory();
			std::string jsonStr = j.dump(4);
			tempFile << acc.getId() << "|" << acc.getPsw() << "|" << std::to_string(acc.getBalance()) << "|" << acc.getType() << "|" << jsonStr << "\n" << std::endl;
			isfind = true;
		}
		tempFile << data;
	}
	ifs.close();
	tempFile.close();
	if (std::remove("accounts.txt") != 0) {
		std::cout << "删除原文件失败" << std::endl;
		std::remove("temp.txt");
		return;
	}
	if (std::rename("temp.txt", "accounts.txt") != 0) {
		std::cout << "临时文件重命名失败" << std::endl;
		std::remove("temp.txt");
		return;
	}
	if (!isfind) {
		std::ofstream ofs("accounts.txt", std::ios::app);
		if (!ofs.is_open()) {
			std::cout << "文件打开失败" << std::endl;
			return;
		}
		nlohmann::json j = acc.getHistory();
		std::string jsonStr = j.dump(4);
		ofs << acc.getId() << "|" << acc.getPsw() << "|" << std::to_string(acc.getBalance()) << "|" << acc.getType() << "|" << jsonStr << "\n" << std::endl;
		
		ofs.close();
	}
}

void Userlogin() {
	//登录：用户输入ID和密码验证（最多3次尝试，失败退出）


}
void ATMSimulate(void) {
	std::cout << "欢迎使用ATM模拟器！" << std::endl;

	while (true) {
		//登录
		Userlogin();
		std::cout << "菜单：1.存款 2.取款 3.查询余额 4.转账 5.交易历史 6.退出" << std::endl;
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
		case 5://交易历史

			break;
		case 6:
			std::cout<< "谢谢使用，再见！" << std::endl;
			return;
		default:
			std::cout << "无效选项，请重新选择。" << std::endl;
			break;
		}
	}
}