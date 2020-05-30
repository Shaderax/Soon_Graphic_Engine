#pragma once

#include "Pipelines/BasePipeline.hpp"

#include <vector>
#include <unordered_map>

namespace Soon
{
	class Mesh;

	/*
	 * In material ptr or raw data ?
	*/

	enum class ECullMode : uint8_t
	{
		NONE = 0,
    	FRONT = 1,
    	BACK = 2,
    	FRONT_AND_BACK = 3
	};

	VkCullModeFlagBits CullModeToVk(ECullMode cull)
	{
		switch (cull)
		{
		case ECullMode::NONE:
			return VK_CULL_MODE_NONE;
		case ECullMode::BACK:
			return VK_CULL_MODE_BACK_BIT;
		case ECullMode::FRONT:
			return VK_CULL_MODE_FRONT_BIT;
		case ECullMode::FRONT_AND_BACK:
			return VK_CULL_MODE_FRONT_AND_BACK;
		default:
			return VK_CULL_MODE_NONE;
		}
		return VK_CULL_MODE_NONE;
	}

	struct ShaderProperty
	{
		void* data = nullptr;
		uint32_t size = 0;
	};

	class ShaderPipeline : public BasePipeline
	{
	private:
		std::unordered_map<std::string, ShaderProperty>	m_Properties;
		using PropertiesIterator = std::unordered_map<std::string, ShaderProperty>::iterator;
	public:
		std::string _pathVert;
		std::string _pathFrag;
		static const PipelineType _type = PipelineType::GRAPHIC;

		void SetShaderProperties(std::string name, void* data)
		{
			PropertiesIterator prop = m_Properties.find(name);
			if (prop == m_Properties.end())
				return ;
			memcpy(prop->second.data, data, prop->second.size);
		}

		ShaderPipeline()
		{
			m_Properties["CullMode"] = {&_conf.rasterizer.cullMode, sizeof(_conf.rasterizer.cullMode)};
			m_Properties["Topology"] = {&_conf.inputAssembly.topology, sizeof(_conf.inputAssembly.topology)};
			m_Properties["PolygoneMode"] = {&_conf.rasterizer.polygonMode, sizeof(_conf.rasterizer.polygonMode)};
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

			_conf.vertexInputInfo.vertexBindingDescriptionCount = 1;
			_conf.vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_attributeDescriptions.size());
			_conf.vertexInputInfo.pVertexBindingDescriptions = &_bindingDescription;
			_conf.vertexInputInfo.pVertexAttributeDescriptions = _attributeDescriptions.data();

			_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.CreateDescriptorSetLayout());
			_conf.pipelineInfo.layout = _pipelineLayout;
			_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
				_conf,
				_pathVert,
				_pathFrag);

			_mUbm.InitBuffers();

			// Alloc Camera
		}

		// TODO: DESTROY _mUbm.CreateDescriptorSetLayout()
		void RecreatePipeline(void)
		{
			VkExtent2D Extent = GraphicsInstance::GetInstance()->GetSwapChainExtent();
			_conf.viewport.width = (float)Extent.width;
			_conf.viewport.height = (float)Extent.height;

			_conf.scissor.extent = Extent;

			_conf.pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
			_pipelineLayout = GraphicsInstance::GetInstance()->CreatePipelineLayout(_mUbm.GetDescriptorSetLayout());
			_conf.pipelineInfo.layout = _pipelineLayout;
			_graphicPipeline = GraphicsInstance::GetInstance()->CreateGraphicsPipeline(
				_conf,
				_pathVert,
				_pathFrag);
			//TODO: Change Cull Mode
		}
	};
} // namespace Soon
