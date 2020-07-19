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
		VkPipeline _pipeline;
		PipelineConf* _conf;
		std::unordered_map<uint32_t, uint32_t> m_ToDraw;
		std::vector<uint32_t> _freeId;

		void ParseUniform(SpvReflectDescriptorBinding* binding);
		void ParseTextureUniform(SpvReflectDescriptorBinding* binding);
		void ParseRuntimeUniform(SpvReflectDescriptorBinding* binding);

		UniformVar ParseUniformMembers(SpvReflectBlockVariable& block) const;
		UniformRuntimeVar ParseRuntimeUniformMembers(SpvReflectBlockVariable& block) const;

	public:
		std::vector<DefaultUniformStruct> _defaultUniform
		{
			/*
	{"uc", "UniformCamera", {EnumVertexElementType::STRUCT}, true, true, {{"view", "", {EnumVertexElementType::FLOAT, 4, 4}}, {"proj", "", {EnumVertexElementType::FLOAT, 4, 4}}}, &UpdateCamera},
	{"um", "UniformModel", {EnumVertexElementType::STRUCT}, false, true, {{"model", "", {EnumVertexElementType::FLOAT, 4, 4}}}, &UpdateModel}
	*/
		};

		BasePipeline(PipelineConf* conf);
		virtual ~BasePipeline();

		virtual void Init() = 0;

		// Getter
		void GetShaderData(std::string _path);

		virtual void BindCaller(VkCommandBuffer commandBuffer, uint32_t currentImage) = 0;

		void RecreateUniforms(void);
		virtual void RecreatePipeline(void) = 0;

		virtual uint32_t CreateNewId( void ) = 0;
		virtual void RemoveId(uint32_t id) = 0;

		void DestroyAllUniforms(void);
		void DestroyPipeline(void);

		void* Get(std::string name, uint32_t id);
		// Setter
		void Set(std::string name, void *value, uint32_t id);
		void SetUnique(std::string name, void* value);
		void SetTexture(std::string name, uint32_t idMat, uint32_t textureId);
		bool SetDefaultUniform(DefaultUniformStruct structure);
		//void SetMesh(uint32_t matId, uint32_t meshId );

		void UpdateData(int currentImg);

		int32_t IsDefaultUniform(std::string name);
		int32_t IsValidDefaultUniform(SpvReflectDescriptorBinding *block, int32_t index);

		virtual void GetInputBindings( spv_reflect::ShaderModule& reflection );
		void GetDescriptorBindings( spv_reflect::ShaderModule& reflection );

		void SetRuntimeAmount(std::string name, uint32_t amount, uint32_t idMat);
		void SetRuntimeBuffer(std::string name, GpuBuffer& buffer, uint32_t idMat);

		UniformRuntime& GetUniformRuntime(std::string name);
	};

	VkFormat VertexTypeToVkFormat(VertexElementType vt);
	VkDescriptorType DescriptorTypeSpvToVk(SpvReflectDescriptorType type);
} // namespace Soon
