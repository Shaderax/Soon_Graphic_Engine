#pragma once

#include <stdexcept>
#include <iostream>
#include <string>
#include <efsw/efsw.hpp>
#include <array>
#include <cstdio>
#include <sstream>
#include <memory>
#include <errno.h>
#include <sys/stat.h>

const std::string gValidShaderExt[] = { "vert", "frag", "tesc", "tese", "geom", "comp"};

class ShaderReCompiler : public efsw::FileWatchListener
{
public:
    ShaderReCompiler( void ) = default;

    void handleFileAction( efsw::WatchID watchid, const std::string& dir, const std::string& filename, efsw::Action action, std::string oldFilename = "" );
};

bool        CreatePipe( void );
void        Close( void );
bool        DirExists(const std::string& inDirName);
bool        CheckIfOnInstance( const char* argv );