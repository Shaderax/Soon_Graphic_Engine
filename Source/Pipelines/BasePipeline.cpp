#include "Pipelines/BasePipeline.hpp"
#include "GraphicsRenderer.hpp"
#include "UniformsBufferManager.hpp"

namespace Soon
{
	BasePipeline::BasePipeline( void )
	{
	}

	BasePipeline::~BasePipeline( void )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		vkDestroyPipeline(device, _graphicPipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

		DestroyAllUniforms();
	}

	void BasePipeline::DestroyAllUniforms(void)
	{
		_mUbm.DestroyAllUniforms();
	}

	void BasePipeline::DestroyGraphicPipeline(void)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		vkDestroyPipeline(device, _graphicPipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
	}

	void BasePipeline::UpdateData( int currentImg )
	{
		_mUbm.UpdateToGPU(currentImg);
	}

	int32_t BasePipeline::IsDefaultVertexInput(std::string name)
	{
		for (uint32_t index = 0; index < DefaultVertexInput.size(); index++)
		{
			if (DefaultVertexInput[index].inputName == name)
				return (index);
		}
		return (-1);
	}

	int32_t BasePipeline::IsDefaultUniform(std::string name)
	{
		for (uint32_t index = 0; index < _defaultUniform.size(); index++)
		{
			if (_defaultUniform[index].name == name)
				return (index);
		}
		return (-1);
	}

	int32_t BasePipeline::IsValidDefaultUniform(SpvReflectDescriptorBinding *block, int32_t index)
	{
		assert((index < 0 || index >= _defaultUniform.size()));

		//if ((block->type_description->member_count != _defaultUniform[index].members.size()) || (block->name != _defaultUniform[index].name))
		//	return (-1);

		//for (int32_t indexMember = 0; index < block->type_description->member_count; indexMember++)
		//{
		//	VertexElementType memberType = SpvTypeToVertexType(&(block->type_description->members[indexMember]));
		//	if (memberType != _defaultUniform[index].members[indexMember].tt)
		//		return (-1);
		//}
		return (0);
	}

	bool BasePipeline::SetDefaultUniform(DefaultUniformStruct structure)
	{
		// Check if already exist
		_defaultUniform.push_back(structure);

		return (true);
	}

	void BasePipeline::GetBindingDescription( void )
	{
		_bindingDescription.binding = 0;
		_bindingDescription.stride = _vertexDescription.GetVertexStrideSize(); // stride : size of one pointe
		_bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	void BasePipeline::Set(std::string name, void *value, uint32_t id)
	{
		_mUbm.Set( name, value, id );
	}

	void BasePipeline::RecreateUniforms(void)
	{
		_mUbm.RecreateUniforms(m_ToDraw);
	}

	void BasePipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage)
	{
		std::cout << "Bind Caller for the Current Image : " << currentImage << std::endl;
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicPipeline);

		VkDeviceSize offsets[] = {0};
		uint32_t dynamicOffset = 0;

		//_mUbm.GetUniqueUniforms();
		std::vector<Uniform>& uniforms = _mUbm.GetNonUniqueUniforms();

		for ( std::unordered_map<uint32_t, uint32_t>::iterator it = m_ToDraw.begin(); it != m_ToDraw.end(); ++it )
		{
			MeshBufferRenderer &bu = GraphicsRenderer::GetInstance()->GetMesh(m_RenderData[it->second].meshId);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(bu.vertex.buffer), offsets);

			vkCmdBindIndexBuffer(commandBuffer, bu.indices.buffer, 0, VK_INDEX_TYPE_UINT32);

			// TODO: Here
			for (uint32_t uniformId = 0; uniformId < uniforms.size(); uniformId++)
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, uniforms[uniformId]._set, 1, &(_mUbm.GetDescriptorSet(currentImage)[uniformId + (uniforms.size() * it->second)]), 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, bu.indices.numIndices, 1, 0, 0, 0);
		}
	}

	VertexDescription BasePipeline::GetVertexDescription()
	{
		return (_vertexDescription);
	}

	void BasePipeline::Render(uint32_t id)
	{
		if (!m_RenderData[id].cached)
			return ;
		m_RenderData[id].cached = false;
		m_ToDraw[id] = id;
	}

	void BasePipeline::UnRender(uint32_t id)
	{
		m_ToDraw.erase(id);
		m_RenderData[id].cached = true;
	}

	uint32_t BasePipeline::AddToPipeline( std::uint32_t meshId )
	{
		uint32_t idMat;

		if (!_freeId.empty())
		{
			idMat = _freeId.back();
			_freeId.pop_back();
			m_RenderData[idMat] = {idMat, meshId, false};
			m_ToDraw[idMat] = idMat;
		}
		else
		{
			idMat = m_RenderData.size();
			m_RenderData.push_back({idMat, meshId, false});
			m_ToDraw[idMat] = idMat;
		}
		_mUbm.Allocate(idMat);
		return idMat;
	}

