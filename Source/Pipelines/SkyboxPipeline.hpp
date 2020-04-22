#pragma once

#include "Core/Engine.hpp"

#include "Core/Renderer/Pipelines/ShaderPipeline.hpp"
#include "Core/Scene/3D/Components/Camera.hpp"
#include "Core/Scene/Common/TextureCubeMap.hpp"
#include "Core/Scene/3D/Components/Mesh.hpp"
#include "Utilities/MeshArray.hpp"
#include "Utilities/ShadersUniform.hpp"
#include "Core/Parsers/RessourceImporter.hpp"

#include <cstring>

#include "Core/Renderer/Vulkan/PipelineConf.hpp"

namespace Soon
{
	class SkyboxPipeline : public ShaderPipeline
	{
		public:
			GraphicsPipelineConf		_conf;
			std::vector< Transform3D* >	_transforms;
			UniformSets			_uniformCamera;
			TextureCubeMap			_skybox;

			std::vector< ImageRenderer >			_imagesRenderer;
			std::vector< std::vector< VkDescriptorSet > >	_uniformsImagesDescriptorSets;
			std::vector< Image >				_images;

			std::vector< uint32_t >			_nbVertex;
			std::vector< VkBuffer >			_gpuBuffers;
			std::vector< VkDeviceMemory >		_gpuMemoryBuffers;
			std::vector< BufferRenderer >		_stagingBuffers;
			std::vector< BufferRenderer >		_indexBuffers;
			std::vector< uint32_t >			_indexSize;

			std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
			VkVertexInputBindingDescription bindingDescription;

			SkyboxPipeline( void )
			{
				GetBindingDescription( );
				GetAttributeDescriptions( );
				_conf.rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;

				_conf.vertexInputInfo.vertexBindingDescriptionCount = 1;
				_conf.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
				_conf.vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
				_conf.vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

				_descriptorSetLayout = GraphicsInstance::GetInstance()->CreateDescriptorSetLayout( GetLayoutBinding() );
				_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_descriptorSetLayout);
				_conf.pipelineInfo.layout = _pipelineLayout;
				_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
						_conf,
						"../Ressources/Shaders/SkyboxShader.vert.spv",
						"../Ressources/Shaders/SkyboxShader.frag.spv");

				_uniformCamera = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformCamera), _descriptorSetLayout, 0);

				///////////////

				MeshArray* ma = RessourceImporter::GetSingleton().Load<MeshArray>("../Ressources/Objects/Basics/cube2.obj");

				Mesh skybox = (*ma)[0];

				BufferRenderer handler = GraphicsInstance::GetInstance()->CreateVertexBuffer(skybox._vertices->size() * sizeof(Vertex), skybox._vertices->data());

				_gpuBuffers.push_back(handler._Buffer[0]);
				_gpuMemoryBuffers.push_back(handler._BufferMemory[0]);
				_indexBuffers.push_back(GraphicsInstance::GetInstance()->CreateIndexBuffer(*(skybox._indices)));
				_nbVertex.push_back(skybox._vertices->size());
				_indexSize.push_back(skybox._indices->size());

				Image img;
				_imagesRenderer.push_back(GraphicsInstance::GetInstance()->CreateTextureImage(_skybox._width, _skybox._height, _skybox._data, static_cast<int32_t>(_skybox._tType), static_cast<int32_t>(_skybox._format)));
				img._textureSampler = GraphicsInstance::GetInstance()->CreateTextureSampler();
				img._imageView = GraphicsInstance::GetInstance()->CreateImageView(_imagesRenderer.back()._textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE);
				_images.push_back(img);

				std::vector<VkDescriptorSet> imageUniform = GraphicsInstance::GetInstance()->CreateImageDescriptorSets(img._imageView, img._textureSampler, _descriptorSetLayout[1]);
				_uniformsImagesDescriptorSets.push_back(imageUniform);
			}

			void BindCaller( VkCommandBuffer commandBuffer, uint32_t index )
			{
				std::cout << "SkyboxPipeline BindCaller" << std::endl;
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicPipeline);

				VkDeviceSize offsets[] = {0};

				vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_gpuBuffers.at(0), offsets);
				vkCmdBindIndexBuffer(commandBuffer, _indexBuffers.at(0)._Buffer[0], 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &(_uniformCamera._descriptorSets.at(index)), 0, nullptr);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 1, 1, &_uniformsImagesDescriptorSets.at(0).at(index), 0, nullptr);

				vkCmdDrawIndexed(commandBuffer, _indexSize.at(0), 1, 0, 0, 0);
			}

			uint32_t AddToRender( Transform3D& tr, Mesh* mesh )
			{

			}

			bool RemoveFromPipeline( uint32_t id )
			{

			}

			void Enable( void )
			{

			}

			void Disable( void )
			{

			}

			void RecreateUniforms( void )
			{
				_uniformCamera = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformCamera), _descriptorSetLayout, 0);

				// TO Delete : Why ?
				std::vector<VkDescriptorSet> imageUniform = GraphicsInstance::GetInstance()->CreateImageDescriptorSets(_images.back()._imageView, _images.back()._textureSampler, _descriptorSetLayout[1]);
				_uniformsImagesDescriptorSets[0] = imageUniform;
			}

			void UpdateData( int currentImage )
			{
				void* data = nullptr;
				VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

				//////////// Cam ///////////
				UniformCamera uc = {};

							if (Engine::GetInstance().GetCurrentScene() && Engine::GetInstance().GetCurrentScene()->GetCurrentCamera())
							{
								uc.view = Engine::GetInstance().GetCurrentScene()->GetCurrentCamera()->GetViewMatrix();
								uc.proj = Engine::GetInstance().GetCurrentScene()->GetCurrentCamera()->GetProjectionMatrix();
							}
							else
							{
				          std::cout << "No Current Camera.";
								uc.view = mat4<float>();
								uc.proj = mat4<float>();
							}

							std::vector<VkDeviceMemory> vkdm = _uniformCamera._uniformRender._BufferMemory;
							vkMapMemory(device, vkdm[currentImage], 0, sizeof(UniformCamera), 0, &data);
							memcpy(data, &uc, sizeof(UniformCamera));
							vkUnmapMemory(device, vkdm[currentImage]);
			}

			std::vector<VkDescriptorSetLayoutBinding> GetLayoutBinding( void )
			{
				std::vector<VkDescriptorSetLayoutBinding> uboLayoutBinding(2);

				/////// CAM /////////////
				uboLayoutBinding[0].binding = 0;
				uboLayoutBinding[0].descriptorCount = 1;
				uboLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				uboLayoutBinding[0].pImmutableSamplers = nullptr;
				uboLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
				//////////// IMAGE ///////////////
				uboLayoutBinding[1].binding = 0;
				uboLayoutBinding[1].descriptorCount = 1;
				uboLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				uboLayoutBinding[1].pImmutableSamplers = nullptr;
				uboLayoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

				return (uboLayoutBinding);
			}

			void GetBindingDescription( void )
			{
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex); // stride : size of one pointe
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			}

			void GetAttributeDescriptions( void )
			{
				attributeDescriptions.resize(3);

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, _position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, _normal);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;//VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Vertex, _texCoords);
			}

			void RecreatePipeline( void )
			{
				_conf.pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
				_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
						_conf,
						"../Ressources/Shaders/SkyboxShader.vert.spv",
						"../Ressources/Shaders/SkyboxShader.frag.spv");
			}
	};
}
