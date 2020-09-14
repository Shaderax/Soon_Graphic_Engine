#include "ShaderCompiler.hpp"
#include <chrono>
#include <thread>
#include <cstring>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "error: usage: ShaderCompiler <directory path>" << std::endl;
        return -1;
    }

    if (!DirExists(argv[1]))
    {
        std::cout << "Not a valid Directory: " << argv[1] << std::endl;
        return -1;
    }

    // Add / at end TODO
    std::size_t len = std::strlen(argv[1]);
    std::string dir;
    dir = argv[1];
    dir += "/";

    /** Check One instance*/
    if (!CheckIfOnInstance(dir.c_str()))
        return 0;

    // Create the file system watcher instance
    efsw::FileWatcher* fileWatcher = new efsw::FileWatcher();
    // Create the instance of your efsw::FileWatcherListener implementation
    ShaderReCompiler* recompiler = new ShaderReCompiler();
    // Add a folder to watch, and get the efsw::WatchID
    // Reporting the files and directories changes to the instance of the listener
    efsw::WatchID watchID = fileWatcher->addWatch( dir.c_str(), recompiler, true );
    // Start watching asynchronously the directories

    char buffer[1024];

    if (!CreatePipe())
        return -1;

    while(1)
	{
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        fileWatcher->watch();
	}

    Close();

    std::cin.get();

    return 0;
}
