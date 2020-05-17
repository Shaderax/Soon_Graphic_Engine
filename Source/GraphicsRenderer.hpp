#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Pipelines/BasePipeline.hpp"
#include "Pipelines/ShaderPipeline.hpp"
#include "Pipelines/ComputePipeline.hpp"

#include "Modules/ClassTypeId.hpp"

#include <bitset>
#include <array>
#include <vector>

#include "ThirdParty/VkMemoryAllocator/vk_mem_alloc.h"

namespace Soon
{
	class Mesh;

	class GraphicsRenderer
	{
	private:
		VmaAllocation m_Allocation;
	public:
		static GraphicsRenderer* _instance;
		static constexpr const std::uint32_t MAX_PIPELINES = 32;
		//static GraphicsRenderer* _instance;

		public:
		GraphicsRenderer( void );
		~GraphicsRenderer( void );
		static GraphicsRenderer* 	GetInstance( void );
		static void 			ReleaseInstance( void );
		void				RemoveAllPipelines( void );

		void 				Initialize( void );
		void 				RecreateAllUniforms( void );
		void 				RecreateAllPipelines( void );
		void 				GraphicPipelinesBindCaller( VkCommandBuffer commandBuffer, uint32_t index );
		void 				ComputePipelinesBindCaller( VkCommandBuffer commandBuffer, uint32_t index );
		void				UpdateAllDatas( uint32_t imageIndex );
		void HasChange( void );
		bool IsChange( void );
		void ResetChange( void );

		// TODO 
		// Max pipelines reach
		template<typename T, typename ... Args>
			T* AddPipeline( Args ... args )
			{
				if (_createdPipeline[ClassTypeId<BasePipeline>::GetId<T>()] == true)
				{
					if (T::_type == PipelineType::COMPUTE)
						return dynamic_cast<T*>(_computePipelines[ClassTypeId<BasePipeline>::GetId<T>()]);
					else if (T::_type == PipelineType::GRAPHIC)
						return dynamic_cast<T*>(_graphicPipelines[ClassTypeId<BasePipeline>::GetId<T>()]);
				}
				T* pipeline;
				pipeline = new T(std::forward<Args>(args) ...);
				if (T::_type == PipelineType::COMPUTE)
					_computePipelines[ClassTypeId<BasePipeline>::GetId<T>()] = dynamic_cast<ComputePipeline*>(pipeline);
				else if (T::_type == PipelineType::GRAPHIC)
					_graphicPipelines[ClassTypeId<BasePipeline>::GetId<T>()] = dynamic_cast<ShaderPipeline*>(pipeline);

				pipeline->Init();
				_createdPipeline[ClassTypeId<BasePipeline>::GetId<T>()] = true;
				_changes = true;

				return (pipeline);
			}


		template<typename T>
			void RemovePipeline( void );

		uint32_t AddMesh(Mesh* mesh);
		void RemoveMesh(uint32_t id);
		MeshBufferRenderer& GetMesh( uint32_t id );
		void DestroyAllUniforms( void );
		void DestroyAllGraphicsPipeline( void );

		private:
		std::array<ShaderPipeline*, MAX_PIPELINES / 2>		_graphicPipelines{};
		std::array<ComputePipeline*, MAX_PIPELINES / 2>		_computePipelines{};
		bool _changes;
		std::bitset<MAX_PIPELINES>					_createdPipeline;
		std::vector<uint32_t>	_freeId;
		std::uint32_t _meshCounter = 0;
		std::vector<MeshBufferRenderer> _meshs;
	};
}
