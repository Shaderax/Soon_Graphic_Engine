#pragma once

#include "Pipelines/PipelineConf.hpp"

namespace Soon
{
	GenericProperty* CullModeToVk(std::string name)
	{
		static std::unordered_map<std::string, VkCullModeFlagBits> cullModeMap = 
		{
			{"NONE", VK_CULL_MODE_NONE},
			{"BACK", VK_CULL_MODE_BACK_BIT},
			{"FRONT", VK_CULL_MODE_FRONT_BIT},
			{"FRONT_AND_BACK", VK_CULL_MODE_FRONT_AND_BACK}
		};

		PropertyWrapper<VkCullModeFlagBits>* prop = new PropertyWrapper<VkCullModeFlagBits>();

		prop->Set(cullModeMap.at(name));

		return (dynamic_cast<GenericProperty*>(prop));
	}

	GenericProperty* FrontFaceToVk(std::string name)
	{
		static std::unordered_map<std::string, VkFrontFace> frontFaceMap = 
		{
			{"COUNTER_CLOCKWISE", VK_FRONT_FACE_COUNTER_CLOCKWISE},
			{"CLOCKWISE", VK_FRONT_FACE_CLOCKWISE}
		};

		PropertyWrapper<VkFrontFace>* prop = new PropertyWrapper<VkFrontFace>();

		prop->Set(frontFaceMap.at(name));

		return (dynamic_cast<GenericProperty*>(prop));
	}

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
