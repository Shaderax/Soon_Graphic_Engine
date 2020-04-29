#include "Renderer/Pipelines/BasePipeline.hpp"
#include "Renderer/Vulkan/GraphicsRenderer.hpp"
namespace Soon
{
VertexElementType SpvTypeToVertexType(SpvReflectTypeDescription *type)
{
    VertexElementType ve;

    if (type->type_flags & SPV_REFLECT_TYPE_FLAG_VOID)
        ve.baseType = EnumVertexElementType::VOID;
    else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_BOOL)
        ve.baseType = EnumVertexElementType::BOOLEAN;
    else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_INT)
    {
        if (type->traits.numeric.scalar.signedness == 1)
            ve.baseType = EnumVertexElementType::INT;
        else
            ve.baseType = EnumVertexElementType::UINT;
    }
    else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT)
    {
        if (type->traits.numeric.scalar.width == 32)
            ve.baseType = EnumVertexElementType::FLOAT;
        else
            ve.baseType = EnumVertexElementType::DOUBLE;
    }
    else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE)
        ve.baseType = EnumVertexElementType::IMAGE;
    else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER)
        ve.baseType = EnumVertexElementType::SAMPLER;
    else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE)
        ve.baseType = EnumVertexElementType::SAMPLEDIMAGE;
    else if (type->type_flags & SPV_REFLECT_TYPE_FLAG_STRUCT)
        ve.baseType = EnumVertexElementType::STRUCT;

    switch (type->op)
    {
    case (SpvOpTypeVector):
        ve.row = 1;
        ve.column = type->traits.numeric.vector.component_count;
        break ;
    case (SpvOpTypeMatrix):
        ve.row = type->traits.numeric.matrix.row_count;
        ve.column = type->traits.numeric.matrix.column_count;
        break ;
    default:
        ve.row = 1;
        ve.column = 1;
        break ;
    }
    return (ve);
}

