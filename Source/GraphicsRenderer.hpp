#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Pipelines/BasePipeline.hpp"
#include "Pipelines/GraphicPipeline.hpp"
#include "Pipelines/ComputePipeline.hpp"

#include "Modules/ClassTypeId.hpp"

#include <bitset>
#include <array>
#include <vector>

#include "VkMemoryAllocator/vk_mem_alloc.h"

#include "Pipelines/PipelineJson.hpp"

namespace Soon
{
	struct MeshRenderer
	{
		MeshBufferRenderer bufferRenderer;
		uint32_t count;
	};

	struct TextureRenderer
	{
		ImageRenderer imageRenderer;
		ImageProperties image;
		uint32_t count;
	};

	class Mesh;

	class GraphicsRenderer
	{
	private:
		static GraphicsRenderer* _instance;
		bool _changes;

		std::uint32_t _meshCounter = 0;
		std::vector<uint32_t> _freeId;
		std::vector<MeshRenderer> _meshs;

		std::uint32_t m_TextureCounter = 0;
		std::vector<uint32_t> m_FreeTextureId;
		std::vector<TextureRenderer> m_Textures;
		Texture*				m_DefaultTexture;

		std::unordered_map<std::string, ComputePipeline*> _computePipelines;
		std::unordered_map<std::string, GraphicPipeline*> _graphicPipelines;

		using ComputePipelinesIterator = std::unordered_map<std::string, ComputePipeline *>::iterator;
		using GraphicPipelinesIterator = std::unordered_map<std::string, GraphicPipeline *>::iterator;

		std::vector<uint32_t> m_MeshToSupress;
		std::vector<uint32_t> m_TextureToSupress;
	public:
		//static GraphicsRenderer* _instance;
		GraphicsRenderer(void);
		~GraphicsRenderer(void);
		static GraphicsRenderer* GetInstanceOrNull(void);
		static GraphicsRenderer *GetInstance(void);
		static void ReleaseInstance(void);
		void RemoveAllPipelines(void);

		void Initialize(void);
		void RecreateAllUniforms(void);
		void RecreateAllPipelines(void);
		void GraphicPipelinesBindCaller(VkCommandBuffer commandBuffer, uint32_t index);
		void ComputePipelinesBindCaller(VkCommandBuffer commandBuffer, uint32_t index);
		void UpdateAllDatas(uint32_t imageIndex);
		void HasChange(void);
		bool IsChange(void);
		void ResetChange(void);
		bool IsValidMeshId( uint32_t id );
		bool IsValidTextureId(uint32_t id);

		//NEW
		// Max pipelines reach
		//template <typename... Args>
		BasePipeline* AddPipeline(std::string name)/*, Args... args)*/
		{
			GraphicPipelinesIterator graphicPip = _graphicPipelines.find(name);
			if (graphicPip != _graphicPipelines.end())
				return graphicPip->second;
			ComputePipelinesIterator computePip = _computePipelines.find(name);
			if (computePip != _computePipelines.end())
				return computePip->second;
			
			PipelineConf* conf = ReadPipelineJson(name);

			BasePipeline* pipeline;
			if ((conf->GetType() == EPipelineType::COMPUTE))
			{
				pipeline = new ComputePipeline(reinterpret_cast<ComputePipelineConf*>(conf));
				_computePipelines[name] = reinterpret_cast<ComputePipeline*>(pipeline);
			}
			else if ((conf->GetType() == EPipelineType::GRAPHIC))
			{
				pipeline = new GraphicPipeline(reinterpret_cast<GraphicPipelineConf*>(conf));
				_graphicPipelines[name] = reinterpret_cast<GraphicPipeline*>(pipeline);
			}

			pipeline->Init();
			_changes = true;

			return (pipeline);
		}

		void RemovePipeline(std::string pipeline);

		// Add Mesh in tab
		uint32_t AddMesh(Mesh *mesh, uint32_t meshId);
		void RemoveMesh(uint32_t meshId);
		MeshBufferRenderer &GetMesh(uint32_t id);

		void DestroyInvalids( void );


		void DestroyAllUniforms(void);
		void DestroyAllGraphicsPipeline(void);

		uint32_t AddTexture(Texture* texture);
		uint32_t AddTexture( uint32_t textureId);
		void RemoveTexture(uint32_t textureId);
		ImageProperties& GetImageProperties(uint32_t id);

		void SetDefaultTexture(Texture* texture);
	};
} // namespace Soon