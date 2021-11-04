#include <iostream>
#include <string>
#include <fstream>

int main()
{
	std::size_t N;
	std::string text;
	std::cin >> N;
	std::getline(std::cin, text);

	for(std::size_t i = 1; i <= N; ++i)
	{
		std::string file = "file_" + std::to_string(i) + ".txt";
		std::ofstream fout(file);
		fout << "Hella world " << i << "\n" << text << " " << i;
	}

	return 0;
}
