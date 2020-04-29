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
	private:
public:
	std::string _pathVert;
	std::string _pathFrag;
	static const PipelineType _type = PipelineType::GRAPHIC;

	ShaderPipeline()
	{
	}

	~ShaderPipeline()
	{
		//if (_graphicPipeline != nullptr)
		//	vkDestroyPipeline(GraphicsInstance::GetInstance()->GetDevice(), _graphicPipeline, nullptr);
	}

	void Init( void )
	{
		assert(!_pathVert.empty() && !_pathFrag.empty() && "Vertex Path or Frag Path not feed");

		GetShaderData(_pathVert);
		GetShaderData(_pathFrag);

		GetBindingDescription();
		GetAttributeDescriptions();

		_conf.vertexInputInfo.vertexBindingDescriptionCount = 1;
		_conf.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributeDescriptions.size());
		_conf.vertexInputInfo.pVertexBindingDescriptions = &_bindingDescription;
		_conf.vertexInputInfo.pVertexAttributeDescriptions = _attributeDescriptions.data();

		_descriptorSetLayout = GraphicsInstance::GetInstance()->CreateDescriptorSetLayout( uboLayoutBinding );
		_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_descriptorSetLayout);
		_conf.pipelineInfo.layout = _pipelineLayout;
		_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
				_conf,
				_pathVert,
				_pathFrag);

		// Alloc Camera
	}

	void RecreatePipeline(void)
	{
		_conf.pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
		_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
				_conf,
				_pathVert,
				_pathFrag);
	}

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
