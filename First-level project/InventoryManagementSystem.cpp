/*3. 库存管理系统

项目需求概括
编写一个控制台程序，模拟商店库存管理系统。系统支持多种物品（每个物品有唯一名称、数量、价格和描述），允许用户添加新物品、更新库存数量、删除物品、搜索物品（按名称或价格范围）、生成库存报告（包括低库存警报和总价值）。数据需持久化到文件（e.g., inventory.txt），以便程序重启后保留。处理常见错误，如无效输入或不存在物品。

物品信息结构：使用类或结构体表示物品。
类Item：包含名称（string）、数量（int，默认0）、价格（double，默认0.0）、描述（string，可选）。

系统功能（菜单驱动）：
添加物品（输入名称、数量、价格、描述）。
更新库存（输入名称和新数量；如果数量<0，报错）。
删除物品（输入名称，确认删除）。
搜索物品（子菜单：按名称精确搜索，或按价格范围搜索e.g., 10-50）。
生成报告（显示所有物品列表、总价值计算、低库存警报阈值e.g., 数量<5）。
退出（保存数据）。

文件持久化：启动时从文件加载所有物品；退出时保存所有物品（格式：名称|数量|价格|描述，每行一个物品，用分隔符|）。
额外规则：总价值 = 数量 * 价格之和；搜索结果按名称排序输出。

预期目标
程序运行后，用户可通过菜单完整管理库存：实时更新数据、快速搜索、生成简易报告，并确保数据持久化。系统交互友好，使用map高效存储（O(1)查找），报告输出清晰表格格式。
可用到的知识点

map容器：用std::map<std::string, Item>存储物品（键为名称，便于快速查找和排序）。
算法：<algorithm>用于排序搜索结果（e.g., std::sort按价格）；计算总价值用循环遍历。
类/结构体：struct Item { std::string name, desc; int qty; double price; }; 或完整类带成员函数（如toString()输出格式）。
文件I/O：<fstream>和<sstream>读写文件；启动时解析行，退出时序列化。
其他：循环菜单（while + switch）、条件判断（if验证输入）、异常基础（if检查无效数量/价格）。
*/

#include <iostream>
#include"InventoryManagementSystem.h"
#include<string>
#include<map>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<vector>
#include <limits>

class Item {
private:
	std::string name;
	int quantity;
	double price;
	std::string description;//描述

public:
	Item(std::string name, int quantity =0, double price=0.0, std::string description ="") : name(name), quantity(quantity), price(price), description(description) {}
	std::string getName() const { return name; }
	int getQuantity() const { return quantity; }
	double getPrice() const { return price; }
	std::string getDescription() const { return description; }

	void setQuantity(int qty) { quantity = qty; }

};
/*添加新物品、更新库存数量、删除物品、搜索物品（按名称或价格范围）、生成库存报告（包括低库存警报和总价值）
添加物品（输入名称、数量、价格、描述）。
更新库存（输入名称和新数量；如果数量<0，报错）。
删除物品（输入名称，确认删除）。
搜索物品（子菜单：按名称精确搜索，或按价格范围搜索e.g., 10-50）。
生成报告（显示所有物品列表、总价值计算、低库存警报阈值e.g., 数量<5）。
额外规则：总价值 = 数量 * 价格之和；搜索结果按名称排序输出。
*/
bool addItem(std::map<std::string, Item>& inventory, const Item& item) {
	auto[it,isinserted] = inventory.emplace(item.getName(), item);
	return isinserted;
}

bool updateItemQuantity(std::map<std::string, Item>& inventory, const std::string& name,int quantity) {
	auto it = inventory.find(name);
	if (it != inventory.end()) {
		it->second.setQuantity(quantity);
		return true;
	}
	else {
		return false;
	}
}

bool FindName(std::map<std::string, Item>inventory, const std::string& name) {
	auto it = inventory.find(name);
	if (it == inventory.end()) {
		return false;
	}
	std::cout << "名称：" << it->second.getName() << " 数量：" << it->second.getQuantity() << " 价格：" << it->second.getPrice() << " 描述：" << it->second.getDescription() << std::endl;
	return true;
}

