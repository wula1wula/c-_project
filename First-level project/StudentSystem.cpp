/*1. 学生数据库系统

项目需求概括：编写一个程序管理学生信息，支持添加、删除、搜索和显示学生记录（姓名、ID、成绩），数据持久化到文件。
预期目标：程序提供菜单驱动界面，用户可操作学生列表，数据在程序重启后保留。
可以用到的知识点：类（Student类，包括成员函数）、vector容器（存储学生列表）、文件I/O（<fstream>保存/加载）、循环菜单（switch）。
简单测试：
输入：添加 学生1 001 85
输出：添加成功
输入：搜索 001
输出：学生1，成绩：85
输入：显示所有
输出：学生1 ID:001 成绩:85
*/


#include<iostream>
#include<vector>
#include <string>
#include<fstream>
#include<sstream>
#include<cstdlib>

class Student {
private:
	std::string name;
	int id;
	float score;
public:
	void setName(std::string name) {
		this->name = name;
	}
	std::string getName() {
		return name;
	}

	void setId(int id) {
		this->id = id;
	}
	int getId() {
		return id;
	}

	void setScore(float score) {
		this->score = score;
	}
	float getScore() {
		return score;
	}
};

class OP {
private:
	std::vector<Student> students;
	std::string data;
public:
	//增加
	void WriteFile(Student s) {
		std::ofstream WFile("studentData.txt", std::ios::app);
		if (WFile.is_open()) {
			WFile << s.getName() << " " << s.getId() << " " << s.getScore() << "\n";
			std::cout << "添加成功" << std::endl;
			WFile.close();
		}
		else {
			std::cout << "文件打开失败" << std::endl;
			return;
		}
	}
	//显示
	void show() {
		//std::cout << "显示所有学生信息" << std::endl;
		std::ifstream RFile("studentData.txt");
		if (RFile.is_open()) {
			while (std::getline(RFile, data)) {
				std::istringstream iss(data);
				std::string name;
				int Lineid;
				float score;
				if (!(iss >> name >> Lineid >> score)) {
					std::cout << "文件数据错误" << std::endl;
					return;
				}
				std::cout << "姓名：" << name <<" 学号："<< Lineid <<" 成绩："<< score << "\n" << std::endl;
			}
			std::cout << "已显示所有学生信息" << std::endl;
			RFile.close();
		}
		else {
			std::cout << "文件打开失败" << std::endl;
			return;
		}
	}
	//搜索
	void search(int id) {
		//std::cout << "搜索学生信息" << std::endl;
		std::ifstream RFile("studentData.txt");
		bool isfind = false;
		if (RFile.is_open()) {
			while (std::getline(RFile, data)) {
				std::istringstream iss(data);
				std::string name;
				int Lineid;
				float score;
				if (!(iss >> name >> Lineid >> score)) {
					std::cout << "文件数据错误" << std::endl;
					return;
				}
				if (Lineid == id) {
					std::cout << "学生信息：" << " 姓名：" << name << " 学号：" << Lineid << " 成绩：" << score << std::endl;
					isfind = true;
				}
			}
			if (!isfind) {
				std::cout << "未找到" << id << "的学生信息" << std::endl;
			}
			RFile.close();
		}
		else {
			std::cout << "文件打开失败" << std::endl;
			return;
		}
	}
	//删除
	void del(int id) {
		//std::cout << "删除学生信息" << std::endl;
		std::ifstream RFile("studentData.txt");
		if (!RFile.is_open()) {
			std::cout << "文件打开失败" << std::endl;
			return;
		}
		std::ofstream TempFile("Temp.txt");
		if (!TempFile.is_open()) {
			std::cout << "临时文件创建失败" << std::endl;
			return;
		}
		std::string data;
		bool isfind = false;
		while (std::getline(RFile, data)) {
			std::istringstream iss(data);
			if (data.empty()) {
				TempFile << data << "\n";
				continue;
			}
			std::string name;
			int Lineid;
			float score;
			if (iss >> name >> Lineid >> score) {
				if (Lineid != id) {
					TempFile << data << "\n";
				}
				else {
					isfind = true;
				}
			}
			else {
				TempFile << data << "\n";
			}
		}
		RFile.close();
		TempFile.close();

		if (std::remove("studentData.txt") != 0) {
			std::cout << "删除文件失败" << std::endl;
			std::remove("Temp.txt");
			return;
		}
		if (std::rename("Temp.txt", "studentData.txt") != 0) {
			std::cout << "临时文件重命名失败" << std::endl;
			std::remove("Temp.txt");
			return;
		}

		if (isfind) {
			std::cout << "删除成功" << std::endl;
		}
		else {
			std::cout << "未找到学生信息" << std::endl;
		}
	}
};

void studentSystem(void) {
	std::cout << "欢迎使用学生数据管理系统" << std::endl;
	Student s;
	OP op;
	std::string input;
	int choice;
	std::string name;
	std::string number_str;
	int id;
	float score;
	while (true) {
		std::cout << "菜单：1.添加 2.删除 3.搜索 4.显示 5.退出" << std::endl;
		std::cin >> input;
		try {
			choice = std::stoi(input);
		}
		catch (...) {
			std::cout << "输入错误，请输入数字" << std::endl;
			continue;
		}
		switch (choice) {
		case 1://添加
			std::cout << "请输入姓名：" << std::endl;
			std::cin >> name;
			std::cout << "请输入学号" << std::endl;
			std::cin >> number_str;
			try {
				id = std::stoi(number_str);
			}
			catch (...) {
				std::cout << "输入错误，请输入数字" << std::endl;
				continue;
			}
			std::cout << "请输入成绩" << std::endl;
			std::cin >> number_str;
			try {
				score = std::stof(number_str);
			}
			catch (...) {
				std::cout << "输入错误，请输入数字" << std::endl;
				continue;
			}
			s.setName(name);
			s.setId(id);
			s.setScore(score);
			op.WriteFile(s);
			//std::cout << "添加成功" << std::endl;
			break;
		case 2://删除
			std::cout << "请输入要删除的学生学号" << std::endl;
			std::cin >> number_str;
			try {
				id = std::stoi(number_str);
			}
			catch (...) {
				std::cout<< "输入错误，请输入数字" << std::endl;
				continue;
			}
			op.del(id);
			//std::cout << "删除成功" << std::endl;
			break;
		case 3://搜索
			std::cout << "请输入要搜索的学生学号" << std::endl;
			std::cin >> number_str;
			try {
				id = std::stoi(number_str);
			}
			catch (...) {
				std::cout << "输入错误，请输入数字" << std::endl;
				continue;
			}
			op.search(id);
			break;
		case 4://显示
			op.show();
			break;
		case 5://退出
			std::cout << "感谢使用，再见！" << std::endl;
			return;
		default:
			std::cout << "输入错误，请输入正确的选项" << std::endl;
			break;
		}
	}
}

