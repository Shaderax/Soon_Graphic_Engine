#include "ComputePipeline.hpp"
#include "GraphicsRenderer.hpp"

namespace Soon
{
	ComputePipeline::ComputePipeline(ComputePipelineConf* conf) : BasePipeline(conf), computeConf(conf)
	{
	}

	ComputePipeline::~ComputePipeline()
	{
	}

	void ComputePipeline::Init( void )
	{
		for (PipelineStage stage : computeConf->GetStages())
			GetShaderData(stage.shaderPath);

		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.CreateDescriptorSetLayout());
		computeConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(computeConf);

		_mUbm.InitBuffers();
	}

	// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
	void ComputePipeline::RecreatePipeline(void)
	{
		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.GetDescriptorSetLayout());
		computeConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(computeConf);
	}

	void ComputePipeline::SetProcessFrequency(EProcessFrequency freq)
	{
		GraphicsRenderer::GetInstance()->SetProcessFrequency(_conf->GetJsonPath(), freq);
	}

	void ComputePipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage)
	{
		if (m_ToDraw.empty())
			return ;

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _pipeline);

		std::vector<DescriptorSetDescription>& sets = _mUbm.GetSets();
		std::vector<DescriptorSetDescription>& uniqueSets = _mUbm.GetUniqueSets();
		std::vector<VkDescriptorSet>& vecDescriptor = _mUbm.GetDescriptorSet(currentImage);

		for (uint32_t uniqueId = 0 ; uniqueId < uniqueSets.size() ; uniqueId++)
		{
			vkCmdBindDescriptorSets(commandBuffer,
									VK_PIPELINE_BIND_POINT_COMPUTE,
									_pipelineLayout,
									uniqueSets[uniqueId].set,
									1,
									&(vecDescriptor[uniqueId]), 0, nullptr);
		}

		VkDeviceSize offsets[] = {0};
		for ( std::unordered_map<uint32_t, uint32_t>::iterator it = m_ToDraw.begin(); it != m_ToDraw.end(); ++it )
		{
			std::cout << "To Draw : " << it->second << std::endl;

			for (uint32_t setId = 0 ; setId < sets.size() ; setId++)
			{
				vkCmdBindDescriptorSets(commandBuffer,
										VK_PIPELINE_BIND_POINT_COMPUTE,
										_pipelineLayout,
										sets[setId].set,
										1,
										&(vecDescriptor[uniqueSets.size() + setId + (sets.size() * it->second)]),
										0,
										nullptr);
			}
			vkCmdDispatch(commandBuffer, m_ProcessData[it->second].workGroup[0], m_ProcessData[it->second].workGroup[1], m_ProcessData[it->second].workGroup[2]);
		}
	}

	void ComputePipeline::Dispatch( void )
	{
		// TODO: Need to replace for wait semaphore
		VkCommandBuffer commandBuffer = GraphicsInstance::GetInstance()->BeginSingleTimeCommands();

		BindCaller(commandBuffer, GraphicsInstance::GetInstance()->GetNextIdImageToRender());

		GraphicsInstance::GetInstance()->EndSingleTimeCommands(commandBuffer);
	}

	bool ComputePipeline::IsValidToProcess(uint32_t id) const
	{
		if (!m_ProcessData[id].cached || id >= m_ProcessData.size())
			return false;

		uint32_t result = 1;

		for (uint32_t index = 0 ; index < 3 ; index++)
			result *= m_ProcessData[id].workGroup[index];
		if (result == 0)
			return (false);
		return true;
	}

	void ComputePipeline::Process(uint32_t id)
	{
		if (IsValidToProcess(id))
			return ;
		m_ProcessData[id].cached = false;
		m_ToDraw[id] = id;
		GraphicsRenderer::GetInstance()->HasChange();
	}

	void ComputePipeline::UnProcess(uint32_t id)
	{
		m_ToDraw.erase(id);
		m_ProcessData[id].cached = true;
		GraphicsRenderer::GetInstance()->HasChange();
	}

	uint32_t ComputePipeline::CreateNewId( void )
	{
		uint32_t idMat;

		if (!_freeId.empty())
		{
			idMat = _freeId.back();
			_freeId.pop_back();
			m_ProcessData[idMat] = {idMat, {0, 0, 0}, true};
		}
		else
		{
			idMat = m_ProcessData.size();
			m_ProcessData.push_back({idMat, {0, 0, 0}, true});
		}
		_mUbm.Allocate(idMat);
		GraphicsRenderer::GetInstance()->HasChange();
		return idMat;
	}

	void ComputePipeline::RemoveId(uint32_t id)
	{
		if (!m_ProcessData[id].cached)
			m_ToDraw.erase(id);

		GraphicsRenderer::GetInstance()->HasChange();

		_freeId.push_back(id);
		//_mUbm.Free(id);
	}
	void ComputePipeline::SetWorkGroup(uint32_t matId, uint32_t x, uint32_t y, uint32_t z)
	{
		m_ProcessData[matId].workGroup[0] = x;
		m_ProcessData[matId].workGroup[1] = y;
		m_ProcessData[matId].workGroup[2] = z;
	}
}
