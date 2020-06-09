#pragma once

#include "BasePipeline.hpp"

namespace Soon
{
	class ComputePipeline : public BasePipeline
	{
	public:
		std::string _computePath;
		const PipelineType _type = PipelineType::COMPUTE;

		ComputePipeline();
		~ComputePipeline();
		void SetPipelineProperties(std::string name, void* data);
		void Init( void );
		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void);
		void Dispatch( void );
	};
}
