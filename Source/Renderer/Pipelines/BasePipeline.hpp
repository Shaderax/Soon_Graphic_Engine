#pragma once

#include "Renderer/Vulkan/GraphicsInstance.hpp"

#include "Utilities/ShadersUniform.hpp"
#include "Utilities/Vertex.hpp"
#include "Utilities/ReadFile.hpp"

#include "Modules/SPIRV-Reflect/spirv_reflect.h"
#include "Modules/Math/mat4.hpp"

#include <vector>
#include <functional>

namespace Soon
{
enum struct PipelineType : uint32_t
{
	GRAPHIC = 0,
	COMPUTE = 1
};

VertexElementType SpvTypeToVertexType(SpvReflectTypeDescription *type);

struct UniformVar
{
	std::string _name;
	VertexElementType _type;
	uint32_t _size;
	uint32_t _offset;
};

struct Uniform
{
	std::string _name;
	uint32_t _size;
	uint32_t _binding;
	uint32_t _set;
	std::vector<UniformVar> _members;
	std::vector<UniformSets> _us;
	std::function<void(int)> _updateFunct;
	bool isUnique;
};

struct UniformTexture
{
	UniformVar _data;
	uint32_t _binding;
	uint32_t _set;
	std::vector<Image> _i;
	std::vector<ImageRenderer> _iR;
	std::vector<VkDescriptorSet> _dS;
	std::function<void(int)> _updateFunct;
	bool isUnique;
};

struct DefaultMemberStruct
{
	std::string name;
	VertexElementType tt;
};

struct DefaultUniformStruct
{
	std::string name;
	std::string typeName;
	VertexElementType tt;
	bool isUnique;
	std::vector<DefaultMemberStruct> members;
	std::function<void(int)> updateFunc;
};

/*
void UpdateCamera(VkDeviceMemory deviceMemory) // VkDeviceMemory
{
	void *data = nullptr;
	VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

	UniformCamera uc = {};

	if (Engine::GetInstance().GetCurrentScene() && Engine::GetInstance().GetCurrentScene()->GetCurrentCamera())
	{
		uc.view = Engine::GetInstance().GetCurrentScene()->GetCurrentCamera()->GetViewMatrix();
		uc.proj = Engine::GetInstance().GetCurrentScene()->GetCurrentCamera()->GetProjectionMatrix();
	}
	else
	{
		std::cout << "No Current Camera." << std::endl;
		uc.view = mat4<float>();
		uc.proj = mat4<float>();
	}

	//std::vector<VkDeviceMemory> vkdm = _uniformCamera._uniformRender._BufferMemory;
	vkMapMemory(device, deviceMemory, 0, sizeof(UniformCamera), 0, &data);
	memcpy(data, &uc, sizeof(UniformCamera));
	vkUnmapMemory(device, deviceMemory);
	}
*/

bool IsImageType(SpvReflectTypeFlags type);

/*
void UpdateModel(VkDeviceMemory deviceMemory, Transform3D *transform)
{
	void *data = nullptr;
	VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

	//int i = -1;
	//for (auto& uniform : _uniformsBuffers)
	//{
	//	++i;

	vkMapMemory(device, deviceMemory, 0, sizeof(UniformModel), 0, &data);

	mat4<float> matModel;

	Transform3D *transform; // = _transforms.at(i);

	matModel = transform->_rot.GetRotationMatrix();

	matModel(0, 3) = transform->_pos.x;
	matModel(1, 3) = transform->_pos.y;
	matModel(2, 3) = transform->_pos.z;

	matModel(0, 0) *= transform->_scale.x;
	matModel(1, 1) *= transform->_scale.y;
	matModel(2, 2) *= transform->_scale.z;

	memcpy(data, &matModel, sizeof(UniformModel));
	vkUnmapMemory(device, deviceMemory);
}
*/
/*
void AutoUpdate(Uniform &uniform)
{
	//for (material->updated)
	//	for (uint32_t index = 0; index < uniform._members)
}
*/

struct IdRender
{
	uint32_t matId;
	uint32_t meshId;
};

class BasePipeline
{
	protected:
	std::vector<VkDescriptorSetLayout> _descriptorSetLayout;
	VkPipelineLayout _pipelineLayout;
	VkPipeline _graphicPipeline;
	std::vector<std::vector<VkDescriptorSetLayoutBinding>> uboLayoutBinding;
private:
	std::vector<IdRender> _toDraw;
	std::vector<uint32_t> _freeId;
	VertexDescription _vertexDescription;

