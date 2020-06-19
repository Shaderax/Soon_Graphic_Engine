#pragma once

#include <string>

namespace Soon
{
	class PipelineConf;

	PipelineConf* ReadPipelineJson( std::string path );
}