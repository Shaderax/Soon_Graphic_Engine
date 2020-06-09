#pragma once

#include "GraphicsInstance.hpp"

#include "Utilities/ShadersUniform.hpp"
#include "Vertex.hpp"
#include "Utilities/ReadFile.hpp"

#include "Modules/SPIRV-Reflect/spirv_reflect.h"
#include "Modules/Math/mat4.hpp"

#include <vector>
#include <functional>
#include <unordered_map>

#include "UniformsBufferManager.hpp"

namespace Soon
{
	struct DefaultMemberStruct
	{
		std::string name;
		VertexElementType tt;
	};

	struct DefaultUniformStruct
	{
		std::string name;
		//std::string typeName;
		//VertexElementType tt;
		bool isUnique;
		//std::vector<DefaultMemberStruct> members;
		//std::function<void(int)> updateFunc;
	};

	class BasePipeline
	{
	protected:
		VkPipelineLayout _pipelineLayout;
		std::vector<std::vector<VkDescriptorSetLayoutBinding>> uboLayoutBinding;
		UniformsBufferManager _mUbm;

	private:
		std::unordered_map<uint32_t, uint32_t> m_ToDraw;
		std::vector<uint32_t> _freeId;

	public:
		VkVertexInputBindingDescription _bindingDescription;
		std::vector<VkVertexInputAttributeDescription> _attributeDescriptions;
		std::vector<DefaultUniformStruct> _defaultUniform
		{
			/*
	{"uc", "UniformCamera", {EnumVertexElementType::STRUCT}, true, true, {{"view", "", {EnumVertexElementType::FLOAT, 4, 4}}, {"proj", "", {EnumVertexElementType::FLOAT, 4, 4}}}, &UpdateCamera},
	{"um", "UniformModel", {EnumVertexElementType::STRUCT}, false, true, {{"model", "", {EnumVertexElementType::FLOAT, 4, 4}}}, &UpdateModel}
	*/
		};

		BasePipeline(void);
		virtual ~BasePipeline();

		virtual void Init() = 0;

		// Getter
		void GetBindingDescription(void);
		void GetShaderData(std::string _path);
		VertexDescription GetVertexDescription();

		void BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage);

		void RecreateUniforms(void);
		virtual void RecreatePipeline(void) = 0;

		void Render(uint32_t id);
		void UnRender(uint32_t id);

		void RemoveFromPipeline(uint32_t id);
		uint32_t AddToPipeline(std::uint32_t meshId);

		void DestroyAllUniforms(void);
		void DestroyGraphicPipeline(void);

		void* Get(std::string name, uint32_t id);
		// Setter
		void Set(std::string name, void *value, uint32_t id);
		void SetTexture(std::string name, uint32_t idMat, uint32_t textureId);
		bool SetDefaultUniform(DefaultUniformStruct structure);
		//void SetMesh(uint32_t matId, uint32_t meshId );

		void UpdateData(int currentImg);

		int32_t IsDefaultVertexInput(std::string name);
		int32_t IsDefaultUniform(std::string name);
		int32_t IsValidDefaultUniform(SpvReflectDescriptorBinding *block, int32_t index);

		void GetInputBindings( spv_reflect::ShaderModule& reflection );
		void GetDescriptorBindings( spv_reflect::ShaderModule& reflection );
	};

	VkFormat VertexTypeToVkFormat(VertexElementType vt);
	VkDescriptorType DescriptorTypeSpvToVk(SpvReflectDescriptorType type);
} // namespace Soon
