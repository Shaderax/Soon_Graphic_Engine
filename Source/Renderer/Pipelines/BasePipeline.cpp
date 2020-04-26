#include "Renderer/Pipelines/BasePipeline.hpp"
//#include "Renderer/Mesh.hpp"

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
    case (SpvOpTypeMatrix):
        ve.row = type->traits.numeric.matrix.row_count;
        ve.column = type->traits.numeric.matrix.column_count;
    default:
        ve.row = 1;
        ve.column = 1;
        break;
    }
    return (ve);
}

bool IsImageType(SpvReflectTypeFlags type)
{
    return ((type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLER || type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_IMAGE || type & SPV_REFLECT_TYPE_FLAG_EXTERNAL_SAMPLED_IMAGE));
}

BasePipeline::BasePipeline(void)
{
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

            _inputs.AddVertexElement(input);
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

            int32_t index = IsDefaultUniform(bindings[index]->name);
            if (index >= 0 && IsValidDefaultUniform(bindings[index], index))
            {
                texture.isUnique = _defaultUniform[index].isUnique;
                texture._updateFunct = _defaultUniform[index].updateFunc;
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
    }
}

void BasePipeline::GetBindingDescription()
{
    bindingDescription.binding = 0;
    bindingDescription.stride = _inputs.GetVertexStrideSize(); // stride : size of one pointe
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void BasePipeline::GetAttributeDescriptions(void)
{
    // Todo Location
    _attributeDescriptions.resize(_inputs.GetNumElement());

    for (uint32_t index = 0; index < _attributeDescriptions.size(); index++)
    {
        _attributeDescriptions[index].binding = 0;
        _attributeDescriptions[index].location = 0; // TODO Location
        _attributeDescriptions[index].format = VertexTypeToVkFormat(_inputs.data[index].type);
        _attributeDescriptions[index].offset = _inputs.GetElementOffset(index); // TODO Opti
    }
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
} // namespace Soon