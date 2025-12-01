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
#include<map>
#include<utility>
#include <limits>
#define NOMINMAX
#include <windows.h>

//声明Account类
class Account;

std::map<std::string, std::unique_ptr<Account>> accountMap;
class Account {
protected:
	std::string ID;
	std::string password;
	double balance;
public:
	std::vector<std::string> history;
	Account(std::string id, std::string pwd, double bal =0.0)
		: ID(id), password(pwd), balance(bal) {}
	virtual ~Account() {}
	std::string getId() const { return ID; }
	std::string getPsw() const { return password; }
	double getBalance() const { return balance; }
	std::vector<std::string> getHistory() const { return history; }	
	//存款
	virtual bool deposit(double amount){
		if (amount <= 0) {
			return false;
		}
		balance += amount;
		addHistory("存款" + std::to_string(amount));
		return true;
	}
	//取款
	virtual bool withdraw(double amount) {
		if (amount <= 0) return false;
		if (balance < amount) return false;
		balance -= amount;
		addHistory("取款" + std::to_string(amount));
		return true;
	}
	//余额查询
	void checkBalance()  {
		std::cout << "余额：" << balance << std::endl;;
	}
	//转账
	bool transfer(const std::string& id,double amount) {
		if (amount <= 0) return false;
		auto it = accountMap.find(id);
		if (it == accountMap.end()) return false;
		if (balance < amount) return false;
		balance -= amount;
		bool depSuccess = it->second->deposit(amount);
		if (!depSuccess) {
			balance += amount;
			return false;
		}
		addHistory("转账" + std::to_string(amount) + "到账户" + id);
		it->second->addHistory("收到转账" + std::to_string(amount) + "来自账户" + ID);
		return true;
	}
	//查看交易历史
	void viewHistory() {
		std::cout << "最近5笔交易记录：" << std::endl;
		size_t start = history.size() >= 5 ? history.size() - 5 : 0;
		for (size_t i = start; i < history.size();++i) {
			std::cout << history[i] << std::endl;
		}
		if (history.empty()) {
			std::cout << "无交易记录。" << std::endl;
		}
	}
	//添加交易历史记录
	void addHistory(std::string mes){
		try {
			history.push_back(getTimeStamp() + " :" + mes);
		}
		catch (const std::invalid_argument& e) {
			std::cout << e.what() << std::endl;
		}
		catch (...) {
			std::cout << "添加交易记录失败，未知错误！" << std::endl;
		}
	}
	//获取当前时间戳
	std::string getTimeStamp() {
		std::time_t now = std::time(nullptr);  // 获取当前时间戳
		if (now == -1) {  // time() 失败检查
			throw std::invalid_argument("时间获取失败");  // 或抛出异常
		}

		std::tm timeinfo;  // 预声明 tm 缓冲区
		errno_t err = localtime_s(&timeinfo, &now);  // 安全转换
		if (err != 0) {  // 检查错误
			throw std::invalid_argument("时间转换失败"); // 或 std::perror("localtime_s failed");
		}

		std::stringstream ss;
		ss << std::put_time(&timeinfo, "%Y-%m-%d");  // 格式化：YYYY-MM-DD
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
	SavingsAccount(std::string id, std::string password, double bal = 0.0) :Account(id, password, bal) {}
	bool deposit(double amount) override {
		if (!Account::deposit(amount)) return false;  // 修改：调用基类（检查+历史）
		balance *= (1 + interestRate);
		if (!history.empty()) {
			history.back() += " (加息后余额: " + std::to_string(balance) + ")";  // 修改：附加到存款记录
		}
		return true;
	}
	std::string getType() override {
		return "SavingsAccount";
	}
};

class CheckingAccount : public Account {
public:
	CheckingAccount(std::string id, std::string password, double bal = 0.0) :Account(id, password, bal) {}
	bool withdraw(double amount) override {
		if (balance - amount < 100) {
			return false;
		}
		return Account::withdraw(amount);
	}
	std::string getType() override {
		return "CheckingAccount";
	}
};
// 辅助函数：UTF-8 to GBK 转换
std::string UTF8ToGBK(const std::string& utf8Str) {
	if (utf8Str.empty()) return utf8Str;
	// UTF-8 -> WideChar (Unicode)
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
	if (len <= 0) return utf8Str;  // 转换失败，返回原
	std::wstring wstr(len, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstr[0], len);
	// WideChar -> GBK (CP_ACP = 936 for Simplified Chinese GBK)
	len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	if (len <= 0) return utf8Str;
	std::string gbk(len, 0);
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &gbk[0], len, NULL, NULL);
	return gbk.substr(0, len - 1);  // 移除 null 终止符
}
//加载函数
void loadFile() {
	std::ifstream ifs("accounts.txt");
	if (!ifs.is_open()) {
		std::cout << "文件不存在，将创建新文件。" << std::endl;  // 可选提示
		return;  // 修复：不创建空文件，直接返回（注册会 save）
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
		size_t start =  hisStr.find_first_not_of(" \t");
		if (start == std::string::npos) start = 0;
		size_t end = hisStr.find_last_not_of(" \t");
		if (start == std::string::npos) end = hisStr.size() - 1;
		hisStr = hisStr.substr(start, end - start + 1);
		try {
			nlohmann::json j = nlohmann::json::parse(hisStr);
			std::vector<std::string> hisVec = j;
			for (const std::string& s : hisVec) {
				acc->history.push_back(UTF8ToGBK(s));
			}
		}catch(...){
			std::cout << "无效历史记录!" << std::endl;
		}
		//存入全局账户映射
		accountMap[id] = std::move(acc);
	}
	ifs.close();
}

