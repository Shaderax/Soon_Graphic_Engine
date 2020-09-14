#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int gFd;
char* myfifo = "/tmp/ShaderCompilerOutput";

int main(void)
{
    char str1[80], str2[80]; 

    mkfifo(myfifo, 0666); 

    gFd = open(myfifo, O_RDONLY); 

	if (gFd == -1)
		return -1;

	int nbRead = 0;
	while (1)
	{
    	nbRead = read(gFd, str1, 80);

		if (nbRead > 0) 
		{
			str1[nbRead] = '\0';
			std::cout << str1;
		}
	}
  
    // Print the read string and close 
    close(gFd); 
  
	unlink(myfifo);

    std::cin.get();

    return (0);
}