#pragma once

#include "Graphics/Pipelines/ComputePipeline.hpp"
#include "Core/Engine.hpp"
#include "Scene/3D/Components/Camera.hpp"
#include "Scene/3D/Components/ParticlesSystem.hpp"
#include "Utilities/ShadersUniform.hpp"

#include <cstring>

namespace Soon
{
	struct DefaultComputeParticlesSystemPipeline : ComputePipeline
	{
		public:
			struct Particle
			{
				vec3<float> _position;
				vec3<float> _velocity;
			};

			std::vector< VkBuffer >*				_gpuBuffers;
			std::vector< uint32_t >*			_pSize;

			std::vector< std::vector< VkDescriptorSet > >		_particlesDescriptorSets;

			DefaultComputeParticlesSystemPipeline( void )
			{
				_descriptorSetLayout = GraphicsInstance::GetInstance()->CreateDescriptorSetLayout( GetLayoutBinding() );
				_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_descriptorSetLayout);
				_computePipeline = GraphicsInstance::GetInstance()->CreateComputePipeline(
						_pipelineLayout,
						"../Ressources/Shaders/DefaultParticles.comp.spv");
			//	_gpuBuffers = &pSysPipeline->GetGpuBuffers();
			//	_pSize = &pSysPipeline->GetPSize();
			}

			void UpdateData( int currentImage )
			{
			}

			void BindCaller( VkCommandBuffer commandBuffer, uint32_t index )
			{
				uint32_t j = 0;
				// Compute //
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _graphicPipeline);
				for (auto& buf : *_gpuBuffers)
				{
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _pipelineLayout, 0, 1, &(_particlesDescriptorSets.at(j).at(index)), 0, nullptr);
					vkCmdDispatch(commandBuffer, (*_pSize).at(j), 1, 1);
					j++;
				}
			}

			std::vector<VkDescriptorSetLayoutBinding> GetLayoutBinding( void )
			{
				std::vector<VkDescriptorSetLayoutBinding> uboLayoutBinding(1);

				///////////// PARTICLES ////////////
				uboLayoutBinding[0].binding = 0;
				uboLayoutBinding[0].descriptorCount = 1;
				uboLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				uboLayoutBinding[0].pImmutableSamplers = nullptr;
				uboLayoutBinding[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

				return (uboLayoutBinding);
			}

		bool	RemoveFromPipeline( uint32_t id )
		{

		}

			uint32_t AddToRender( Transform3D& tr, Mesh* mesh, uint32_t amount )
			{
				std::cout << "Create Particle System Descriptor Set" << std::endl;
				// Go creer un tas de transform 
				_particlesDescriptorSets.push_back(GraphicsInstance::GetInstance()->CreateDescriptorSets(ps->_size * sizeof(Particle), _descriptorSetLayout, 0, &((*_gpuBuffers).back()), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER));
				std::cout << "END : Create Particle System Descriptor Set" << std::endl;
				return _particlesDescriptorSets.size();
			}

			void RecreateUniforms( void )
			{
				uint32_t j = 0;
				for (auto& pDs : _particlesDescriptorSets)
				{
					pDs = GraphicsInstance::GetInstance()->CreateDescriptorSets((*_pSize).at(j) * sizeof(Particle), _descriptorSetLayout, 0, &((*_gpuBuffers).at(j)), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
					++j;
				}
			}

			void RecreatePipeline( void )
			{
				_computePipeline = GraphicsInstance::GetInstance()->CreateComputePipeline(
						_pipelineLayout,
						"../Ressources/Shaders/DefaultParticles.comp.spv");
			}
	VkVertexInputBindingDescription GetBindingDescription( void ) {}
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions( void ) {}
	};
}
