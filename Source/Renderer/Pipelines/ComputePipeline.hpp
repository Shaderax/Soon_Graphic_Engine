#pragma once

#include "Renderer/Pipelines/BasePipeline.hpp"

namespace Soon
{
	class Mesh;

	class ComputePipeline : public BasePipeline
	{
		public:
		static const PipelineType _type = PipelineType::COMPUTE;
			ComputePipeline()
			{
			}

			virtual uint32_t AddToPipeline( Mesh* mesh ) = 0;
	};
}
