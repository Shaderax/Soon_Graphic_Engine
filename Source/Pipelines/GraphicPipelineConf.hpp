#pragma once

#include "Pipelines/PipelineConf.hpp"

namespace Soon
{
	GenericProperty* CullModeToVk(std::string name);
	GenericProperty* FrontFaceToVk(std::string name);

	class GraphicPipelineConf : public PipelineConf
	{
	public:
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
	};
} // namespace Soon
