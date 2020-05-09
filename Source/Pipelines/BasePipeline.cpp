#include "Renderer/Pipelines/BasePipeline.hpp"
#include "Renderer/Vulkan/GraphicsRenderer.hpp"
#include "UniformsBufferManager.hpp"

namespace Soon
{
	BasePipeline::BasePipeline( void )
	{
	}

	BasePipeline::~BasePipeline( void )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		for (VkDescriptorSetLayout &dsl : _descriptorSetLayout)
			vkDestroyDescriptorSetLayout(device, dsl, nullptr);

		vkDestroyPipeline(device, _graphicPipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

		DestroyAllUniforms();
	}

	void BasePipeline::DestroyAllUniforms(void)
	{
		// TODO:
	}

	void BasePipeline::DestroyGraphicPipeline(void)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		vkDestroyPipeline(device, _graphicPipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
	}

	void BasePipeline::UpdateData(int currentImg)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		void *data = nullptr;

		vec3<float> vec(0.2f, 0.2f, 0.0f);

		vkMapMemory(device, _uniforms[0]._us[0]._uniformRender.bufferMemory[currentImg], 0, _uniforms[0]._members[0]._size, 0, &data);
		memcpy(data, &vec, _uniforms[0]._members[0]._size);
		vkUnmapMemory(device, _uniforms[0]._us[0]._uniformRender.bufferMemory[currentImg]);

		_mUbm.UpdateToGPU();
		// in GPU
		for (uint32_t index = 0 ; index < _toDraw.size() ; index++)
		{
			//_toDraw[index].matId
		}
		// TODO: How do i Auto Update Model ? We don't know.
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

		if ((block->type_description->member_count != _defaultUniform[index].members.size()) || (block->name != _defaultUniform[index].name))
			return (-1);

