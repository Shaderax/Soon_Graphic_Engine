#include "Pipelines/GraphicPipeline.hpp"
#include "GraphicsRenderer.hpp"

#include <stdexcept>

#include "Utilities/Error.hpp"

namespace Soon
{
	GraphicPipeline::GraphicPipeline(GraphicPipelineConf* conf) : BasePipeline(conf), graphicConf(conf)
	{
	}

	GraphicPipeline::~GraphicPipeline()
	{		
		for ( std::unordered_map<uint32_t, uint32_t>::iterator it = m_ToDraw.begin(); it != m_ToDraw.end(); ++it )
		{
			GraphicsRenderer::GetInstance()->RemoveMesh(m_RenderData[it->second].inputId[0]);
			uint32_t index  = 0;
			for (InputBindingDescription& input : m_VertexInput)
			{
				GraphicsRenderer::GetInstance()->RemoveBuffer(m_RenderData[it->second].inputId[index]);
				++index;
			}
		}
		for (uint32_t index = 0 ; index < m_RenderData.size() ; index++)
			delete[] m_RenderData[index].inputId;
	}

	void GraphicPipeline::Init( void )
	{
		for (PipelineStage stage : graphicConf->GetStages())
			GetShaderData(stage.shaderPath);

		std::vector<VkVertexInputBindingDescription> bindings = GetBindingDescription();

		graphicConf->vertexInputInfo.vertexBindingDescriptionCount = bindings.size();
		graphicConf->vertexInputInfo.pVertexBindingDescriptions = bindings.data();

		graphicConf->vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
		graphicConf->vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();

		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.CreateDescriptorSetLayout());
		graphicConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(dynamic_cast<PipelineConf*>(graphicConf));