VkDescriptorType DescriptorTypeSpvToVk(SpvReflectDescriptorType type)
{
    switch (type)
    {
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
        return (VK_DESCRIPTOR_TYPE_SAMPLER);
    case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        return (VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
    case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
        return (VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        return (VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
        return (VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER);
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
        return (VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER);
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
        return (VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
    case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
        return (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC);
    case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
        return (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);
    default:
        return (VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM);
    }
    return (VkDescriptorType::VK_DESCRIPTOR_TYPE_MAX_ENUM);
}

VkShaderStageFlags SpvStageToVulkanStage(SpvReflectShaderStageFlagBits stage)
{
    switch (stage)
    {
    case SPV_REFLECT_SHADER_STAGE_VERTEX_BIT:
        return (VK_SHADER_STAGE_VERTEX_BIT);
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return (VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    case SPV_REFLECT_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        return (VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
    case SPV_REFLECT_SHADER_STAGE_GEOMETRY_BIT:
        return (VK_SHADER_STAGE_GEOMETRY_BIT);
    case SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT:
        return (VK_SHADER_STAGE_FRAGMENT_BIT);
    case SPV_REFLECT_SHADER_STAGE_COMPUTE_BIT:
        return (VK_SHADER_STAGE_COMPUTE_BIT);
    default:
        break;
    }
    return (0);
}
VkFormat VertexTypeToVkFormat(VertexElementType vt)
{
    switch (vt.baseType)
    {
    case EnumVertexElementType::INT:
        switch (vt.column)
        {
        case 1:
            return (VK_FORMAT_R32_SINT);
        case 2:
            return (VK_FORMAT_R32G32_SINT);
        case 3:
            return (VK_FORMAT_R32G32B32_SINT);
        case 4:
            return (VK_FORMAT_R32G32B32A32_SINT);
        default:
            return (VK_FORMAT_UNDEFINED);
        }
    case EnumVertexElementType::UINT:
        switch (vt.column)
        {
        case 1:
            return (VK_FORMAT_R32_UINT);
        case 2:
            return (VK_FORMAT_R32G32_UINT);
        case 3:
            return (VK_FORMAT_R32G32B32_UINT);
        case 4:
            return (VK_FORMAT_R32G32B32A32_UINT);
        default:
            return (VK_FORMAT_UNDEFINED);
        }
    case EnumVertexElementType::FLOAT:
        switch (vt.column)
        {
        case 1:
            return (VK_FORMAT_R32_SFLOAT);
        case 2:
            return (VK_FORMAT_R32G32_SFLOAT);
        case 3:
            return (VK_FORMAT_R32G32B32_SFLOAT);
        case 4:
            return (VK_FORMAT_R32G32B32A32_SFLOAT);
        default:
            return (VK_FORMAT_UNDEFINED);
        }
    case EnumVertexElementType::DOUBLE:
        switch (vt.column)
        {
        case 1:
            return (VK_FORMAT_R64_SFLOAT);
        case 2:
            return (VK_FORMAT_R64G64_SFLOAT);
        case 3:
            return (VK_FORMAT_R64G64B64_SFLOAT);
        case 4:
            return (VK_FORMAT_R64G64B64A64_SFLOAT);
        default:
            return (VK_FORMAT_UNDEFINED);
        }
    default:
        return (VK_FORMAT_UNDEFINED);
    }
    return (VK_FORMAT_UNDEFINED);
}

bool IsImageType(SpvReflectTypeFlags type)
{
    return ((type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER || type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE || type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE));
}

BasePipeline::BasePipeline(void)
{
}

BasePipeline::~BasePipeline(void)
{

    VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

    for (VkDescriptorSetLayout &dsl : _descriptorSetLayout)
        vkDestroyDescriptorSetLayout(device, dsl, nullptr);

    vkDestroyPipeline(device, _graphicPipeline, nullptr);
    vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);
}

void BasePipeline::UpdateData(int currentImg)
{
    /*
		//for (UniformTexture& uniform : _uniformsTexture)
		//{
		//}
		for (uint32_t index = 0; index < _uniforms.size(); index++)
		{
			if (_uniforms[index]._updateFunct != nullptr)
			{
				for (UniformSets &set : _uniforms[index]._us)
					_uniforms[index]._updateFunct(set._uniformRender._BufferMemory[currentImg]);
			}
			else
			{
				AutoUpdate(_uniforms[index], _materials[index]);
			}
		}
		//for (UniformTexture& uniform : _uniqueUniformsTexture)
		//{
		//}
		*/
}

int32_t BasePipeline::IsDefaultVertexInput(std::string name)
{
    for (int32_t index = 0; index < DefaultVertexInput.size(); index++)
    {
        if (DefaultVertexInput[index].inputName == name)
            return (index);
    }
    return (-1);
}

int32_t BasePipeline::IsDefaultUniform(std::string name)
{
    for (int32_t index = 0; index < _defaultUniform.size(); index++)
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

    for (uint32_t indexMember = 0; index < block->type_description->member_count; indexMember++)
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
            if (indexDefault >= 0 && IsValidDefaultUniform(bindings[index], indexDefault))
            {
                texture.isUnique = _defaultUniform[indexDefault].isUnique;
                texture._updateFunct = _defaultUniform[indexDefault].updateFunc;
            }
            else
            {
                texture.isUnique = false;
                texture._updateFunct = nullptr;
            }
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
        // uboLayout ?
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

void BasePipeline::GetBindingDescription()
{
    _bindingDescription.binding = 0;
    _bindingDescription.stride = _vertexDescription.GetVertexStrideSize(); // stride : size of one pointe
    _bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void BasePipeline::GetAttributeDescriptions(void)
{
    // Todo Location
    _attributeDescriptions.resize(_vertexDescription.GetNumElement());

    for (uint32_t index = 0; index < _attributeDescriptions.size(); index++)
    {
        _attributeDescriptions[index].binding = 0;
        _attributeDescriptions[index].location = index; // TODO Location
        _attributeDescriptions[index].format = VertexTypeToVkFormat(_vertexDescription.data[index].type);
        _attributeDescriptions[index].offset = _vertexDescription.GetElementOffset(index); // TODO Opti
    }
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
        // Aller cherche _unifroms[index] set et binding a envoyé en dessus pour qu'il fasse 
        //ds._descriptorSets = GraphicsInstance::GetInstance()->CreateDescriptorSets( _uniforms[index]._size, layoutArray, dlayout, ds._uniformRender.buffer.data(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );
        //vkDestroyBuffer(ds._uniformRender.buffer[0]);
    }

    for (uint32_t index = 0; index < _uniformsTexture.size(); index++)
    {
    }
    return 0;
}

void BasePipeline::BindCaller(VkCommandBuffer commandBuffer, uint32_t index)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicPipeline);

    VkDeviceSize offsets[] = {0};

    for (uint32_t index = 0; index < _toDraw.size(); index++)
    {
        std::cout << "DRAW !" << std::endl;
        MeshBufferRenderer &bu = GraphicsRenderer::GetInstance()->GetMesh(_toDraw[index].meshId);
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(bu.vertex.buffer), offsets);

        vkCmdBindIndexBuffer(commandBuffer, bu.indices.buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(commandBuffer, 3, 1, 0, 0, 0);
    }
}

VertexDescription BasePipeline::GetVertexDescription( )
{
    return (_vertexDescription);
}
} // namespace Soon