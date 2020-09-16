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

#include "BufferRenderer.hpp"

namespace Soon
{
	struct GpuRessource
	{
		uint32_t mCount;
	};

	struct MeshRenderer
	{
		MeshBufferRenderer bufferRenderer;
		uint32_t count;
	};

	struct TextureRenderer
	{
		ImageRenderer imageRenderer;
		ImageProperties image;
		uint32_t count = 0;
	};

	class Mesh;
	class Buffer;

	class GraphicsRenderer
	{
	private:
		static GraphicsRenderer* _instance;
		bool _changes;

		// GPURessource
		std::uint32_t m_RessourceCounter = 0;
		std::vector<uint32_t> m_RessourceFreeId;
		std::vector<GpuRessource*> m_GpuRessources;

		// MESH
		std::uint32_t _meshCounter = 0;
		std::vector<uint32_t> _freeId;
		std::vector<MeshRenderer> _meshs;

		// TEXTURE
		std::uint32_t m_TextureCounter = 0;
		std::vector<uint32_t> m_FreeTextureId;
		std::vector<TextureRenderer> m_Textures;
		Texture*				m_DefaultTexture;

		// BUFFER
		std::uint32_t m_BufferCounter = 0;
		std::vector<uint32_t> m_BufferFreeId;
		std::vector<BufferRenderer> m_Buffers = {};

		std::unordered_map<std::string, ComputePipeline*> m_UniqueComputePipelines;
		std::unordered_map<std::string, ComputePipeline*> _computePipelines;
		std::unordered_map<std::string, GraphicPipeline*> _graphicPipelines;

		using ComputePipelinesIterator = std::unordered_map<std::string, ComputePipeline *>::iterator;
		using GraphicPipelinesIterator = std::unordered_map<std::string, GraphicPipeline *>::iterator;

		std::vector<uint32_t> m_MeshToSupress;
		std::vector<uint32_t> m_TextureToSupress;
		std::vector<uint32_t> m_BufferToSupress;

		std::vector<std::string> m_PipelinesToRecreate;
	public:
		//static GraphicsRenderer* _instance;
		GraphicsRenderer(void);
		~GraphicsRenderer(void);
		static GraphicsRenderer* GetInstanceOrNull(void);
		static GraphicsRenderer *GetInstance(void);
		static void ReleaseInstance(void);
		void RemoveAllPipelines(void);

		void Update( void );
		void SetProcessFrequency( std::string name, EProcessFrequency frequency );

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
		BasePipeline* AddPipeline(std::string name);/*, Args... args)*/

		void RemovePipeline(std::string pipeline);
		void AddPipelineToRecreate( std::string name );
		void RecreatePipelines( void );


		// Mesh
		uint32_t AddMesh(Mesh *mesh, uint32_t meshId);
		uint32_t AddMesh(uint32_t meshId);
		void RemoveMesh(uint32_t meshId);
		MeshBufferRenderer &GetMesh(uint32_t id);

		// BUFFER
		bool IsValidBufferId(uint32_t id);
		uint32_t AddBuffer(GpuBuffer& buffer);
		uint32_t AddBuffer(uint32_t bufferId);
		void RemoveBuffer(uint32_t bufferId);
		BufferRenderer& GetBufferRenderer(uint32_t id);

		// TEXTURE
		uint32_t AddTexture(Texture& texture);
		uint32_t AddTexture( uint32_t textureId);
		void RemoveTexture(uint32_t textureId);
		ImageProperties& GetImageProperties(uint32_t id);

		void SetDefaultTexture(Texture& texture);

		void DestroyInvalids( void );

		void DestroyAllUniforms(void);
		void DestroyAllPipelines(void);

	};
} // namespace Soon