		_mUbm.InitBuffers();
	}

	// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
	void GraphicPipeline::RecreatePipeline(void)
	{
		VkExtent2D Extent = GraphicsInstance::GetInstance()->GetSwapChainExtent();
		graphicConf->viewport.width = (float)Extent.width;
		graphicConf->viewport.height = (float)Extent.height;

		graphicConf->scissor.extent = Extent;

/**/
		std::vector<VkVertexInputBindingDescription> bindings = GetBindingDescription();

		graphicConf->vertexInputInfo.vertexBindingDescriptionCount = bindings.size();
		graphicConf->vertexInputInfo.pVertexBindingDescriptions = bindings.data();
		graphicConf->vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
		graphicConf->vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();

		//for (MeshVertexElement& a: m_MeshVertexInput.data)
		//{
		//	std::cout << "Mesh offset: " << a.mOffset << std::endl;
		//	std::cout << "Mesh sementic: " << (int)a.sementic << std::endl;
		//	std::cout << "Mesh typeSize: " << a.type.GetTypeSize() << std::endl;
		//}
		//for (VkVertexInputBindingDescription& a: bindings)
		//{
		//	std::cout << "Binding binding: " << a.binding << std::endl;
		//	std::cout << "Binding inputRate: " << a.inputRate << std::endl;
		//	std::cout << "Binding stride: " << a.stride << std::endl;
		//}
		//for (VkVertexInputAttributeDescription& a : m_AttributeDescriptions)
		//{
		//	std::cout << "Attribute Binding: " << a.binding << std::endl;
		//	std::cout << "Attribute Format: " << a.format << std::endl;
		//	std::cout << "Attribute Location: " << a.location << std::endl;
		//	std::cout << "Attribute Offset: " << a.offset << std::endl;
		//}
/**/

		graphicConf->pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.GetDescriptorSetLayout());
		graphicConf->pipelineInfo.layout = _pipelineLayout;
		_pipeline = GraphicsInstance::GetInstance()->CreatePipeline(graphicConf);
	}
	
	std::vector<VkVertexInputBindingDescription> GraphicPipeline::GetBindingDescription( void )
	{
		std::vector<VkVertexInputBindingDescription> bindings;

		bindings.push_back({0, m_MeshVertexInput.GetVertexStrideSize(), VK_VERTEX_INPUT_RATE_VERTEX});

		for (const InputBindingDescription& it : m_VertexInput)
			bindings.push_back({it.mBinding, it.mDescription.GetVertexStrideSize(), it.mInputRate});
		
		return bindings;
	}

	void GraphicPipeline::SetNumInstance(uint32_t id, uint32_t instances)
	{
		//TODO: if (IsValidId)
		m_RenderData[id].numInstance = instances;
	}

	MeshVertexDescription GraphicPipeline::GetMeshVertexDescription()
	{
		return (m_MeshVertexInput);
	}

	void GraphicPipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage)
	{
		if (m_ToDraw.empty()) // MayBe Delete
			return ;


		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

		std::vector<DescriptorSetDescription>& sets = _mUbm.GetSets();
		std::vector<DescriptorSetDescription>& uniqueSets = _mUbm.GetUniqueSets();
		std::vector<VkDescriptorSet>& vecDescriptor = _mUbm.GetDescriptorSet(currentImage);

		for (uint32_t uniqueId = 0 ; uniqueId < uniqueSets.size() ; uniqueId++)
		{
			vkCmdBindDescriptorSets(commandBuffer,
									VK_PIPELINE_BIND_POINT_GRAPHICS,
									_pipelineLayout,
									uniqueSets[uniqueId].set,
									1,
									&(vecDescriptor[uniqueId]), 0, nullptr);
		}

		uint32_t index = 0;
		VkDeviceSize offsets[1] = {0};
		// Need to rework this
		// Maybe pool of mem and list outsize ?
		for ( std::unordered_map<uint32_t, uint32_t>::iterator it = m_ToDraw.begin(); it != m_ToDraw.end(); ++it )
		{
			//std::cout << "To Draw : " << it->second << std::endl;
			MeshBufferRenderer &bu = GraphicsRenderer::GetInstance()->GetMesh(m_RenderData[it->second].inputId[0]);

			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(bu.vertex.buffer), offsets);
			vkCmdBindIndexBuffer(commandBuffer, bu.indices.buffer, 0, VK_INDEX_TYPE_UINT32);

			index = 1;
			for (InputBindingDescription& input : m_VertexInput)
			{
				//std::cout << "BaseOffset in bind: " << input.mDescription.mBaseOffset << std::endl;
				//std::cout << "Stride in bind: " << input.mDescription.strideSize << std::endl;
				//std::cout << "InstanceRate in bind: " << input.mInputRate << std::endl;
				//std::cout << "Bind in bind: " << input.mBinding << std::endl;

				BufferRenderer& buffer = GraphicsRenderer::GetInstance()->GetBufferRenderer(m_RenderData[it->second].inputId[index]);
				vkCmdBindVertexBuffers(commandBuffer, input.mBinding, 1, &(buffer.GetBuffer()), &(input.mDescription.mBaseOffset));
				++index;
			}

			for (uint32_t setId = 0 ; setId < sets.size() ; setId++)
			{
				vkCmdBindDescriptorSets(commandBuffer,
										VK_PIPELINE_BIND_POINT_GRAPHICS,
										_pipelineLayout,
										sets[setId].set,
										1,
										&(vecDescriptor[uniqueSets.size() + setId + (sets.size() * it->second)]),
										0,
										nullptr);
			}
			//std::cout << "Instance: " << m_RenderData[it->second].numInstance << std::endl;
			vkCmdDrawIndexed(commandBuffer, bu.indices.numIndices, m_RenderData[it->second].numInstance, 0, 0, 0);
		}
	}

	bool GraphicPipeline::IsValidToRender(uint32_t id) const
	{
		if (!m_RenderData[id].cached || id >= m_RenderData.size())
		{
			std::cout << "First" << std::endl;
			return false;
		}

		for (uint32_t index = 0 ; index < m_VertexInput.size() + 1; index++) // Should i iterate on my list vertexInut ?
		{
			if (m_RenderData[id].inputId[index] == Soon::IdError)
			{
				std::cout << "Second: " << index << std::endl;
				return false;
			}
		}
		return true;
	}

	void GraphicPipeline::Render(uint32_t id)
	{
		if (!IsValidToRender(id))
		{
			std::cout << "Id Not Valid For Render: " << id << std::endl;
			return ;
		}
		m_RenderData[id].cached = false;
		m_ToDraw[id] = id;
		GraphicsRenderer::GetInstance()->HasChange();
	}

	void GraphicPipeline::UnRender(uint32_t id)
	{
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
			for (uint32_t index = 0 ; index < m_VertexInput.size() + 1 ; index++)
				m_RenderData[idMat].inputId[index] = Soon::IdError;
			m_RenderData[idMat].matId = idMat;
			m_RenderData[idMat].cached = true;
			m_RenderData[idMat].numInstance = 1;
		}
		else
		{
			idMat = m_RenderData.size();
			uint32_t* ids = new uint32_t[m_VertexInput.size() + 1]();
			for (uint32_t index = 0 ; index < m_VertexInput.size() + 1 ; index++)
				ids[index] = Soon::IdError;
			m_RenderData.push_back({idMat, ids, 1, true});
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

		uint32_t offsetMesh = 0;
		uint32_t offsetVertex = 0;

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
				attributeDes.offset = offsetMesh;

				m_AttributeDescriptions.push_back(attributeDes);

        		DEBUG("\n", inputs[index]->name);
				DEBUG("Location: ", inputs[index]->location);
				DEBUG("Base Type : ", (int)input.type.baseType, " Column : ", input.type.column, " Row : ", input.type.row);
				DEBUG("Ofset: ", offsetMesh);

				offsetMesh += input.type.GetTypeSize();
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
				attributeDes.offset = offsetVertex;

				m_AttributeDescriptions.push_back(attributeDes);

				offsetVertex += input.type.GetTypeSize();

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

		it->mDescription.mBaseOffset = description.mBaseOffset;
		it->mDescription.strideSize = description.strideSize;
		DEBUG("InputVertex Binding: ", it->mBinding);
		DEBUG("InputVertex BaseOffset: ", it->mDescription.mBaseOffset);
		DEBUG("InputVertex StrideSize: ", it->mDescription.strideSize);
		DEBUG("InputVertex Rate: ", it->mInputRate);

		auto itAttr = std::find_if(m_AttributeDescriptions.begin(), m_AttributeDescriptions.end(), [&binding](VkVertexInputAttributeDescription& a){ return binding == a.binding;});
		for (uint32_t index = 0 ; index < description.data.size() ; index++)
		{
			itAttr->offset = description.data[index].mOffset;
			DEBUG("Attribute Location: ", itAttr->location);
			DEBUG("Attribute offset: ", itAttr->offset);
			++itAttr;
		}		

		GraphicsRenderer::GetInstance()->AddPipelineToRecreate(_conf->GetJsonPath());
	}

	void GraphicPipeline::SetBindingVertexInput( uint32_t idMat, uint32_t binding, const GpuBuffer& buffer)
	{
		if (binding == 0)
			throw std::runtime_error("Not Good");
		
		uint32_t index = 1;
		for ( InputBindingDescription& input : m_VertexInput )
		{
			if (input.mBinding == binding)
			{
				// TODO: idMat Checker
				if (m_RenderData[idMat].inputId[index] != Soon::IdError)
					GraphicsRenderer::GetInstance()->RemoveBuffer(m_RenderData[idMat].inputId[index]);

				GraphicsRenderer::GetInstance()->AddBuffer(buffer.GetId());
				m_RenderData[idMat].inputId[index] = buffer.GetId();
				return ;
			}
			++index;
		}
		throw std::runtime_error("Binding Not Found");
	}
} // namespace Soon