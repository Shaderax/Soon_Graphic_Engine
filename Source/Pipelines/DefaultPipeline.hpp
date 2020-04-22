#pragma once

#include <cstring>
#include "Core/Engine.hpp"
#include "Core/Parsers/RessourceImporter.hpp"
#include "Core/Scene/Common/Texture2D.hpp"
#include "Core/Scene/Materials/Material.hpp"
#include "Core/Scene/3D/Components/Camera.hpp"
#include "Core/Scene/3D/Components/Mesh.hpp"
#include "Core/Renderer/Pipelines/BasePipeline.hpp"
#include "Core/Renderer/Pipelines/ShaderPipeline.hpp"
#include "Core/Renderer/Vulkan/PipelineConf.hpp"
#include "Core/Renderer/Vulkan/GraphicsInstance.hpp"
#include "Utilities/Vertex.hpp"
#include "Utilities/ShadersUniform.hpp"
#include <vector>

namespace Soon
{
	struct DefaultPipeline : ShaderPipeline
	{
		//// UNIFORM CAMERA
		UniformSets						_uniformCamera;

		std::vector< Transform3D* >			_transforms;

		std::vector< VkBuffer >			_gpuBuffers;
		std::vector< VkDeviceMemory >		_gpuMemoryBuffers;
		std::vector< BufferRenderer >		_indexBuffers;
		std::vector< uint32_t >			_indexSize;

		std::vector< ImageRenderer >		_imagesRenderer;
		std::vector< std::vector< VkDescriptorSet > >	_uniformsImagesDescriptorSets;
		std::vector< Image >			_images;

		// UNIFORM
		std::vector< BufferRenderer > 	_uniformsBuffers;
		std::vector< std::vector< VkDescriptorSet > >	_uniformsDescriptorSets;

		// UNIFORM MATERIAL
		// NEW
		std::vector<Material*>			_vecMaterials;
		std::vector< BufferRenderer > 	_uniformsMaterials;
		std::vector< std::vector< VkDescriptorSet > >	_uniformsMaterialsDescriptorSets;

		// UNIFORM LIGHT
		//		std::vector<DirectionalLight*>		_vecLights;
		std::vector< BufferRenderer > 	_uniformsLights;
		std::vector< std::vector< VkDescriptorSet > >	_uniformsLightsDescriptorSets;

		GraphicsPipelineConf		_conf;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		VkVertexInputBindingDescription bindingDescription = {};

		DefaultPipeline( void )
		{
			GetBindingDescription();
			GetAttributeDescriptions();
			_conf.vertexInputInfo.vertexBindingDescriptionCount = 1;
			_conf.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			_conf.vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			_conf.vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			_descriptorSetLayout = GraphicsInstance::GetInstance()->CreateDescriptorSetLayout( GetLayoutBinding() );
			_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_descriptorSetLayout);
			_conf.pipelineInfo.layout = _pipelineLayout;
			_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
					_conf,
					"../Ressources/Shaders/DefaultShader.vert.spv",
					"../Ressources/Shaders/DefaultShader.frag.spv");

