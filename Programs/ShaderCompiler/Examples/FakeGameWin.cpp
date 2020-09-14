#include <Windows.h>
#include <stdexcept>
#include <iostream>

int main(void)
{
    HANDLE hPipe;
    DWORD dwRead;

        hPipe = CreateFile(TEXT("\\\\.\\pipe\\ShaderCompilerOutput"), 
                       GENERIC_READ,
                       0,
                       NULL,
                       OPEN_EXISTING,
                       0,
                       NULL);

        if (hPipe == INVALID_HANDLE_VALUE)
            return -1;
    while (1)
    {
            char buffer[1024];

            while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
            {
                buffer[dwRead] = '\0';

                std::cout << buffer << std::endl;
            }
    }
    CloseHandle(hPipe);

    return (0);
}