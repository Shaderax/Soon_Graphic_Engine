#pragma once

#include "Pipelines/PipelineConf.hpp"

#include "Vertex.hpp"
#include "MeshVertex.hpp"

#include <list>

namespace Soon
{
	GenericProperty* CullModeToVk(std::string name);
	GenericProperty* FrontFaceToVk(std::string name);

	class GraphicPipelineConf : public PipelineConf
	{
	private:
	public:
		std::list<DefaultInputBinding>	m_DefaultInputBindings;
		std::vector<DefaultMeshVertexInput>	m_MeshDefaultVertexInput = 
		{
			{"inPosition", EnumVertexElementSementic::POSITION},
			{"inNormal", EnumVertexElementSementic::NORMAL},
			{"inTexCoord", EnumVertexElementSementic::TEXCOORD}
		};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		VkViewport viewport = {};
		VkRect2D scissor = {};
		VkPipelineViewportStateCreateInfo viewportState = {};
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		VkGraphicsPipelineCreateInfo pipelineInfo = {};

		GraphicPipelineConf(void);

		void AddInputBindings(std::string name, uint32_t binding);
		void SetBindingInputRate( uint32_t binding, VkVertexInputRate inputRate);

		bool IsDefaultVertexInput(std::string name);
		bool IsMeshDefaultVertexInput(std::string name);

		const DefaultInputBinding& GetDefaultVertexInput(std::string name) const;
		const DefaultMeshVertexInput& GetMeshDefaultVertexInput(std::string name) const;
	};
} // namespace Soon
