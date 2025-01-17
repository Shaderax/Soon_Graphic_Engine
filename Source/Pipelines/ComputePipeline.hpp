#pragma once

#include "BasePipeline.hpp"
#include "Pipelines/ComputePipelineConf.hpp"

namespace Soon
{
	struct IdCompute
	{
		uint32_t matId;
		uint32_t workGroup[3];
		bool	cached;
	};

	enum class EProcessFrequency : uint8_t
	{
		FRAME = 0,
		ONCE = 1
	};

	class ComputePipeline : public BasePipeline
	{
	private:
		std::vector<IdCompute> m_ProcessData;
		ComputePipelineConf* computeConf;
	public:
		ComputePipeline(ComputePipelineConf* conf);
		~ComputePipeline();
		void Init( void );
		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void);
		void Dispatch( void );
		void SetProcessFrequency(EProcessFrequency freq);

		bool IsValidToProcess(uint32_t id) const;
		void Process(uint32_t id);
		void UnProcess(uint32_t id);
		uint32_t CreateNewId( void );
		void RemoveId(uint32_t id);
		void BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage);
		void SetWorkGroup(uint32_t matId, uint32_t x, uint32_t y, uint32_t z);
	};
}