// 辅助函数：GBK to UTF-8 转换
std::string GBKToUTF8(const std::string& gbkStr) {
	if (gbkStr.empty()) return gbkStr;
	// GBK -> WideChar (Unicode)
	int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);  // CP_ACP = GBK
	std::wstring wstr(len, 0);
	MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, &wstr[0], len);
	// WideChar -> UTF-8
	len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	std::string utf8(len, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8[0], len, NULL, NULL);
	return utf8.substr(0, len - 1);  // 移除 null 终止符
}

//保存函数
void saveFile() {
	std::ofstream ofs("accounts.txt");
	if (!ofs.is_open()) {
		std::cout << "文件打开失败" << std::endl;
		return;
	}
	for (const auto& pair : accountMap) {
		try {
			// 修改：转换 history 每个 string 为 UTF-8
			std::vector<std::string> utf8History;
			for (const std::string& h : pair.second->getHistory()) {
				utf8History.push_back(GBKToUTF8(h));  // 转换
			}
			nlohmann::json j = utf8History;  // 用转换后的 vector
			std::string jsonStr = j.dump(-1);
			ofs << pair.second->getId() << "|" << pair.second->getPsw() << "|"
				<< std::to_string(pair.second->getBalance()) << "|" << pair.second->getType() << "|"
				<< jsonStr << "\n";  // "\n" 加速
		}
		catch (const nlohmann::json::exception& e) {
			std::cout << "JSON 序列化失败 for " << pair.first << ": " << e.what() << std::endl;
			continue;
		}
		catch (...) {
			std::cout << "保存异常 for " << pair.first << std::endl;
			break;
		}
	}
	ofs.flush();
	ofs.close();
	std::cout << "保存完成。" << std::endl;
}
//void saveFile() {
//		std::ofstream ofs("accounts.txt");
//		if (!ofs.is_open()) {
//			std::cout << "文件打开失败" << std::endl;
//			return;
//		}
//		for (const auto& pair : accountMap) {
//			nlohmann::json j = pair.second->getHistory();
//			std::string jsonStr = j.dump(0);
//			ofs << pair.second->getId() << "|" << pair.second->getPsw() << "|" << std::to_string(pair.second->getBalance()) << "|" << pair.second->getType() << "|" << jsonStr << std::endl;
//		}
//		ofs.close();
//}

