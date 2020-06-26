#include "Pipelines/BasePipeline.hpp"
#include "GraphicsRenderer.hpp"
#include "UniformsBufferManager.hpp"

namespace Soon
{
	/**
	 * CONSTRUCTOR/DESTRUCTOR
	 */
	BasePipeline::BasePipeline(PipelineConf* conf) : _conf(conf)
	{
	}

	BasePipeline::~BasePipeline( void )
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		vkDestroyPipeline(device, _pipeline, nullptr);
		vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

		DestroyAllUniforms();
	}

	/**
	 * Uniforms
	 */
	void BasePipeline::DestroyAllUniforms(void)
	{
		_mUbm.DestroyAllUniforms();
	}

	void BasePipeline::RecreateUniforms(void)
	{
		_mUbm.RecreateUniforms(m_ToDraw);
	}

	bool BasePipeline::SetDefaultUniform(DefaultUniformStruct structure)
	{
		// Check if already exist
		_defaultUniform.push_back(structure);

		return (true);
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
	/**
	 * PIPELINES
	 */
	void BasePipeline::DestroyGraphicPipeline(void)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

		vkDestroyPipeline(device, _pipeline, nullptr);
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

	void* BasePipeline::Get(std::string name, uint32_t id)
	{
		return _mUbm.Get(name, id);
	}

	void BasePipeline::Set(std::string name, void *value, uint32_t id)
	{
		_mUbm.Set( name, value, id );
	}

	/**
	 * TEXTURE
	 */
	void BasePipeline::SetTexture(std::string name, uint32_t idMat, uint32_t textureId)
	{
		_mUbm.SetTexture(name, idMat, textureId);
	}

	/**
	 * GET SHADER DATA
	 */	
	void BasePipeline::GetInputBindings( spv_reflect::ShaderModule& reflection )
	{
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

				std::cout << "UniformTexture Name : " << texture._name << std::endl;

				int32_t indexDefault = IsDefaultUniform(bindings[index]->name);
				//texture.isTexture = true;
				// TODO: Unique
				texture._updateFunct = nullptr;
				if (_conf->IsUniqueSet(bindings[index]->set))
					_mUbm.AddUniqueUniform(texture);
				else
					_mUbm.AddUniform(texture);
				//_uniformsTexture.push_back(texture);
			}
			else
			{
				Uniform uniform;
				uniform._name = bindings[index]->name;
				uniform._binding = bindings[index]->binding;
				uniform._set = bindings[index]->set;
				uniform._size = bindings[index]->block.size + (bindings[index]->block.size % 32);

				std::cout << "Uniform Name : " << uniform._name << ", Set : " << uniform._set << ", Binding: " << uniform._binding << std::endl;
				if (bindings[index]->type_description->traits.array.dims_count > 0)
					std::cout << "Dim: " << bindings[index]->type_description->traits.array.dims[0] << std::endl;
				std::cout << "Op: " << bindings[index]->type_description->op << std::endl;

				for (uint32_t indexMember = 0; indexMember < bindings[index]->block.member_count; indexMember++)
				{
					UniformVar uniVar;
					uniVar._name = bindings[index]->block.members[indexMember].name;
					uniVar._offset = bindings[index]->block.members[indexMember].offset;
					uniVar._size = bindings[index]->block.members[indexMember].size;

					std::cout << "\t UniformVar Name : " << uniVar._name << std::endl;
					std::cout << "\t UniformVar Offset : " << uniVar._offset << std::endl;
					std::cout << "\t UniformVar Size : " << uniVar._size << std::endl;
					std::cout << "\t UniformVar SpvOp: " << bindings[index]->block.members[indexMember].type_description->op << std::endl;

					if (bindings[index]->block.members[indexMember].type_description->traits.array.dims_count > 0)
						std::cout << "Dim: " << bindings[index]->block.members[indexMember].type_description->traits.array.dims[0] << std::endl;

					uniVar._type = SpvTypeToVertexType(bindings[index]->block.members[indexMember].type_description);

					uniform._members.push_back(uniVar);
				}
				/*
				Comment je fais pour coller une fonction d'update a mes uniform ?
				Pour que ma camera soit update auto
				Genre mes models sont par id, comment le mec fait pour dire la pos pour le model ? Faudrait que la mesh connaisse son entity owner et nous passe son id.
				void funct( void )
				{
					mat4 mat .... operation {entity.GetComponent<Transform>()};
					memcpy(um, mat);
				}
				Mesh.GetMaterial().GetPipeline()->SetUpdateFunction("um", &funct);
				*/
				if (_conf->IsUniqueSet(bindings[index]->set))
					_mUbm.AddUniqueUniform(uniform);
				else
					_mUbm.AddUniform(uniform);
			}
			// uboLayout
			VkDescriptorSetLayoutBinding ubo;
			ubo.binding = bindings[index]->binding;
			ubo.descriptorCount = (bindings[index]->array.dims_count != 0) ? bindings[index]->array.dims[0] : 1;
			ubo.descriptorType = DescriptorTypeSpvToVk(bindings[index]->descriptor_type); //VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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