/*文件持久化：启动时从文件加载所有物品；退出时保存所有物品（格式：名称|数量|价格|描述，每行一个物品，用分隔符|）。*/
//只读取了第一个物品

void loadFile(std::map<std::string, Item>& inventory) {
	std::ifstream ifs("inventory.txt");
	if (!ifs.is_open()) {
		std::cout << "文件打开失败" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(ifs, line)) {
		if (line.empty()) continue;

		std::stringstream ss(line);
		std::string name;
		std::string Q_Str;
		std::string P_Str;
		int quantity;
		double price;
		std::string description;
		if (
			!std::getline(ss, name, '|') ||
			!std::getline(ss, Q_Str, '|') ||
			!std::getline(ss, P_Str, '|') ||
			!std::getline(ss, description)
			) {
			std::cout << "文件格式错误" << std::endl;
			continue;
		}
		try {
			quantity = std::stoi(Q_Str);
			price = std::stod(P_Str);
		}
		catch (...) {
			std::cout << "文件格式转换错误" << std::endl;
			continue;
		}
		inventory.emplace(name, Item(name, quantity, price, description));
	}
}
void saveFile(std::map<std::string, Item>& inventory) {
	std::ofstream ofs("inventory.txt");
	if (!ofs.is_open()) {
		std::cout << "文件打开失败" << std::endl;
		return;
	}
	for (auto& pair : inventory) {
		try {
			ofs << pair.second.getName() << "|" << pair.second.getQuantity() << "|" << pair.second.getPrice() << "|" << pair.second.getDescription() << std::endl;
		}
		catch (...) {
			std::cout << "保存数据失败" << std::endl;
		}
	}
	ofs.close();
	std::cout << "保存完成" << std::endl;
}
void InventoryManagementSystem(void) {
	std::cout << "欢迎使用库存管理系统" << std::endl;
	std::map<std::string,Item> inventory;
	//从文件中加载数据
	loadFile(inventory);
	
	while (true) {
		std::cout << "菜单：1.添加物品 2.更新库存 3.删除物品 4.搜索物品 5.生成报告 6.退出" << std::endl;
		std::string input;
		int choice;
		std::cin >> input;
		try {
			choice = std::stoi(input);
		}
		catch (...) {
			std::cout << "输入错误，请输入数字" << std::endl;
			std::cin.clear();  // 重置 cin 的失败/EOF 标志
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 忽略所有残留字符，直到行尾
			continue;
		}
		std::string name;
		std::string Str;
		int quantity;
		double price;
		std::string description;
		bool success = false;
		size_t erased;
		int case_num;
		int MaxPrice, MinPrice;
		std::vector<Item> items;

		switch (choice) {
		case 1://添加物品（输入名称、数量、价格、描述）。
			std::cout << "请输入物品名称：" << std::endl;
			std::cin >> name;
			std::cout << "请输入物品数量：" << std::endl;
			std::cin >> Str;
			try {
				quantity = std::stoi(Str);
			}
			catch (...) {
				std::cout << "输入错误，请输入数字" << std::endl;
				continue;
			}
			std::cout << "请输入物品价格：" << std::endl;
			std::cin >> Str;
			try {
				price = std::stod(Str);
			}
			catch(...) {
				std::cout << "输入错误，请输入数字" << std::endl;
				continue;
			}
			std::cout << "请输入物品描述：" << std::endl;
			std::cin >> description;

			success = addItem(inventory, Item(name, quantity, price, description));
			if (success) {
				std::cout << "物品添加成功" << std::endl;
			}
			else {
				std::cout << "物品已存在，添加失败" << std::endl;
			}
			break;

		case 2://更新库存（输入名称和新数量；如果数量<0，报错）。
			std::cout << "请输入物品名称：" << std::endl;
			std::cin >> name;
			std::cout << "请输入新数量：" << std::endl;
			std::cin >> Str;
			try {
				quantity = std::stoi(Str);

			}
			catch (...) {
				std::cout << "输入错误，请输入数字" << std::endl;
			}
			if (quantity < 0) {
				std::cout << "数量不能为负，更新失败" << std::endl;
				continue;
			}
			
			success = updateItemQuantity(inventory, name, quantity);
			if (success) {
				std::cout << "更新成功" << std::endl;
			}
			else {
				std::cout << "物品不存在，更新失败" << std::endl;
				continue;
			}	
			break;
		case 3://删除物品（输入名称，确认删除）。
			std::cout << "请输入需要删除的物品名称：" << std::endl;
			std::cin >> name;
			erased = inventory.erase(name);
			if (erased > 0) {
				std::cout << "物品删除成功" << std::endl;
			}
			else {
				std::cout << "物品不存在，删除失败" << std::endl;
				continue;
			}
			break;
		case 4://搜索物品（子菜单：按名称精确搜索，或按价格范围搜索e.g., 10-50）。
			std::cout << "1.通过名称搜索 2.通过价格范围搜索" << std::endl;
			std::cout << Str;
			try {
				case_num = std::stoi(Str);
			}
			catch (...) {
				std::cout<< "输入错误，请输入数字" << std::endl;
				continue;
			}
			if (case_num == 1) {
				std::cout << "请输入物品名称：" << std::endl;
				std::cin >> name;
				success = FindName(inventory, name);
				if (!success) {
					std::cout << "未找到物品" << std::endl;
				}
			}
			else if (case_num == 2) {
				std::cout << "请输入最大值：" << std::endl;
				std::cin >> Str;
				try {
					MaxPrice = std::stoi(Str);

				}
				catch (...) {
					std::cout << "输入错误，请输入数字" << std::endl;
					continue;
				}
				std::cout << "请输入最小值：" << std::endl;
				std::cin >> Str;
				try {
					MinPrice = std::stoi(Str);
				}
				catch (...) {
					std::cout << "输入错误，请输入数字" << std::endl;
					continue;
				}
				
				for (auto& pair : inventory) {
					if (pair.second.getPrice() >= MinPrice && pair.second.getPrice() <= MaxPrice) {
						items.push_back(pair.second);
					}
				}
				std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.getPrice() < b.getPrice(); });
				std::cout << "找到物品：" << std::endl;
				for (Item& item : items) {
					std::cout << "名称：" << item.getName() << " 数量：" << item.getQuantity() << " 价格：" << item.getPrice() << " 描述：" << item.getDescription() << std::endl;
				}
			}
			else {
				std::cout << "请选择提供的选项" << std::endl;
				continue;
			}

			break;
		case 5://生成报告（显示所有物品列表、总价值计算、低库存警报阈值e.g., 数量<5）。 额外规则：总价值 = 数量 * 价格之和；搜索结果按名称排序输出。
			for (auto& pair : inventory) {
				items.push_back(pair.second);
			}
			std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) { return a.getName() < b.getName(); });
			for (Item& item : items) {
				if (item.getQuantity() < 5) {
					std::cout << "名称：" << item.getName() << " 数量：" << item.getQuantity() << " 价格：" << item.getPrice() << " 描述：" << item.getDescription() << " 总价值：" << (item.getPrice() * item.getQuantity()) << " 低库存警报：该物品数量已经小于5，请补货" << std::endl;
				}
				else {
					std::cout << "名称：" << item.getName() << " 数量：" << item.getQuantity() << " 价格：" << item.getPrice() << " 描述：" << item.getDescription() << " 总价值：" << (item.getPrice() * item.getQuantity()) << std::endl;
				}
			}

			break;
		case 6://退出时保存所有物品（格式：名称|数量|价格|描述，每行一个物品，用分隔符|）
			std::cout << "感谢使用本系统，再见" << std::endl;
			saveFile(inventory);
			return;
		default:
			std::cout << "输入错误，请输入有效选项" << std::endl;
			break;
		}
	}
}