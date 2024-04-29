#include <iostream>
#include <Windows.h>
#include <fstream>
#include "intgrl_func.h"

using std::cout;

int main() {
	SetConsoleOutputCP(1251);
	std::ifstream in("exmpl.txt");
	std::ofstream out("out.txt");
	if (!in || !out) {
		std::cerr << "Ошибка работы с файлами\n";
	} else {
		try {
			processFile(in, cout);
		}
		catch (std::string& s) {
			cout << "Ошибка: " << s << '\n';
		}
	}
	system("pause");
}