/*
	void BasePipeline::SetMesh( uint32_t matId, uint32_t meshId )
	{

	}
*/

	void BasePipeline::RemoveFromPipeline(uint32_t id)
	{
		if (!m_RenderData[id].cached)
			m_ToDraw.erase(id);
		_freeId.push_back(id);
		//_mUbm.Free(id);
	}

	/////////// GET SHADER DATA /////////////
	
	void BasePipeline::GetInputBindings( spv_reflect::ShaderModule& reflection )
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
				std::cout << inputs[index]->name << std::endl;

				assert(indexDefault != -1 && "Is Not default Vertex");

				input.sementic = DefaultVertexInput[indexDefault].type;
				input.type = SpvTypeToVertexType(inputs[index]->type_description);
				std::cout << "Base Type : " << (int)input.type.baseType << " Column : " << input.type.column << " Row : " << input.type.row << std::endl;

				_vertexDescription.AddVertexElement(input);

				VkVertexInputAttributeDescription attributeDes;
				attributeDes.binding = 0;
				attributeDes.format = VertexTypeToVkFormat(SpvTypeToVertexType(inputs[index]->type_description));
				attributeDes.location = inputs[index]->location;
				attributeDes.offset = offset;

				_attributeDescriptions.push_back(attributeDes);

				offset += input.type.GetTypeSize();
				/*
        std::cout << inputs[index]->name << std::endl;
        std::cout << inputs[index]->type_description->type_flags << std::endl;
        std::cout << inputs[index]->format << std::endl;
        */
			}
		}

	}

	void BasePipeline::GetDescriptorBindings( spv_reflect::ShaderModule& reflection )
	{

		SpvReflectResult result = SPV_REFLECT_RESULT_NOT_READY;
		uint32_t count = 0;
		std::vector<SpvReflectDescriptorBinding *> bindings;
		/// Descriptor Binding ///

		result = reflection.EnumerateDescriptorBindings(&count, nullptr);

		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		bindings.resize(count);
		reflection.EnumerateDescriptorBindings(&count, bindings.data());

		for (uint32_t index = 0; index < count; index++)
		{
			if (IsImageType(bindings[index]->type_description->type_flags))
			{
				UniformTexture texture;
				texture._binding = bindings[index]->binding;
				texture._set = bindings[index]->set;
				texture._name = bindings[index]->name;
				texture._type = SpvTypeToVertexType(bindings[index]->type_description);

				int32_t indexDefault = IsDefaultUniform(bindings[index]->name);
				//texture.isTexture = true;
				// TODO: Unique
				texture._updateFunct = nullptr;
				_mUbm.AddUniform(texture);
				//_uniformsTexture.push_back(texture);
			}
			else
			{
				Uniform uniform;
				uniform._name = bindings[index]->name;
				uniform._binding = bindings[index]->binding;
				uniform._set = bindings[index]->set;
				uniform._size = bindings[index]->block.size;
				for (uint32_t indexMember = 0; indexMember < bindings[index]->block.member_count; indexMember++)
				{
					UniformVar uniVar;
					uniVar._name = bindings[index]->block.members[indexMember].name;
					uniVar._offset = bindings[index]->block.members[indexMember].offset;
					uniVar._size = bindings[index]->block.members[indexMember].size;

					uniVar._type = SpvTypeToVertexType(bindings[index]->block.members[indexMember].type_description);

					uniform._members.push_back(uniVar);
				}
				/*
				int32_t defaultIndex = IsDefaultUniform(bindings[index]->name);
				if (defaultIndex >= 0 && IsValidDefaultUniform(bindings[index], defaultIndex))
				{
					uniform.isUnique = _defaultUniform[defaultIndex].isUnique;
					//uniform._updateFunct = _defaultUniform[defaultIndex].updateFunc;
				}
				else
				{
					uniform.isUnique = false;
					uniform._updateFunct = nullptr;
				}
				*/
				// IF UNIQUE ADD_UNIQUE_UNIFORM()
				_mUbm.AddUniform(uniform);
			}
			// std::cout << "Size :: " << bindings[index]->block.size << std::endl;
			// uboLayout
			VkDescriptorSetLayoutBinding ubo;
			ubo.binding = bindings[index]->binding;
			ubo.descriptorCount = (bindings[index]->array.dims_count != 0) ? bindings[index]->array.dims[0] : 1;
			ubo.descriptorType = DescriptorTypeSpvToVk(bindings[index]->descriptor_type); //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			std::cout << ubo.descriptorType << std::endl;
			ubo.pImmutableSamplers = nullptr;
			ubo.stageFlags = reflection.GetShaderModule().shader_stage;

			_mUbm.AddLayoutBinding(ubo, bindings[index]->set);
		}
	}

	void BasePipeline::GetShaderData(std::string _path)
	{
		std::vector<char> spirvFile = ReadFile(_path);

		spv_reflect::ShaderModule reflection(spirvFile.size(), spirvFile.data());

		if (reflection.GetResult() != SPV_REFLECT_RESULT_SUCCESS)
		{
			std::cerr << "ERROR: could not process '"
					  << "./SPV_Reader/Ressources/Shaders/DefaultShader.vert.spv"
					  << "' (is it a valid SPIR-V bytecode?)" << std::endl;
			return;
		}

		GetInputBindings(reflection);
		GetDescriptorBindings(reflection);
	}
	///////////////////////////
} // namespace Soon