			_uniformCamera = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformCamera), _descriptorSetLayout, 0);

			UniformSets lightUniform = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformLight), _descriptorSetLayout, 4);

			_uniformsLights.push_back(lightUniform._uniformRender);
			_uniformsLightsDescriptorSets.push_back(lightUniform._descriptorSets);
		}

		~DefaultPipeline( void )
		{
			VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

			for (VkBuffer buffer : _gpuBuffers)
				vkDestroyBuffer(device, buffer, nullptr);
			for (VkDeviceMemory bufferMemory : _gpuMemoryBuffers)
				vkFreeMemory(device, bufferMemory, nullptr);

			DestroyBufferRenderer(_indexBuffers);
			DestroyBufferRenderer(_uniformsBuffers);
			DestroyBufferRenderer(_uniformsMaterials);
			DestroyBufferRenderer(_uniformsLights);

			for (VkDescriptorSetLayout& dsl : _descriptorSetLayout)
				vkDestroyDescriptorSetLayout(device, dsl, nullptr);

			vkDestroyPipeline(device, _graphicPipeline, nullptr);
			vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

			for (ImageRenderer& img : _imagesRenderer)
			{
				vkDestroyImage(device, img._textureImage, nullptr);
				vkFreeMemory(device, img._textureImageMemory, nullptr);
			}

			for (Image& img : _images)
			{
				vkDestroySampler(device, img._textureSampler, nullptr);
				vkDestroyImageView(device, img._imageView, nullptr);
			}

			for (VkBuffer& bu : _uniformCamera._uniformRender._Buffer)
				vkDestroyBuffer(device, bu, nullptr);
			for (VkDeviceMemory& bu : _uniformCamera._uniformRender._BufferMemory)
				vkFreeMemory(device, bu, nullptr);
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

			/////////// Model //////////
			////////////// NEED TO MERGE MATERIAL AND MODEL

			data = nullptr;

			// For Every Uniform
			int i = -1;
			for (auto& uniform : _uniformsBuffers)
			{
				++i;

				vkMapMemory(device, uniform._BufferMemory[currentImage], 0, sizeof(UniformModel), 0, &data);

				mat4<float> matModel;

				Transform3D* transform = _transforms.at(i);

				matModel = transform->_rot.GetRotationMatrix();

				matModel(0,3) = transform->_pos.x;
				matModel(1,3) = transform->_pos.y;
				matModel(2,3) = transform->_pos.z;

				matModel(0,0) *= transform->_scale.x;
				matModel(1,1) *= transform->_scale.y;
				matModel(2,2) *= transform->_scale.z;

				memcpy(data, &matModel, sizeof(UniformModel));
				vkUnmapMemory(device, uniform._BufferMemory[currentImage]);
			}
			data = nullptr;

			///// MATERIALS
			i = -1;
			for (auto& uniformMaterial : _uniformsMaterials)
			{
				++i;

				vkMapMemory(device, uniformMaterial._BufferMemory[currentImage], 0, sizeof(UniformMaterial), 0, &data);

				memcpy(static_cast<char*>(data) + offsetof(UniformMaterial, _ambient), &(_vecMaterials.at(i)->GetVec3("ambient")), sizeof(vec3<float>));
				memcpy(static_cast<char*>(data) + offsetof(UniformMaterial, _diffuse), &(_vecMaterials.at(i)->GetVec3("diffuse")), sizeof(vec3<float>));
				memcpy(static_cast<char*>(data) + offsetof(UniformMaterial, _specular), &(_vecMaterials.at(i)->GetVec3("specular")), sizeof(vec3<float>));
				memcpy(static_cast<char*>(data) + offsetof(UniformMaterial, _shininess), &(_vecMaterials.at(i)->GetFloat("shininess")), sizeof(float));

				vkUnmapMemory(device, uniformMaterial._BufferMemory[currentImage]);
			}
			data = nullptr;

			///////// LIGHTs
			UniformLight li;
			li._direction = vec3<float>(0.0f, 1.0f, 0.0f);
			li._lightColor = vec3<float>(1.0f, 1.0f, 1.0f);
			li._intensity = 1.0f;

			i = -1;
			for (auto& uniformLight : _uniformsLights)
			{
				++i;
				vkMapMemory(device, uniformLight._BufferMemory[currentImage], 0, sizeof(UniformLight), 0, &data);
				memcpy(data, &(li._direction), sizeof(UniformLight));
				vkUnmapMemory(device, uniformLight._BufferMemory[currentImage]);
			}

		}

		std::vector<VkDescriptorSetLayoutBinding> GetLayoutBinding( void )
		{
			std::vector<VkDescriptorSetLayoutBinding> uboLayoutBinding(5);

			////// BINDING 0 : CAM //////////
			uboLayoutBinding[0].binding = 0;
			uboLayoutBinding[0].descriptorCount = 1;
			uboLayoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding[0].pImmutableSamplers = nullptr;
			uboLayoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			////// BINDING 1 : MODEL //////////
			uboLayoutBinding[1].binding = 0;
			uboLayoutBinding[1].descriptorCount = 1;
			uboLayoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding[1].pImmutableSamplers = nullptr;
			uboLayoutBinding[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			//////////// IMAGE ///////////////
			uboLayoutBinding[2].binding = 0;
			uboLayoutBinding[2].descriptorCount = 1;
			uboLayoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			uboLayoutBinding[2].pImmutableSamplers = nullptr;
			uboLayoutBinding[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			///////// MATERIALS /////////////
			uboLayoutBinding[3].binding = 0;
			uboLayoutBinding[3].descriptorCount = 1;
			uboLayoutBinding[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding[3].pImmutableSamplers = nullptr;
			uboLayoutBinding[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			///////////// LIGHTS ////////////
			uboLayoutBinding[4].binding = 0;
			uboLayoutBinding[4].descriptorCount = 1;
			uboLayoutBinding[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding[4].pImmutableSamplers = nullptr;
			uboLayoutBinding[4].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

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

		bool	RemoveFromPipeline( uint32_t id )
		{
			/*
			VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

			for (VkBuffer buffer : _gpuBuffers)
				vkDestroyBuffer(device, buffer, nullptr);
			for (VkDeviceMemory bufferMemory : _gpuMemoryBuffers)
				vkFreeMemory(device, bufferMemory, nullptr);

			DestroyBufferRenderer(_indexBuffers);
			DestroyBufferRenderer(_uniformsBuffers);
			DestroyBufferRenderer(_uniformsMaterials);
			DestroyBufferRenderer(_uniformsLights);

			for (VkDescriptorSetLayout& dsl : _descriptorSetLayout)
				vkDestroyDescriptorSetLayout(device, dsl, nullptr);

			vkDestroyPipeline(device, _graphicPipeline, nullptr);
			vkDestroyPipelineLayout(device, _pipelineLayout, nullptr);

			for (ImageRenderer& img : _imagesRenderer)
			{
				vkDestroyImage(device, img._textureImage, nullptr);
				vkFreeMemory(device, img._textureImageMemory, nullptr);
			}

			for (Image& img : _images)
			{
				vkDestroySampler(device, img._textureSampler, nullptr);
				vkDestroyImageView(device, img._imageView, nullptr);
			}

			for (VkBuffer& bu : _uniformCamera._uniformRender._Buffer)
				vkDestroyBuffer(device, bu, nullptr);
			for (VkDeviceMemory& bu : _uniformCamera._uniformRender._BufferMemory)
				vkFreeMemory(device, bu, nullptr);
			_gpuBuffers.erase(_gpuBuffers.begin() + id );
			_gpuMemoryBuffers.erase(_gpuMemoryBuffers.begin() + id );

			   _indexBuffers.erase(_indexBuffers.begin() + id );
			   _indexSize.erase(_indexSize.begin() + id );
			   _transforms.erase( _transforms.begin() + id );
			_uniformsBuffers.erase(_uniformsBuffers.begin() + id);
			_uniformsDescriptorSets.erase(_uniformsDescriptorSets.begin() + id );
			_imagesRenderer.erase(_imagesRenderer.begin() + id );

			_images.erase(_images.begin() + id );
			_uniformsImagesDescriptorSets.erase(_uniformsImagesDescriptorSets.begin() + id );
			_uniformsMaterials.erase(_uniformsMaterials.begin() + id );
			_uniformsMaterialsDescriptorSets.erase(_uniformsMaterialsDescriptorSets.begin() + id );
			_vecMaterials.erase(_vecMaterials.begin() + id);
			*/

			return true;
		}

		/*
		void BindCaller( VkCommandBuffer commandBuffer, uint32_t index )
		{
			std::cout << "DefaultPipeline BindCaller" << std::endl;
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicPipeline);

			VkDeviceSize offsets[] = {0};

			// Bind Cam
			if (!_gpuBuffers.empty())
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &(_uniformCamera._descriptorSets.at(index)), 0, nullptr);

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 4, 1, &(_uniformsLightsDescriptorSets.at(0).at(index)), 0, nullptr);

			uint32_t j = 0;
			for (auto& buf : _gpuBuffers)
			{
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buf, offsets);

				vkCmdBindIndexBuffer(commandBuffer, _indexBuffers.at(j)._Buffer[0], 0, VK_INDEX_TYPE_UINT32);

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 1, 1, &_uniformsDescriptorSets.at(j).at(index), 0, nullptr);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 2, 1, &_uniformsImagesDescriptorSets.at(j).at(index), 0, nullptr);

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 3, 1, &_uniformsMaterialsDescriptorSets.at(j).at(index), 0  , nullptr);

				vkCmdDrawIndexed(commandBuffer, _indexSize.at(j), 1, 0, 0, 0);
				++j;
			}
		}


		uint32_t AddToRender( Transform3D& tr, Mesh* mesh )
		{
			BufferRenderer handler = GraphicsInstance::GetInstance()->CreateVertexBuffer(mesh->_vertices->size() * sizeof(Vertex), mesh->_vertices->data());

			_gpuBuffers.push_back(handler._Buffer[0]);
			_gpuMemoryBuffers.push_back(handler._BufferMemory[0]);

			_indexBuffers.push_back(GraphicsInstance::GetInstance()->CreateIndexBuffer(*(mesh->_indices)));

			_indexSize.push_back(mesh->_indices->size());
			_transforms.push_back(&tr);

			///////////// UNIFORM /////////////

			UniformSets modelUniform = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformModel), _descriptorSetLayout, 1);

			_uniformsBuffers.push_back(modelUniform._uniformRender);
			_uniformsDescriptorSets.push_back(modelUniform._descriptorSets);

			///////////// TEXTURE ////////////

			Image img;

			Texture* texture = mesh->_material.GetTexture("texSampler");

			if (!texture)
				texture = Soon::RessourceImporter::GetSingleton().Load<Texture2D>("../Ressources/Textures/white.png");

			_imagesRenderer.push_back(GraphicsInstance::GetInstance()->CreateTextureImage(texture->_width, texture->_height, texture->_data, static_cast<int32_t>(texture->_tType), static_cast<int32_t>(texture->_format)));
			img._textureSampler = GraphicsInstance::GetInstance()->CreateTextureSampler();
			img._imageView = GraphicsInstance::GetInstance()->CreateImageView(_imagesRenderer.back()._textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
			_images.push_back(img);

			std::vector<VkDescriptorSet> imageUniform = GraphicsInstance::GetInstance()->CreateImageDescriptorSets(img._imageView, img._textureSampler, _descriptorSetLayout[2]);
			_uniformsImagesDescriptorSets.push_back(imageUniform);

			/////////// MATERIAL //////////////

			UniformSets matUniform = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformMaterial), _descriptorSetLayout, 3);

			_uniformsMaterials.push_back(matUniform._uniformRender);
			_uniformsMaterialsDescriptorSets.push_back(matUniform._descriptorSets);
			_vecMaterials.push_back(&(mesh->_material));

			/////////////////

			//			return ret;
			return _transforms.size();
		}
*/
		void RecreateUniforms( void )
		{
			VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

			for (VkBuffer& bu : _uniformCamera._uniformRender._Buffer)
				vkDestroyBuffer(device, bu, nullptr);
			for (VkDeviceMemory& bu : _uniformCamera._uniformRender._BufferMemory)
				vkFreeMemory(device, bu, nullptr);
			_uniformCamera = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformCamera), _descriptorSetLayout, 0);

			std::size_t j = 0;
			while (j < _uniformsBuffers.size())
			{
				for (VkBuffer& bu : _uniformsBuffers.at(j)._Buffer)
					vkDestroyBuffer(device, bu, nullptr);
				for (VkDeviceMemory& bu : _uniformsBuffers.at(j)._BufferMemory)
					vkFreeMemory(device, bu, nullptr);
				UniformSets modelUniform = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformModel), _descriptorSetLayout, 1);
				_uniformsBuffers.at(j) = modelUniform._uniformRender;
				_uniformsDescriptorSets.at(j) = modelUniform._descriptorSets;
				++j;
			}
			j = 0;
			while (j < _uniformsImagesDescriptorSets.size())
			{	
				std::vector<VkDescriptorSet> imageUniform = GraphicsInstance::GetInstance()->CreateImageDescriptorSets(_images.at(j)._imageView, _images.at(j)._textureSampler, _descriptorSetLayout[2]);
				_uniformsImagesDescriptorSets.at(j) = imageUniform;
				++j;
			}

			/// RECREATE MATERIALS
			j = 0;
			while (j < _uniformsMaterialsDescriptorSets.size())
			{
				for (VkBuffer& bu : _uniformsMaterials.at(j)._Buffer)
					vkDestroyBuffer(device, bu, nullptr);
				for (VkDeviceMemory& bu : _uniformsMaterials.at(j)._BufferMemory)
					vkFreeMemory(device, bu, nullptr);

				UniformSets matUniform = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformMaterial), _descriptorSetLayout, 3);
				_uniformsMaterials.at(j) = matUniform._uniformRender;
				_uniformsMaterialsDescriptorSets.at(j) = matUniform._descriptorSets;
				++j;
			}

			///// RECREATE LIGHTS
			j = 0;
			//while (j < _vecLights.size())
			while (j < _uniformsLights.size())
			{
				for (VkBuffer& bu : _uniformsLights.at(j)._Buffer)
					vkDestroyBuffer(device, bu, nullptr);
				for (VkDeviceMemory& bu : _uniformsLights.at(j)._BufferMemory)
					vkFreeMemory(device, bu, nullptr);

				UniformSets lightUniform = GraphicsInstance::GetInstance()->CreateUniform(sizeof(UniformLight), _descriptorSetLayout, 4);
				_uniformsLights.at(j) = lightUniform._uniformRender;
				_uniformsLightsDescriptorSets.at(j) = lightUniform._descriptorSets;
				++j;
			}
		}

		void RecreatePipeline( void )
		{
			vkDestroyPipeline(GraphicsInstance::GetInstance()->GetDevice(), _graphicPipeline, nullptr);
			_conf.pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
			_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
					_conf,
					"../Ressources/Shaders/DefaultShader.vert.spv",
					"../Ressources/Shaders/DefaultShader.frag.spv");
		}

		void DestroyBufferRenderer(std::vector<BufferRenderer>& buffer)
		{
			VkDevice device = GraphicsInstance::GetInstance()->GetDevice();

			for (BufferRenderer& br : buffer)
			{
				for (VkBuffer& bu : br._Buffer)
					vkDestroyBuffer(device, bu, nullptr);
				for (VkDeviceMemory& bu : br._BufferMemory)
					vkFreeMemory(device, bu, nullptr);
			}
		}

		void Enable( void ) {};
		void Disable( void ) {};
	};
}
