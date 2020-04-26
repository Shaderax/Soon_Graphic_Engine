#pragma once

#include "Renderer/Pipelines/BasePipeline.hpp"

#include <vector>

namespace Soon
{
class Mesh;

/*
	 * In material ptr or raw data ?
*/

class ShaderPipeline : public BasePipeline
{
	//private:
	//std::vector<VkVertexInputBindingDescription> bindingDescription = {};

public:
	std::string _pathVert;
	std::string _pathFrag;
	static const PipelineType _type = PipelineType::GRAPHIC;

	ShaderPipeline()
	{
	}

	~ShaderPipeline()
	{
	}

	void Init( void )
	{
		assert(!_pathVert.empty() && !_pathFrag.empty() && "Vertex Path or Frag Path not feed");

		GetShaderData(_pathVert);
		GetShaderData(_pathFrag);

		GetBindingDescription();
	}

/*
	uint32_t AddToPipeline(Mesh *mesh)
	{
		// Check if mesh is equal to the pipeline stride

		//Alloc data
		return (0);
	}
	*/

	//virtual void Render( uint32_t id ) = 0;
	//virtual void UnRender( uint32_t id ) = 0;

	//virtual uint32_t AddToPipeline( Transform3D &transform, Mesh* mesh ) = 0;
	//virtual void RemoveFromPipeline( uint32_t id ) = 0;
	/*
			virtual std::vector<VkDescriptorSetLayoutBinding> GetLayoutBinding( void ) = 0;
			virtual void UpdateData( int currentImg ) = 0;
			virtual void BindCaller( VkCommandBuffer commandBuffer, uint32_t index ) = 0;
			//		virtual std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions( void ) = 0;
			//		virtual VkVertexInputBindingDescription GetBindingDescription( void ) = 0;
			virtual void RecreateUniforms( void ) = 0;
			virtual void RecreatePipeline( void ) = 0;
				*/


	void InternalDataUpdate(uint32_t currentImage)
	{
	}

	/*
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

		vkUnmapMemory(device, uniformMaterial._BufferMemory[currentImage]);
		}
		*/

	uint32_t AddToRender(Mesh *mesh)
	{
		return 0;
		/*
		for (InputVar input : _inputs)
		{
			
		}
		// Vertex input
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
		BufferRenderer handler = GraphicsInstance::GetInstance()->CreateVertexBuffer(mesh->_vertices->size() * sizeof(Vertex), mesh->_vertices->data());

		_gpuBuffers.push_back(handler._Buffer[0]);
		_gpuMemoryBuffers.push_back(handler._BufferMemory[0]);

		_indexBuffers.push_back(GraphicsInstance::GetInstance()->CreateIndexBuffer(*(mesh->_indices)));

		_indexSize.push_back(mesh->_indices->size());
		_transforms.push_back(&tr);
		_vecMaterials.push_back(&(mesh->_material));

		///////////// UNIFORM /////////////
		for (Uniforms &u : _u)
		{
			UniformSets uniform = GraphicsInstance::GetInstance()->CreateUniform(u._size, _descriptorSetLayout, u._layout);

			u._us.push_back(uniform);
		}

		///////////// TEXTURE ////////////
		for (UniformsTexture &u : _uT)
		{
			Image img;

			Texture *texture = mesh->_material.GetTexture(u._uV.name);

			if (!texture)
				texture = Soon::RessourceImporter::GetSingleton().Load<Texture2D>("../Ressources/Textures/white.png");

			u._iR.push_back(GraphicsInstance::GetInstance()->CreateTextureImage(texture->_width, texture->_height, texture->_data, static_cast<int32_t>(texture->_tType), static_cast<int32_t>(texture->_format)));
			img._textureSampler = GraphicsInstance::GetInstance()->CreateTextureSampler();
			img._imageView = GraphicsInstance::GetInstance()->CreateImageView(_imagesRenderer.back()._textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
			u._i.push_back(img);

			std::vector<VkDescriptorSet> imageUniform = GraphicsInstance::GetInstance()->CreateImageDescriptorSets(img._imageView, img._textureSampler, _descriptorSetLayout[2]);
			u._dS.push_back(imageUniform);
		}*/
	}

/*
	void BindCaller(VkCommandBuffer commandBuffer, uint32_t index)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicPipeline);

		VkDeviceSize offsets[] = {0};

		uint32_t j = 0;
		for (auto &buf : _gpuBuffers)
		{
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buf, offsets);

			vkCmdBindIndexBuffer(commandBuffer, _indexBuffers.at(j)._Buffer[0], 0, VK_INDEX_TYPE_UINT32);

			///////////// UNIFORM /////////////
			for (Uniforms &u : _u)
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, u._layout, 1, &u._us._dS.at(j).at(index), 0, nullptr);
			///////////// TEXTURE ////////////
			for (UniformsTexture &u : _uT)
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, u._layout, 1, &u._dS.at(j).at(index), 0, nullptr);

			vkCmdDrawIndexed(commandBuffer, _indexSize.at(j), 1, 0, 0, 0);
			++j;
		}
	}
	*/
};
} // namespace Soon
