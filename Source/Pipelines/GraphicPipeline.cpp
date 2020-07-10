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
			
		for ( std::unordered_map<uint32_t, uint32_t>::iterator it = m_ToDraw.begin(); it != m_ToDraw.end(); ++it )
		{
			for (uint32_t index = 0 ; index < m_BindingDescription.size() ; index++)
			{
				if (index == 0)
					GraphicsRenderer::GetInstance()->RemoveMesh(m_RenderData[it->second].inputId[index]);
				GraphicsRenderer::GetInstance()->RemoveBuffer(m_RenderData[it->second].inputId[index]);
			}
		}
		for (uint32_t index = 0 ; index < m_RenderData.size() ; index++)
			delete[] m_RenderData[index].inputId;
	}

	void GraphicPipeline::Init( void )
	{
		for (PipelineStage stage : graphicConf->GetStages())
			GetShaderData(stage.shaderPath);

		GetBindingDescription();

		graphicConf->vertexInputInfo.vertexBindingDescriptionCount = 1 + m_VertexInput.size();
		graphicConf->vertexInputInfo.pVertexBindingDescriptions = m_BindingDescription.data();

		graphicConf->vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
		graphicConf->vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();

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
		m_BindingDescription.push_back({0, m_MeshVertexInput.GetVertexStrideSize(), VK_VERTEX_INPUT_RATE_VERTEX});

		for (const InputBindingDescription& it : m_VertexInput)
			m_BindingDescription.push_back({it.mBinding, it.mDescription.GetVertexStrideSize(), it.mInputRate});
	}

	MeshVertexDescription GraphicPipeline::GetMeshVertexDescription()
	{
		return (m_MeshVertexInput);
	}

	void GraphicPipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage)
	{
		std::cout << graphicConf->GetStages()[0].shaderPath << std::endl;
		std::cout << "\t Bind Caller for the Current Image : " << currentImage << std::endl;

		if (m_ToDraw.empty())
			return ;

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
		// Need to rework this
		// Maybe pool of mem and list outsize ?
		for ( std::unordered_map<uint32_t, uint32_t>::iterator it = m_ToDraw.begin(); it != m_ToDraw.end(); ++it )
		{
			std::cout << "To Draw : " << it->second << std::endl;
			MeshBufferRenderer &bu = GraphicsRenderer::GetInstance()->GetMesh(m_RenderData[it->second].inputId[0]);

			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(bu.vertex.buffer), offsets);
			vkCmdBindIndexBuffer(commandBuffer, bu.indices.buffer, 0, VK_INDEX_TYPE_UINT32);

			for (uint32_t index = 1 ; index < m_BindingDescription.size() ; index++)
			{
				if (m_RenderData[it->second].inputId[index] == Soon::IdError)
					throw std::runtime_error(std::string("BufferId not feed for binding : ") + std::to_string(index));
				BufferRenderer& buffer = GraphicsRenderer::GetInstance()->GetBufferRenderer(m_RenderData[it->second].inputId[index]);
				vkCmdBindVertexBuffers(commandBuffer, m_BindingDescription[index].binding, 1, &(buffer.GetBuffer()), offsets);
			}

			for (uint32_t setId = 0 ; setId < sets.size() ; setId++)
			{
				std::cout << vecDescriptor[uniqueSets.size() + setId + (sets.size() * it->second)] << std::endl;
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
		if (!m_RenderData[id].cached || m_RenderData[id].inputId[0] == Soon::IdError)
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
			for (uint32_t index = 0 ; index < m_BindingDescription.size() ; index++)
				m_RenderData[idMat].inputId[index] = Soon::IdError;
			m_RenderData[idMat].matId = idMat;
			m_RenderData[idMat].cached = true;
		}
		else
		{
			idMat = m_RenderData.size();
			uint32_t* ids = new uint32_t[m_BindingDescription.size()]();
			for (uint32_t index = 0 ; index < m_BindingDescription.size() ; index++)
				ids[index] = Soon::IdError;
			m_RenderData.push_back({idMat, ids, true});
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
		//_mUbm.Free(id); TODO:
	}

	void GraphicPipeline::SetMeshId(uint32_t matId, uint32_t meshId)
	{
		if (m_RenderData[matId].inputId[0] != Soon::IdError)
			GraphicsRenderer::GetInstance()->RemoveMesh(m_RenderData[matId].inputId[0]);
		GraphicsRenderer::GetInstance()->AddMesh(meshId);
		m_RenderData[matId].inputId[0] = meshId;
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
		if (reflection.GetShaderStage() != SpvReflectShaderStageFlagBits::SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
			return ;
		result = reflection.EnumerateInputVariables(&count, nullptr);

		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		inputs.resize(count);
		reflection.EnumerateInputVariables(&count, inputs.data());

		uint32_t offset = 0;

		for (uint32_t index = 0; index < count; index++)
		{
			if (graphicConf->IsMeshDefaultVertexInput(inputs[index]->name))
			{
				const DefaultMeshVertexInput& defaultVertex = graphicConf->GetMeshDefaultVertexInput(inputs[index]->name);

				MeshVertexElement input;
				input.sementic = defaultVertex.sementic;
				input.type = SpvTypeToVertexType(inputs[index]->type_description);

				m_MeshVertexInput.AddVertexElement(input);

				VkVertexInputAttributeDescription attributeDes;
				attributeDes.binding = 0;
				attributeDes.format = VertexTypeToVkFormat(SpvTypeToVertexType(inputs[index]->type_description));
				attributeDes.location = inputs[index]->location;
				attributeDes.offset = offset;

				m_AttributeDescriptions.push_back(attributeDes);

        		std::cout << inputs[index]->name << std::endl;
				std::cout << std::endl << "Location: " << inputs[index]->location << std::endl;
				std::cout << "Base Type : " << (int)input.type.baseType << " Column : " << input.type.column << " Row : " << input.type.row << std::endl;

				offset += input.type.GetTypeSize();
			}
			else if (graphicConf->IsDefaultVertexInput(inputs[index]->name))
			{
				const DefaultInputBinding& defaultVertex = graphicConf->GetDefaultVertexInput(inputs[index]->name);

				VertexElement input;
				uint32_t defaultBinding = defaultVertex.mBinding;
				input.type = SpvTypeToVertexType(inputs[index]->type_description);

				auto it = std::find_if(m_VertexInput.begin(), m_VertexInput.end(),
													[&defaultBinding](const InputBindingDescription& x) { return x.mBinding == defaultBinding;});
				if (it != m_VertexInput.end())
					it->mDescription.AddVertexElement(input);
				else
				{
					VertexDescription des;
					des.AddVertexElement(input);
					m_VertexInput.push_back({defaultBinding, defaultVertex.mInputRate, des});
				}

				VkVertexInputAttributeDescription attributeDes;
				attributeDes.binding = defaultBinding;
				attributeDes.format = VertexTypeToVkFormat(SpvTypeToVertexType(inputs[index]->type_description));
				attributeDes.location = inputs[index]->location;
				attributeDes.offset = offset;

				m_AttributeDescriptions.push_back(attributeDes);

				offset += input.type.GetTypeSize();

				std::cout << std::endl << "Location: " << inputs[index]->location << std::endl;
    			std::cout << inputs[index]->name << std::endl;
				std::cout << "Base Type : " << (int)input.type.baseType << " Column : " << input.type.column << " Row : " << input.type.row << std::endl;

			}
			else
				throw std::runtime_error(inputs[index]->name + std::string(" Not a default InputBinding"));

			if (m_MeshVertexInput.GetVertexStrideSize() == 0)
				throw std::runtime_error(std::string("No Mesh Input found !"));
		}
	}

	void GraphicPipeline::SetAttributeDescriptionOffset( uint32_t binding, VertexDescription description )
	{
		if (binding == 0 || !m_ToDraw.empty())
			throw std::runtime_error("binding can't be 0 or pipeline not empty");

		auto it = std::find_if(m_VertexInput.begin(), m_VertexInput.end(), [&binding](InputBindingDescription& input){ return input.mBinding == binding; });
		if (it == m_VertexInput.end())
			throw std::runtime_error("No Binding");

		for (uint32_t index = 0 ; index < description.data.size() ; index++)
		{
			if (description.data[index].type != it->mDescription.data[index].type)
				throw std::runtime_error("Not Good");
		}

		auto itAttr = std::find_if(m_AttributeDescriptions.begin(), m_AttributeDescriptions.end(), [&binding](VkVertexInputAttributeDescription& a){ return binding == a.binding;});
		for (uint32_t index = 0 ; index < description.data.size() ; index++)
		{
			// TODO: REVOIR LARCHI DES ATTRIBU
			itAttr->offset = description.data[index].mOffset;
		}
	}

	void GraphicPipeline::SetBindingVertexInput( uint32_t idMat, uint32_t binding, GpuBuffer& buffer)
	{
		if (binding == 0)
			throw std::runtime_error("Not Good");
		
		for ( uint32_t index = 0 ; index < m_BindingDescription.size() ; index++)
		{
			if (m_BindingDescription[index].binding == binding)
			{
				// TODO: idMat Checker
				if (m_RenderData[idMat].inputId[index] == Soon::IdError)
					GraphicsRenderer::GetInstance()->RemoveBuffer(m_RenderData[idMat].inputId[index]);

				GraphicsRenderer::GetInstance()->AddBuffer(buffer.GetId());
				m_RenderData[idMat].inputId[index] = buffer.GetId();
				return ;
			}
		}
	}
} // namespace Soon