	std::vector<UniformTexture> _uniformsTexture; // uniformSampler2D
	std::vector<Uniform> _uniforms;				  // uniformModel, uniformLight

public:
	GraphicsPipelineConf		_conf;

	BasePipeline(void);

	virtual void Init() = 0;
	~BasePipeline();
	//std::vector<VkDescriptorSetLayoutBinding> GetLayoutBinding(void);
	//void UpdateData(int currentImg);
	void BindCaller(VkCommandBuffer commandBuffer, uint32_t index);
	std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;
	void GetAttributeDescriptions( void );
	VkVertexInputBindingDescription _bindingDescription;
	void GetBindingDescription( void );
	virtual void RecreateUniforms(void) {};
	virtual void RecreatePipeline(void) {};

	virtual void Render(uint32_t id) {};
	virtual void UnRender(uint32_t id) {};

	uint32_t AddToPipeline(std::uint32_t meshId);
	virtual void RemoveFromPipeline(uint32_t id) {};

	VertexDescription GetVertexDescription( );

	std::vector<DefaultUniformStruct> _defaultUniform{
		/*
	{"uc", "UniformCamera", {EnumVertexElementType::STRUCT}, true, true, {{"view", "", {EnumVertexElementType::FLOAT, 4, 4}}, {"proj", "", {EnumVertexElementType::FLOAT, 4, 4}}}, &UpdateCamera},
	{"um", "UniformModel", {EnumVertexElementType::STRUCT}, false, true, {{"model", "", {EnumVertexElementType::FLOAT, 4, 4}}}, &UpdateModel}
	*/
	};

/*
	template <typename T>
	void Set(std::string name, VertexElementType type, T value, uint32_t id)
	{
		VkDevice device = GraphicsInstance::GetInstance()->GetDevice();
		uint32_t currentImage = GraphicsInstance::GetInstance()->GetNextIdImageToRender();
		void *data = nullptr;

		int32_t pos = name.find(".");
		if (pos == std::string::npos)
		{
			for (uint32_t index = 0; index < _uniforms.size(); index++)
			{
				if (_uniforms[index]._name == name)
				{
					vkMapMemory(device, _uniforms[index]._us[id]._uniformRender._BufferMemory[currentImage], 0, _uniforms[index]._size, 0, &data);
					memcpy(data, &value, _uniforms[index]._size);
					vkUnmapMemory(device, _uniforms[index]._us[id]._uniformRender._BufferMemory[currentImage]);
					return;
				}
			}
		}
		else
		{
			std::string varName;

			for (uint32_t index = 0; index < _uniforms.size(); index++)
			{
				if (_uniforms[index]._name == name)
				{
					for (uint32_t member = 0; member < _uniforms[index]._members.size(); member++)
					{
						if (varName == _uniforms[index]._members[member]._name)
						{
							if (type != _uniforms[index]._members[member]._type) {}
							//return; // error
							// Memcpy
							vkMapMemory(device, _uniforms[index]._us[id]._uniformRender._BufferMemory[currentImage], 0, _uniforms[index]._members[member]._size, 0, &data);
							memcpy(data, &value, _uniforms[index]._members[member]._size);
							vkUnmapMemory(device, _uniforms[index]._us[id]._uniformRender._BufferMemory[currentImage]);
						}
					}
					return;
				}
			}
		}
	}*/

	void UpdateData(int currentImg);

	int32_t IsDefaultVertexInput(std::string name);
	int32_t IsDefaultUniform(std::string name);
	int32_t IsValidDefaultUniform(SpvReflectDescriptorBinding *block, int32_t index);
	bool SetDefaultUniform(DefaultUniformStruct structure);
	void GetShaderData(std::string _path);
};

VkFormat VertexTypeToVkFormat( VertexElementType vt );
VkDescriptorType DescriptorTypeSpvToVk( SpvReflectDescriptorType type );
} // namespace Soon