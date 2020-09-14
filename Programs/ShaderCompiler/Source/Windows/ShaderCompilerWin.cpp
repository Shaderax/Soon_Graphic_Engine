#include <Windows.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <array>
#include <cstdio>
#include <sstream>

#include "ShaderCompiler.hpp"

HANDLE hMutex;
HANDLE gHPipe;
DWORD dwRead;

bool CheckIfOnInstance( const char* argv)
{
    hMutex = OpenMutex(
        MUTEX_ALL_ACCESS, 0, argv);

    if (!hMutex)
        hMutex = CreateMutex(0, 0, argv);
    else
    {
        ReleaseMutex(hMutex);
        return false;
    }

    return true;
}

static std::string Exec(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);

    if (!pipe)
        throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();

    return result;
}

void ShaderReCompiler::handleFileAction( efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename)
{
    if (action != efsw::Action::Modified)
        return;

    std::size_t pos = filename.rfind(".");
    for (const std::string &ext : gValidShaderExt)
    {
        if (pos != std::string::npos && filename.substr(pos + 1) == ext)
        {
            std::ostringstream cmd;

            std::string cmplPath = dir + "/" + filename;
            std::cout << cmplPath << (int)action << std::endl;

            cmd << "glslc " << cmplPath << " -o " << cmplPath << ".spv"
                << " 2>&1";
            std::string log = Exec(cmd.str().c_str());
            std::cout << "Exec return: " << log << std::endl;
            WriteFile(gHPipe, log.c_str(), log.size() + 1, &dwRead, NULL);
            std::cout << "End Log" << std::endl;

            return;
        }
    }
    std::cout << dir << "/" << filename << "Don't know what is that" << std::endl;
}

bool DirExists(const std::string &inDirName)
{
    DWORD ftyp = GetFileAttributesA(inDirName.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false; //something is wrong with your path!

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true; // this is a directory!

    return false; // this is not a directory!
}

bool CreatePipe(void)
{
    gHPipe = CreateNamedPipeA(TEXT("\\\\.\\pipe\\ShaderCompilerOutput"),
                              PIPE_ACCESS_DUPLEX,
                              PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                              5,
                              1024,
                              1024,
                              3,
                              NULL);

    if (gHPipe == INVALID_HANDLE_VALUE)
        return false;
    return true;
}

void Close(void)
{
    ReleaseMutex(hMutex);
    CloseHandle(gHPipe);
}