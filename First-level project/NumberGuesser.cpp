/*3. 猜数字游戏

项目需求概括：程序生成一个1 - 100的随机数，用户猜测，程序提示“太大”或“太小”，直到猜中。记录猜测次数，支持重玩。
预期目标：互动游戏，引导用户猜测，提供反馈和总结。
可以用到的知识点：随机数（<random>）、循环（do - while）、条件语句（if - else）、变量计数、I / O。
简单测试：
随机数：50
输入：70
输出：太大！
输入：40
输出：太小！
输入：50
输出：恭喜猜中！用了3次*/

#include<iostream>
#include"NumberGuesser.h"
#include<random>
#include<chrono>

void NumberGuesser(void) {
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> dis(1, 100);

	char play_again = 'y';
	while (play_again == 'y' || play_again == 'Y') {
		int target_number = dis(gen);
		int guess = 0;
		int count = 0;

		std::cout << "欢迎来到猜数字游戏！我已经选好了一个1到100之间的数字。" << std::endl;
		do {
			std::cin >> guess;
			if (guess == target_number) {
				count++;
				std::cout << "恭喜你猜中了，用了" << count << "次" << std::endl;
				break;
			}
			else if (guess < target_number) {
				count++;
				std::cout << "太小了" << std::endl;
			}
			else {
				count++;
				std::cout << "太大了" << std::endl;
			}
		} while (true);
		std::cout << "想再玩一次吗？(y/n): ";
		std::cin >> play_again;
		if (play_again != 'y' && play_again != 'Y') {
			std::cout << "感谢游玩！再见！" << std::endl;
		}
	}
}