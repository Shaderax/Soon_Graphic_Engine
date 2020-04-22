#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Soon
{
	class GraphicsPipelineConf
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

			GraphicsPipelineConf( void );
	};
}
