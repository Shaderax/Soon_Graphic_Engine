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

	bool IsRuntimeUniform(SpvReflectDescriptorBinding* binding)
	{
		for (uint32_t indexMember = 0; indexMember < binding->block.member_count; indexMember++)
		{
			if (binding->block.members[indexMember].type_description->op == SpvOpTypeRuntimeArray)
				return true;
		}
		return false;
	}

	void BasePipeline::GetInputBindings( spv_reflect::ShaderModule& reflection )
	{
	}

	void BasePipeline::GetUniform(SpvReflectDescriptorBinding* binding)
	{
		Uniform uniform;
		uniform._name = binding->name;
		uniform._binding = binding->binding;
		uniform._set = binding->set;
		uniform._size = binding->block.size + (binding->block.size % 32);

		std::cout << "Uniform Name : " << uniform._name << ", Set : " << uniform._set << ", Binding: " << uniform._binding << std::endl;

		uniform.dimCount = binding->type_description->traits.array.dims_count;

		if (uniform.dimCount > 0)
			uniform.dim = new uint32_t[uniform.dimCount];

		for (uint32_t index = 0 ; index < binding->type_description->traits.array.dims_count ; index++)
		{
			uniform.dim[index] = binding->type_description->traits.array.dims[index];
			std::cout << "Dim: " << binding->type_description->traits.array.dims[index] << std::endl;
		}

		std::cout << "Op: " << binding->type_description->op << std::endl;

		for (uint32_t indexMember = 0; indexMember < binding->block.member_count; indexMember++)
		{
			UniformVar uniVar;
			uniVar.name = binding->block.members[indexMember].name;
			uniVar.offset = binding->block.members[indexMember].offset;
			uniVar.size = binding->block.members[indexMember].size;

			std::cout << "\t UniformVar Name : " << uniVar.name << std::endl;
			std::cout << "\t UniformVar Offset : " << uniVar.offset << std::endl;
			std::cout << "\t UniformVar Size : " << uniVar.size << std::endl;
			std::cout << "\t UniformVar SpvOp: " << binding->block.members[indexMember].type_description->op << std::endl;

			if (binding->block.members[indexMember].type_description->traits.array.dims_count > 0)
				std::cout << "\t UniformVar Dim: " << binding->block.members[indexMember].type_description->traits.array.dims[0] << std::endl;

			uniVar.type = SpvTypeToVertexType(binding->block.members[indexMember].type_description);

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
		if (_conf->IsUniqueSet(binding->set))
			_mUbm.AddUniqueUniform(uniform);
		else
			_mUbm.AddUniform(uniform);
	}

	void BasePipeline::GetTextureUniform(SpvReflectDescriptorBinding* binding)
	{
		UniformTexture texture;
		texture._binding = binding->binding;
		texture._set = binding->set;
		texture._name = binding->name;
		texture._type = SpvTypeToVertexType(binding->type_description);

		std::cout << "UniformTexture Name: " << texture._name << std::endl;
		std::cout << "UniformTexture Op: " << binding->type_description->op << std::endl;

		texture.dimCount = binding->type_description->traits.array.dims_count;
		if (texture.dimCount > 0)
			texture.dim = new uint32_t[texture.dimCount];
		for (uint32_t index = 0 ; index < binding->type_description->traits.array.dims_count ; index++)
		{
			texture.dim[index] = binding->type_description->traits.array.dims[index];
			std::cout << "Dim: " << binding->type_description->traits.array.dims[index] << std::endl;
		}

		int32_t indexDefault = IsDefaultUniform(binding->name);
		//texture.isTexture = true;
		// TODO: Unique
		texture._updateFunct = nullptr;
		if (_conf->IsUniqueSet(binding->set))
			_mUbm.AddUniqueUniform(texture);
		else
			_mUbm.AddUniform(texture);
		//_uniformsTexture.push_back(texture);
	}

	void BasePipeline::GetRuntimeUniform(SpvReflectDescriptorBinding* binding)
	{
		UniformRuntime uniform;
		uniform._name = binding->name;
		uniform._binding = binding->binding;
		uniform._set = binding->set;
		uniform._size = binding->block.size + (binding->block.size % 32);

		std::cout << "UniformRuntime Name : " << uniform._name << ", Set : " << uniform._set << ", Binding: " << uniform._binding << std::endl;

		//TODO: DIM MEMBER

		uniform.dimCount = binding->type_description->traits.array.dims_count;

		if (uniform.dimCount > 0)
			uniform.dim = new uint32_t[uniform.dimCount];

		for (uint32_t index = 0 ; index < binding->type_description->traits.array.dims_count ; index++)
		{
			uniform.dim[index] = binding->type_description->traits.array.dims[index];
			std::cout << "UniformRuntime Dim: " << binding->type_description->traits.array.dims[index] << std::endl;
		}

		std::cout << "UniformRuntime Op: " << binding->type_description->op << std::endl;

		for (uint32_t indexMember = 0; indexMember < binding->block.member_count; indexMember++)
		{
			UniformRuntimeVar uniVar;
			uniVar._name = binding->block.members[indexMember].name;
			uniVar._offset = binding->block.members[indexMember].offset;
			uniVar._size = binding->block.members[indexMember].size;

			std::cout << "\t UniformRuntimeVar Name : " << uniVar._name << std::endl;
			std::cout << "\t UniformRuntimeVar Offset : " << uniVar._offset << std::endl;
			std::cout << "\t UniformRuntimeVar Size : " << uniVar._size << std::endl;
			std::cout << "\t UniformRuntimeVar SpvOp: " << binding->block.members[indexMember].type_description->op << std::endl;

			uniVar.dimCount = binding->block.members[indexMember].type_description->traits.array.dims_count;

			if (uniVar.dimCount > 0)
				uniVar.dim = new uint32_t[uniVar.dimCount];

			for (uint32_t index = 0 ; index < uniVar.dimCount ; index++)
			{
				uniVar.dim[index] = binding->block.members[indexMember].type_description->traits.array.dims[index];
				std::cout << "UniformRuntime Dim: " << uniVar.dim[index] << std::endl;
			}

			uniVar._type = SpvTypeToVertexType(binding->block.members[indexMember].type_description);

			if (binding->block.members[indexMember].type_description->op == SpvOpTypeRuntimeArray)
				uniVar.isRuntime = true;
			else
				uniVar.isRuntime = false;

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
		if (_conf->IsUniqueSet(binding->set))
			_mUbm.AddUniqueUniform(uniform);
		else
			_mUbm.AddUniform(uniform);
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
				GetTextureUniform(bindings[index]);
			else if (IsRuntimeUniform(bindings[index]))
				GetRuntimeUniform(bindings[index]);
			else
				GetUniform(bindings[index]);

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