		for (int32_t indexMember = 0; index < block->type_description->member_count; indexMember++)
		{
			VertexElementType memberType = SpvTypeToVertexType(&(block->type_description->members[indexMember]));
			if (memberType != _defaultUniform[index].members[indexMember].tt)
				return (-1);
		}
		return (0);
	}

	bool BasePipeline::SetDefaultUniform(DefaultUniformStruct structure)
	{
		// Check if already exist
		_defaultUniform.push_back(structure);

		return (true);
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

		SpvReflectResult result = SPV_REFLECT_RESULT_NOT_READY;
		uint32_t count = 0;
		std::vector<SpvReflectDescriptorBinding *> bindings;
		std::vector<SpvReflectInterfaceVariable *> inputs;

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
				texture._data._name = bindings[index]->name;
				texture._data._type = SpvTypeToVertexType(bindings[index]->type_description);

				int32_t indexDefault = IsDefaultUniform(bindings[index]->name);
				//if (indexDefault >= 0 && IsValidDefaultUniform(bindings[index], indexDefault))
				//{
				//	texture.isUnique = _defaultUniform[indexDefault].isUnique;
				//	texture._updateFunct = _defaultUniform[indexDefault].updateFunc;
				//}
				//else
				//{
					texture.isUnique = false;
					texture._updateFunct = nullptr;
				//}
				_uniformsTexture.push_back(texture);
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
				int32_t defaultIndex = IsDefaultUniform(bindings[index]->name);
				if (defaultIndex >= 0 && IsValidDefaultUniform(bindings[index], defaultIndex))
				{
					uniform.isUnique = _defaultUniform[defaultIndex].isUnique;
					uniform._updateFunct = _defaultUniform[defaultIndex].updateFunc;
				}
				else
				{
					uniform.isUnique = false;
					uniform._updateFunct = nullptr;
				}
				_uniforms.push_back(uniform);
			}
			// total Size
			_totalUniformSize += bindings[index]->block.size;
			// std::cout << "Size :: " << bindings[index]->block.size << std::endl;
			// uboLayout
			VkDescriptorSetLayoutBinding ubo;
			ubo.binding = bindings[index]->binding;
			ubo.descriptorCount = (bindings[index]->array.dims_count != 0) ? bindings[index]->array.dims[0] : 1;
			ubo.descriptorType = DescriptorTypeSpvToVk(bindings[index]->descriptor_type); //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			ubo.pImmutableSamplers = nullptr;
			ubo.stageFlags = reflection.GetShaderModule().shader_stage;

			if (uboLayoutBinding.size() <= bindings[index]->set)
				uboLayoutBinding.resize(bindings[index]->set + 1);
			uboLayoutBinding[bindings[index]->set].push_back(ubo);
		}
	}

	void BasePipeline::GetBindingDescription( void )
	{
		_bindingDescription.binding = 0;
		_bindingDescription.stride = _vertexDescription.GetVertexStrideSize(); // stride : size of one pointe
		_bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	uint32_t BasePipeline::AddToPipeline(std::uint32_t meshId)
	{
		// TODO Make VMA in CreateBuffer
		uint32_t idMat;

		if (!_freeId.empty())
		{
			idMat = _freeId.back();
			_freeId.pop_back();
			_toDraw.push_back({idMat, meshId});
		}
		else
		{
			idMat = _toDraw.size();
			_toDraw.push_back({idMat, meshId});
		}

		for (uint32_t index = 0; index < _uniforms.size(); index++)
		{
			UniformSets ds;
			ds._uniformRender = GraphicsInstance::GetInstance()->CreateUniformBuffers(_uniforms[index]._size);
			ds._descriptorSets = GraphicsInstance::GetInstance()->CreateDescriptorSets(_uniforms[index]._size, _uniforms[index]._binding, _descriptorSetLayout[_uniforms[index]._set], ds._uniformRender.buffer.data(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			_uniforms[index]._us.push_back(ds);
		}

		for (uint32_t index = 0; index < _uniformsTexture.size(); index++)
		{
		}
		return 0;
	}

	void BasePipeline::Set(std::string name, void *value, uint32_t id)
	{
		// TODO: If we exced _uniformDataSize
		// Set doit maintenant aller chercher la memoire dans un giga buffer qui est réalloc a chaque fois qu'on dépasse la memoire.
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		uint32_t currentImage = GraphicsInstance::GetInstance()->GetNextIdImageToRender();

		uint32_t offset = 0;

		void *data = nullptr;

		int32_t pos = name.find(".");
		if (pos == std::string::npos)
		{
			for (uint32_t index = 0; index < _uniforms.size(); index++)
			{
				std::cout << _uniforms[index]._name << std::endl;
				if (_uniforms[index]._name == name)
				{
					// Local data
					memcpy(_uniformData + offset + (_totalUniformSize * id), value, _uniforms[index]._size);

					return;
				}
				offset += _uniforms[index]._size;
			}
		}
		else
		{
			std::string varName;
			varName = name.substr(pos + 1);

			for (uint32_t index = 0; index < _uniforms.size(); index++)
			{
				if (_uniforms[index]._name == name.substr(0, pos))
				{
					for (uint32_t member = 0; member < _uniforms[index]._members.size(); member++)
					{
						if (varName == _uniforms[index]._members[member]._name)
						{
							//std::cout << "Found Uniform : " << name.substr(0, pos) + "." << varName << std::endl;
							//vkMapMemory(device, _uniforms[index]._us[id]._uniformRender.bufferMemory[currentImage], 0, _uniforms[index]._members[member]._size, 0, &data);
							//memcpy(data, value, _uniforms[index]._members[member]._size);
							//vkUnmapMemory(device, _uniforms[index]._us[id]._uniformRender.bufferMemory[currentImage]);

							// Local data
							memcpy(_uniformData + offset + (_totalUniformSize * id), value, _uniforms[index]._members[member]._size);
						}
						offset += _uniforms[index]._members[member]._size;
					}
					return;
				}
				offset += _uniforms[index]._size;
			}
		}
	}

	void BasePipeline::SetUniformsArray(void)
	{
		_uniformData = new uint8_t[_totalUniformSize * _minUniformBuffer];
		_uniformDataSize = _totalUniformSize * _minUniformBuffer;
	}

	void BasePipeline::RecreateUniforms(void)
	{
		for (uint32_t index = 0; index < _uniforms.size(); index++)
		{
			for (uint32_t uniform = 0; uniform < _uniforms[index]._us.size(); uniform++)
			{
				_uniforms[index]._us[uniform]._uniformRender = GraphicsInstance::GetInstance()->CreateUniformBuffers(_uniforms[index]._size);
				_uniforms[index]._us[uniform]._descriptorSets = GraphicsInstance::GetInstance()->CreateDescriptorSets(_uniforms[index]._size, _uniforms[index]._binding, _descriptorSetLayout[_uniforms[index]._set], _uniforms[index]._us[uniform]._uniformRender.buffer.data(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			}
		}
	}

	void BasePipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage)
	{
		std::cout << "Bind Caller for the Current Image : " << currentImage << std::endl;
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicPipeline);

		VkDeviceSize offsets[] = {0};

		for (uint32_t index = 0; index < _toDraw.size(); index++)
		{
			MeshBufferRenderer &bu = GraphicsRenderer::GetInstance()->GetMesh(_toDraw[index].meshId);
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(bu.vertex.buffer), offsets);

			vkCmdBindIndexBuffer(commandBuffer, bu.indices.buffer, 0, VK_INDEX_TYPE_UINT32);

			for (uint32_t uniforms = 0; uniforms < _uniforms.size(); uniforms++)
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, _uniforms[uniforms]._set, 1, &(_uniforms[uniforms]._us[_toDraw[index].matId]._descriptorSets[currentImage]), 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, bu.indices.numIndices, 1, 0, 0, 0);
		}
	}

	VertexDescription BasePipeline::GetVertexDescription()
	{
		return (_vertexDescription);
	}

	void BasePipeline::Render(uint32_t id)
	{
		(void)id;
	}

	void BasePipeline::UnRender(uint32_t id)
	{
		(void)id;
	}

	void BasePipeline::RemoveFromPipeline(uint32_t id)
	{
		(void)id;
	}
} // namespace Soon