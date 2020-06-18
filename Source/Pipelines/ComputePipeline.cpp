#include "ComputePipeline.hpp"
#include "GraphicsRenderer.hpp"

namespace Soon
{
	ComputePipeline::ComputePipeline(ComputePipelineConf* conf) : BasePipeline(conf), computeConf(conf)
	{
	}

	ComputePipeline::~ComputePipeline()
	{
		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.GetDescriptorSetLayout());
		computeConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(computeConf);
	}

	void ComputePipeline::Init( void )
	{
		for (PipelineStage stage : computeConf->GetStages())
			GetShaderData(stage.shaderPath);

		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.CreateDescriptorSetLayout());
		computeConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(computeConf);

		_mUbm.InitBuffers();

		// Alloc Camera

	}

	// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
	void ComputePipeline::RecreatePipeline(void)
	{

	}

	void ComputePipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage)
	{
		std::vector<VkDescriptorSet>& vecDescriptor = _mUbm.GetDescriptorSet(currentImage);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

		std::vector<DescriptorSetDescription>& sets = _mUbm.GetSets();
		std::vector<DescriptorSetDescription>& uniqueSets = _mUbm.GetUniqueSets();

		for (uint32_t uniqueId = 0 ; uniqueId < uniqueSets.size() ; uniqueId++)
		{
			vkCmdBindDescriptorSets(commandBuffer,
									VK_PIPELINE_BIND_POINT_GRAPHICS,
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
				for (uint32_t Id = 0 ; Id < sets[setId].uniformsTexture.size() ; Id++)
					std::cout << sets[setId].uniformsTexture[Id]._textureId[it->second] << std::endl;
				vkCmdBindDescriptorSets(commandBuffer,
										VK_PIPELINE_BIND_POINT_GRAPHICS,
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

	void ComputePipeline::Render(uint32_t id)
	{
		if (!m_ProcessData[id].cached)
			return ;
		m_ProcessData[id].cached = false;
		m_ToDraw[id] = id;
		GraphicsRenderer::GetInstance()->HasChange();
	}

	void ComputePipeline::UnRender(uint32_t id)
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
			//m_ProcessData[idMat] = {idMat, meshId, false};
			m_ToDraw[idMat] = idMat;
		}
		else
		{
			idMat = m_ProcessData.size();
			//m_ProcessData.push_back({idMat, meshId, false});
			m_ToDraw[idMat] = idMat;
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
