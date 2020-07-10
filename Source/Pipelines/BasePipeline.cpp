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

		delete _conf;

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

	void BasePipeline::SetRuntimeAmount(std::string name, uint32_t amount, uint32_t idMat)
	{
		_mUbm.SetRuntimeAmount(name, amount, idMat);
	}

	const UniformRuntime& BasePipeline::GetUniformRuntime(std::string name) const
	{
		return _mUbm.GetUniformRuntime(name);
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

	UniformVar BasePipeline::ParseUniformMembers(SpvReflectBlockVariable& block) const
	{
		UniformVar uniVar;
		uniVar.name = block.name;
		uniVar.offset = block.offset;
		uniVar.size = block.size;

		std::cout << "\t UniformVar Name : " << uniVar.name << std::endl;
		std::cout << "\t UniformVar Offset : " << uniVar.offset << std::endl;
		std::cout << "\t UniformVar Size : " << uniVar.size << std::endl;
		std::cout << "\t UniformVar SpvOp: " << block.type_description->op << std::endl;

		if (block.type_description->traits.array.dims_count > 0)
			std::cout << "\t UniformVar Dim: " << block.type_description->traits.array.dims[0] << std::endl;

		uniVar.dimCount = block.type_description->traits.array.dims_count;
		std::cout << "\t UniformVar DimCount: " << uniVar.dimCount << std::endl;

		if (uniVar.dimCount > 0)
			uniVar.dim = new uint32_t[uniVar.dimCount];

		for (uint32_t index = 0 ; index < uniVar.dimCount ; index++)
		{
			uniVar.dim[index] = block.type_description->traits.array.dims[index];
			std::cout << "\t UniformVar Dim: " << uniVar.dim[index] << std::endl;
		}

		uniVar.type = SpvTypeToVertexType(block.type_description);

		for (uint32_t indexMember = 0; indexMember < block.member_count; indexMember++)
			uniVar.mMembers.push_back(ParseUniformMembers(block.members[indexMember]));
		return uniVar;
	}

	void BasePipeline::ParseUniform(SpvReflectDescriptorBinding* binding)
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
			uniform._members.push_back(ParseUniformMembers(binding->block.members[indexMember]));
		}
				/*
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

	void BasePipeline::ParseTextureUniform(SpvReflectDescriptorBinding* binding)
	{
		UniformTexture texture;
		texture._binding = binding->binding;
		texture._set = binding->set;
		texture._name = binding->name;
		texture._type = SpvTypeToVertexType(binding->type_description);

		std::cout << std::endl << "UniformTexture Name: " << texture._name << std::endl;
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

		texture._updateFunct = nullptr;
		if (_conf->IsUniqueSet(binding->set))
			_mUbm.AddUniqueUniform(texture);
		else
			_mUbm.AddUniform(texture);
		//_uniformsTexture.push_back(texture);
	}

	UniformRuntimeVar BasePipeline::ParseRuntimeUniformMembers(SpvReflectBlockVariable& block) const
	{
		UniformRuntimeVar uniVar;
		uniVar._name = block.name;
		uniVar._offset = block.offset;
		uniVar._size = block.size;

		std::cout << "\t UniformRuntimeVar Name : " << uniVar._name << std::endl;
		std::cout << "\t UniformRuntimeVar Offset : " << uniVar._offset << std::endl;
		std::cout << "\t UniformRuntimeVar Size : " << uniVar._size << std::endl;
		std::cout << "\t UniformRuntimeVar SpvOp: " << block.type_description->op << std::endl;

		uniVar.dimCount = block.type_description->traits.array.dims_count;
		std::cout << "\t UniformRuntimeVar DimCount: " << uniVar.dimCount << std::endl;

		if (uniVar.dimCount > 0)
			uniVar.dim = new uint32_t[uniVar.dimCount];

		for (uint32_t index = 0 ; index < uniVar.dimCount ; index++)
		{
			uniVar.dim[index] = block.type_description->traits.array.dims[index];
			std::cout << "\t UniformRuntime Dim: " << uniVar.dim[index] << std::endl;
		}

		uniVar._type = SpvTypeToVertexType(block.type_description);

		if (block.type_description->op == SpvOpTypeRuntimeArray)
			uniVar.isRuntime = true;
		else
			uniVar.isRuntime = false;

		for (uint32_t indexMember = 0; indexMember < block.member_count; indexMember++)
			uniVar.mMembers.push_back(ParseRuntimeUniformMembers(block.members[indexMember]));
		return uniVar;
	}

	void BasePipeline::ParseRuntimeUniform(SpvReflectDescriptorBinding* binding)
	{
		UniformRuntime uniform;
		uniform.mName = binding->name;
		if (uniform.mName.empty())
			uniform.mName = binding->type_description->type_name;
		uniform.mBinding = binding->binding;
		uniform.mSet = binding->set;
		uniform.mSize = binding->block.size + (binding->block.size % 32);

		std::cout << "UniformRuntime Name : " << uniform.mName << ", Set : " << uniform.mSet << ", Binding: " << uniform.mBinding << std::endl;
		std::cout << "UniformRuntime Size : " << uniform.mSize << std::endl;

		uniform.mDimCount = binding->type_description->traits.array.dims_count;

		if (uniform.mDimCount > 0)
			uniform.mDims = new uint32_t[uniform.mDimCount];

		for (uint32_t index = 0 ; index < binding->type_description->traits.array.dims_count ; index++)
		{
			uniform.mDims[index] = binding->type_description->traits.array.dims[index];
			std::cout << "UniformRuntime Dim: " << binding->type_description->traits.array.dims[index] << std::endl;
		}

		std::cout << "UniformRuntime Op: " << binding->type_description->op << std::endl;

		for (uint32_t indexMember = 0; indexMember < binding->block.member_count; indexMember++)
			uniform.mMembers.push_back(ParseRuntimeUniformMembers(binding->block.members[indexMember]));

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
				ParseTextureUniform(bindings[index]);
			else if (IsRuntimeUniform(bindings[index]))
				ParseRuntimeUniform(bindings[index]);
			else
				ParseUniform(bindings[index]);

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