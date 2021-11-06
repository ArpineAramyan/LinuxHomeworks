#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring> 

int main()
{
	std::size_t N;
	char text[100];
	std::cin >> N;
	std::cin.getline(text, 100);
	char ab[2] = "\n";

	char* buf[1000];

	for(std::size_t i = 1; i <= N; ++i)
	{
		char file[10] = "file_";
		char filee = i + '0';
		file[5] = filee;
		char fileee[10] = ".txt";
		//strcat(file, filee);
		strcat(file, fileee);

		char hi[150] = "Hello world ";
		hi[12] = filee;
		strcat(hi, ab);
		strcat(hi, text);
		strcat(hi, ab);
		
		int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		write(fd, hi, sizeof(hi));
		read(fd, buf, sizeof(hi));
		close(fd);
	}
}
