/*2. 石头剪刀布游戏

项目需求概括：实现一个两人游戏（用户 vs 电脑），用户输入石头、剪刀或布，电脑随机生成选择，判断胜负。支持多轮游戏，统计胜率，直到用户退出。
预期目标：程序模拟简单游戏循环，显示每轮结果和总统计。
可以用到的知识点：随机数生成（<random>、<ctime>）、switch或if - else判断、for / while循环、变量存储统计、基本I / O。
简单测试：
输入：石头（电脑随机布）
输出：你输了！当前胜率：0 %
输入：剪刀（电脑随机石头）
输出：你赢了！当前胜率：50 %
输入：q
输出：游戏结束，总胜率：50 %*/

#include<iostream>
#include <string>
#include<random>
#include <chrono>    // 可选：用于种子（基于时间）
#include"Rock-Paper-Scissors.h"

void Rock_Paper_Scissors(void) {
	std::string input;
	std::string choices[3] = { "石头", "剪刀", "布" };
	int all =0;
	int win =0;
	int index;
	double win_rate =0.0;
	std::random_device rd;
	std::mt19937 gen(rd());
	// 均匀分布整数 [0 , 2]
	std::uniform_int_distribution<int> dis(0, 2);
	std::cout << "请出招" << std::endl;
	while (true) {
		getline(std::cin, input);

		index = dis(gen);
		if (input == "q") {
			if (all == 0) {
				std::cout << "游戏结束，未开始游戏" << std::endl;
			}
			else {
				win_rate = (static_cast<double>(win) / all) * 100;
				std::cout << "游戏结束，总胜率：" << static_cast<int> (win_rate) << "%" << std::endl;
			}
			break;
		}
		if (input == "石头") {
			if (index == 0) {
				all++;
				std::cout << "平局，电脑出" << choices[index] << std::endl;
			}
			else if (index == 1) {
				win++;
				all++;
				std::cout << "你赢了，电脑出" << choices[index] << std::endl;
			}
			else {
				all++;
				std::cout << "你输了，电脑出" << choices[index] << std::endl;
			}
		}
		else if (input == "剪刀") {
			if (index == 0) {
				all++;
				std::cout << "你输了，电脑出" << choices[index] << std::endl;
			}
			else if (index == 1) {
				all++;
				std::cout << "平局，电脑出" << choices[index] << std::endl;
			}
			else {
				all++;
				win++;
				std::cout << "你赢了，电脑出" << choices[index] << std::endl;
			}
		}
		else if (input == "布") {
			if (index == 0) {
				all++;
				win++;
				std::cout << "你赢了，电脑出" << choices[index] << std::endl;
			}
			else if (index == 1) {
				all++;
				std::cout << "你输了，电脑出" << choices[index] << std::endl;
			}
			else {
				all++;
				std::cout << "平局，电脑出" << choices[index] << std::endl;
			}
		}
		else {
			std::cout << "出招不合理，请重新出招" << std::endl;
		}
	}
}