std::string Userlogin() {
	while (true) {
		std::cout << "请选择：1.登录 2.注册" << std::endl;
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
		std::string newId;
		std::string newPsw;
		std::string accType;
		int Failure = 0;
		switch (choice) {
		case 1:
			//登录：用户输入ID和密码验证（最多3次尝试，失败退出）
			while (Failure < 3) {
				std::string id;
				std::string psw;
				std::cout << "请输入账户ID：" << std::endl;
				std::cin >> id;
				std::cout << "请输入密码：" << std::endl;
				std::cin >> psw;
				auto it = accountMap.find(id);
				if (it != accountMap.end() && it->second->getPsw() == psw) {
					return id;
				}
				else {
					std::cout << "ID或密码错误，请重试。" << std::endl;
					Failure++;
				}
			}
			std::cout << "登录失败次数过多，程序退出。" << std::endl;
			exit(0);
		case 2:
			//注册
			std::cout << "请输入新账户ID：" << std::endl;
			std::cin >> newId;
			if (accountMap.find(newId) != accountMap.end()) {
				std::cout << "账户ID已存在，请选择其他ID。" << std::endl;
				break;
			}
			std::cout << "请输入新账户密码：" << std::endl;
			std::cin >> newPsw;
			std::cout << "请选择账户类型：1.储蓄账户 2.支票账户" << std::endl;
			std::cin >> accType;
			if (accType == "1") {
				accountMap[newId] = std::make_unique<SavingsAccount>(newId, newPsw);
			}
			else if (accType == "2") {
				accountMap[newId] = std::make_unique<CheckingAccount>(newId, newPsw);
			}
			else {
				std::cout << "请选择有效的账户类型！" << std::endl;
				continue;
			}
			std::cout << "注册成功！" << std::endl;
			return newId;
			break;
		default:
			std::cout << "无效选项，请重新选择。" << std::endl;
			break;
		}
	}
}
void ATMSimulate(void) {
	std::cout << "欢迎使用ATM模拟器！" << std::endl;
	loadFile();
	while (true) {
		//登录
		std::string id = Userlogin();
		if (id.empty()) continue;
		auto it = accountMap.find(id);
		if (it == accountMap.end()) {
			std::cout << "账户不存在，返回登录界面。" << std::endl;
			continue;
		}
		Account* currentAccount = it->second.get();
		//菜单
		std::cout << "登录成功，欢迎 " << currentAccount->getId() << "！" << std::endl;
		std::cout << "----------------------------------------------------------------------" << std::endl;

		bool sessionActive = true; //内层循环控制变量
		while (sessionActive) {
			std::cout << "菜单：0.退出程序 1.存款 2.取款 3.查询余额 4.转账 5.交易历史 6.退出登录" << std::endl;
			std::string input;
			int choice;
			std::cin >> input;
			// 清空cin buffer（防输入残留）
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			try {
				choice = std::stoi(input);
			}
			catch (...) {
				std::cout << "无效输入，请输入数字选项。" << std::endl;
				continue;
			}
			std::string AmountStr;
			std::string targetId;
			double amount;
			bool opSuccess = false;

			switch (choice) {
			case 0:
				std::cout << "谢谢使用，再见！" << std::endl;
				saveFile();
				exit(0);
				break;
			case 1://存款
				std::cout << "请输入存款金额：" << std::endl;
				std::cin >> AmountStr;
				std::cin.ignore();
				try {
					amount = std::stod(AmountStr);
					opSuccess = currentAccount->deposit(amount);
					if (opSuccess) {
						std::cout << "存款成功！当前余额：" << currentAccount->getBalance() << std::endl;
					}
					else {
						std::cout << "取款失败，余额不足或无效金额！" << std::endl;
					}
				}
				catch (const std::invalid_argument& e) {
					std::cout << e.what() << std::endl;
				}
				catch (...) {
					std::cout << "存款失败，未知错误！" << std::endl;
				}
				break;
			case 2://取款
				std::cout << "请输入取款金额：" << std::endl;
				std::cin >> AmountStr;
				std::cin.ignore();
				try {
					amount = std::stod(AmountStr);
					opSuccess = currentAccount->withdraw(amount);
					if (opSuccess) {
						std::cout << "取款成功！当前余额：" << currentAccount->getBalance() << std::endl;
					}
					else {
						std::cout << "取款失败，余额不足或无效金额！" << std::endl;
					}
				}
				catch (const std::invalid_argument& e) {
					std::cout << e.what() << std::endl;
				}
				catch (...) {
					std::cout << "取款失败，未知错误！" << std::endl;
				}
				break;
			case 3://查询余额
				currentAccount->checkBalance();
				break;
			case 4://转账
				std::cout << "输入目标账户ID：" << std::endl;
				std::cin >> targetId;
				std::cin.ignore();
				std::cout << "输入转账金额：" << std::endl;
				std::cin >> AmountStr;
				std::cin.ignore();
				try {
					amount = std::stod(AmountStr);
				}
				catch(...){
					std::cout << "无效金额输入！" << std::endl;
					break;
				}
				opSuccess = currentAccount->transfer(targetId, amount);
				if (opSuccess) {
					std::cout << "转账成功！当前余额：" << currentAccount->getBalance() << std::endl;
				}
				else {
					std::cout << "转账失败，余额不足或目标账户不存在！" << std::endl;
				}
				break;
			case 5://交易历史
				currentAccount->viewHistory();
				break;
			case 6:
				try {
					saveFile();
					std::cout << "数据已保存，返回登录界面..." << std::endl;  // 修改：确认日志
				}
				catch (...) {
					std::cout << "保存失败，但返回登录界面。" << std::endl;
				}
				sessionActive = false;
				break;
			default:
				std::cout << "无效选项，请重新选择。" << std::endl;
				break;
			}
			if (!opSuccess && (choice ==1 || choice ==2 || choice == 4)) {
				std::cout << "操作未成功，请重试！" << std::endl;
			}
		}
	}
}