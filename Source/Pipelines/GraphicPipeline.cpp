#include "Pipelines/GraphicPipeline.hpp"
#include "GraphicsRenderer.hpp"

#include <stdexcept>

namespace Soon
{
	GraphicPipeline::GraphicPipeline(GraphicPipelineConf* conf) : BasePipeline(conf), graphicConf(conf)
	{
	}

	GraphicPipeline::~GraphicPipeline()
	{
		//if (_graphicPipeline != nullptr)
		//	vkDestroyPipeline(GraphicsInstance::GetInstance()->GetDevice(), _graphicPipeline, nullptr);
	}

	void GraphicPipeline::Init( void )
	{
		for (PipelineStage stage : graphicConf->GetStages())
			GetShaderData(stage.shaderPath);

		GetBindingDescription();

		graphicConf->vertexInputInfo.vertexBindingDescriptionCount = 1;
		graphicConf->vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributeDescriptions.size());
		graphicConf->vertexInputInfo.pVertexBindingDescriptions = &_bindingDescription;
		graphicConf->vertexInputInfo.pVertexAttributeDescriptions = _attributeDescriptions.data();

		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.CreateDescriptorSetLayout());
		graphicConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(dynamic_cast<PipelineConf*>(graphicConf));

		_mUbm.InitBuffers();

		// Alloc Camera
	}

	// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
	void GraphicPipeline::RecreatePipeline(void)
	{
		VkExtent2D Extent = GraphicsInstance::GetInstance()->GetSwapChainExtent();
		graphicConf->viewport.width = (float)Extent.width;
		graphicConf->viewport.height = (float)Extent.height;

		graphicConf->scissor.extent = Extent;

		graphicConf->pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.GetDescriptorSetLayout());
		graphicConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(graphicConf);
	}
	
	void GraphicPipeline::GetBindingDescription( void )
	{
		_bindingDescription.binding = 0;
		_bindingDescription.stride = _vertexDescription.GetVertexStrideSize(); // stride : size of one pointe
		_bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	VertexDescription GraphicPipeline::GetVertexDescription()
	{
		return (_vertexDescription);
	}

	void GraphicPipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage)
	{
		std::cout << "Bind Caller for the Current Image : " << currentImage << std::endl;

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
			MeshBufferRenderer &bu = GraphicsRenderer::GetInstance()->GetMesh(m_RenderData[it->second].meshId);

			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(bu.vertex.buffer), offsets);
			vkCmdBindIndexBuffer(commandBuffer, bu.indices.buffer, 0, VK_INDEX_TYPE_UINT32);

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
			vkCmdDrawIndexed(commandBuffer, bu.indices.numIndices, 1, 0, 0, 0);
		}
	}

	void GraphicPipeline::Render(uint32_t id)
	{
		if (!m_RenderData[id].cached || m_RenderData[id].meshId == Soon::IdError)
			return ;
		m_RenderData[id].cached = false;
		m_ToDraw[id] = id;
		GraphicsRenderer::GetInstance()->HasChange();
	}

	void GraphicPipeline::UnRender(uint32_t id)
	{
		if (m_RenderData[id].cached || id == Soon::IdError)
			return ;
		m_ToDraw.erase(id);
		m_RenderData[id].cached = true;
		GraphicsRenderer::GetInstance()->HasChange();
	}

	uint32_t GraphicPipeline::CreateNewId( void )
	{
		uint32_t idMat;

		if (!_freeId.empty())
		{
			idMat = _freeId.back();
			_freeId.pop_back();
			m_RenderData[idMat] = {idMat, Soon::IdError, true};
			//m_ToDraw[idMat] = idMat;
		}
		else
		{
			idMat = m_RenderData.size();
			m_RenderData.push_back({idMat, Soon::IdError, true});
			//m_ToDraw[idMat] = idMat;
		}
		_mUbm.Allocate(idMat);
		GraphicsRenderer::GetInstance()->HasChange();

		return idMat;
	}

	void GraphicPipeline::RemoveId(uint32_t id)
	{
		if (!m_RenderData[id].cached)
			m_ToDraw.erase(id);

		GraphicsRenderer::GetInstance()->HasChange();

		_freeId.push_back(id);
		//_mUbm.Free(id);
	}

	void GraphicPipeline::SetMeshId(uint32_t matId, uint32_t meshId)
	{
		m_RenderData[matId].meshId = meshId;
	}

	/**
	 *  GET SHADER DATA
	 */
	void GraphicPipeline::GetInputBindings( spv_reflect::ShaderModule& reflection )
	{
		std::vector<SpvReflectInterfaceVariable *> inputs;
		SpvReflectResult result = SPV_REFLECT_RESULT_NOT_READY;
		uint32_t count = 0;
		/// INPUT ///
		if (reflection.GetShaderStage() == SpvReflectShaderStageFlagBits::SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
		{
			result = reflection.EnumerateInputVariables(&count, nullptr);

			assert(result == SPV_REFLECT_RESULT_SUCCESS);

			inputs.resize(count);
			reflection.EnumerateInputVariables(&count, inputs.data());

			uint32_t offset = 0;

			for (uint32_t index = 0; index < count; index++)
			{
				VertexElement input;
				int indexDefault;

				indexDefault = IsDefaultVertexInput(inputs[index]->name);

				if (indexDefault == -1)
					throw std::runtime_error("Is Not default Vertex");

				input.sementic = DefaultVertexInput[indexDefault].type;
				input.type = SpvTypeToVertexType(inputs[index]->type_description);

				_vertexDescription.AddVertexElement(input);

				VkVertexInputAttributeDescription attributeDes;
				attributeDes.binding = 0;
				std::cout << std::endl << "Location: " << inputs[index]->location << std::endl;
				attributeDes.format = VertexTypeToVkFormat(SpvTypeToVertexType(inputs[index]->type_description));
				attributeDes.location = inputs[index]->location;
				attributeDes.offset = offset;

				_attributeDescriptions.push_back(attributeDes);

				offset += input.type.GetTypeSize();

        		std::cout << inputs[index]->name << std::endl;
				std::cout << "Base Type : " << (int)input.type.baseType << " Column : " << input.type.column << " Row : " << input.type.row << std::endl;
			}
		}
	}
} // namespace Soon