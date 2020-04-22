#include <iostream>
#include <fstream>
#include <assert.h>
#include "SPIRV-Reflect/spirv_reflect.h"

static std::vector<char> ReadFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!" + filename);
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize / sizeof(char));

    file.seekg(0);
    file.read(reinterpret_cast<char *>(buffer.data()), fileSize);

    file.close();

    return buffer;
}

int main()
{
    //std::vector<char> spirvFile = ReadFile("./SPV_Reader/Ressources/Shaders/DefaultShader.vert.spv");
    std::vector<char> spirvFile = ReadFile("./SPV_Reader/Ressources/Shaders/DefaultShader.frag.spv");

    spv_reflect::ShaderModule reflection(spirvFile.size(), spirvFile.data());
    if (reflection.GetResult() != SPV_REFLECT_RESULT_SUCCESS)
    {
        std::cerr << "ERROR: could not process '"
                  << "./SPV_Reader/Ressources/Shaders/DefaultShader.vert.spv"
                  << "' (is it a valid SPIR-V bytecode?)" << std::endl;
        return EXIT_FAILURE;
    }

    SpvReflectResult result = SPV_REFLECT_RESULT_NOT_READY;
    uint32_t count = 0;
    std::vector<SpvReflectDescriptorBinding *> bindings;
    std::vector<SpvReflectInterfaceVariable *> inputs;

    /// Descriptor Binding ///
    result = reflection.EnumerateDescriptorBindings(&count, nullptr);

    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    bindings.resize(count);
    reflection.EnumerateDescriptorBindings(&count, bindings.data());

    for (uint32_t index = 0 ; index < count ; index++)
    {
        std::cout << "bindin name : " <<bindings[index]->name << std::endl;
        std::cout << "type flag : " << bindings[index]->type_description->type_flags << std::endl;
        std::cout << "size : " << bindings[index]->block.size << std::endl;

        if (bindings[index]->type_description->struct_member_name)
            std::cout << bindings[index]->type_description->struct_member_name << std::endl;
        if (bindings[index]->type_description->type_name)
            std::cout << "type name : " << bindings[index]->type_description->type_name << std::endl;
        std::cout << "binding : " << bindings[index]->binding << std::endl;
        std::cout << "set : " << bindings[index]->set << std::endl;

        for (uint32_t index2 = 0 ; index2 < bindings[index]->type_description->member_count ; index2++)
        {
            std::cout << "struct member name : " << bindings[index]->type_description->members[index2].struct_member_name << std::endl;

            switch (bindings[index]->type_description->members[index2].op)
            {
                case (SpvOpTypeVector):
                    std::cout << "vector :" << std::endl;
                    std::cout << bindings[index]->type_description->members[index2].traits.numeric.scalar.width << std::endl;
                    std::cout << bindings[index]->type_description->members[index2].traits.numeric.vector.component_count << std::endl;
                case (SpvOpTypeMatrix):
                    std::cout << "matrix :" << std::endl;
                    std::cout << "col :" << bindings[index]->type_description->members[index2].traits.numeric.matrix.column_count << std::endl;
                    std::cout << "row :" << bindings[index]->type_description->members[index2].traits.numeric.matrix.row_count << std::endl;
                    std::cout << "format : " << ((bindings[index]->type_description->members[index2].type_flags & SpvReflectTypeFlagBits::SPV_REFLECT_TYPE_FLAG_FLOAT) ? "float" : "idk") << std::endl;
                    std::cout << "signed : " << bindings[index]->type_description->members[index2].traits.numeric.scalar.signedness << std::endl;
                default:
                    break;
            }
            std::cout << std::endl;
        }
    }

    // Input
    result = reflection.EnumerateInputVariables(&count, nullptr);

    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    inputs.resize(count);
    reflection.EnumerateInputVariables(&count, inputs.data());

    for (uint32_t index = 0 ; index < count ; index++)
    {
        std::cout << inputs[index]->name << std::endl;
        std::cout << inputs[index]->type_description->type_flags << std::endl;
        std::cout << inputs[index]->type_description->traits.numeric.vector.component_count << std::endl;
        std::cout << inputs[index]->format << std::endl;
    }
    return (0);
}
