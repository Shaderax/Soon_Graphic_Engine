#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "GraphicsInstance.hpp"
#include "GraphicPipelineConf.hpp"

namespace Soon
{
	GraphicPipelineConf::GraphicPipelineConf(void) : PipelineConf(EPipelineType::GRAPHIC)
	{
		m_Properties["CullMode"] = {&CullModeToVk, &rasterizer.cullMode, sizeof(rasterizer.cullMode)};
		m_Properties["FrontFace"] = {&FrontFaceToVk, &rasterizer.frontFace, sizeof(rasterizer.frontFace)};

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkExtent2D Extent = GraphicsInstance::GetInstance()->GetSwapChainExtent();
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)Extent.width;
		viewport.height = (float)Extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		scissor.offset = {0, 0};
		scissor.extent = Extent;

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;

		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE; //VK_FRONT_FACE_COUNTER_CLOCKWISE;//
		rasterizer.depthBiasEnable = VK_FALSE;

		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		//		depthStencil.minDepthBounds = 0.0f; // Optional
		//		depthStencil.maxDepthBounds = 1.0f; // Optional
		depthStencil.stencilTestEnable = VK_FALSE;
		//		depthStencil.front = {}; // Optional
		//		depthStencil.back = {}; // Optional

		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		/////////// PIPELINE LAYOUT ////////////

		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.renderPass = GraphicsInstance::GetInstance()->GetRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		//		pipelineInfo.layout = pipelineLayout;
	}

	bool GraphicPipelineConf::IsMeshDefaultVertexInput(std::string name)
	{
		for (uint32_t index = 0; index < m_MeshDefaultVertexInput.size(); index++)
		{
			if (m_MeshDefaultVertexInput[index].inputName == name)
				return (true);
		}
		return (false);
	}

	bool GraphicPipelineConf::IsDefaultVertexInput(std::string name)
	{
		for (uint32_t index = 0; index < m_DefaultInputBindings.size(); index++)
		{
			for (auto const& input : m_DefaultInputBindings)
			{
				if (std::find(input.mInputName.begin(), input.mInputName.end(), name) != input.mInputName.end())
					return (true);
			}
		}
		return (false);
	}

	const DefaultInputBinding& GraphicPipelineConf::GetDefaultVertexInput(std::string name) const
	{
		// TODO: Binding Can't be 0
		for (auto const& input : m_DefaultInputBindings)
		{
			if (std::find(input.mInputName.begin(), input.mInputName.end(), name) != input.mInputName.end())
				return (input);
		}
		//__FILE__, __LINE__, __FUNCTION__;
		throw std::runtime_error(name + std::string(" Not a DefaultVertexInput"));
	}
	
	const DefaultMeshVertexInput& GraphicPipelineConf::GetMeshDefaultVertexInput(std::string name) const
	{
		for (uint32_t index = 0; index < m_MeshDefaultVertexInput.size(); index++)
		{
			if (m_MeshDefaultVertexInput[index].inputName == name)
				return (m_MeshDefaultVertexInput[index]);
		}
		throw std::runtime_error(name + std::string(" Not a DefaultMeshVertexInput"));
	}

	void GraphicPipelineConf::SetBindingInputRate( uint32_t binding, VkVertexInputRate inputRate)
	{
		for (auto& input : m_DefaultInputBindings)
		{
			if (input.mBinding == binding)
			{
				input.mInputRate = inputRate;
				return ;
			}
		}
	}

	void GraphicPipelineConf::AddInputBindings(std::string name, uint32_t binding)
	{
		if (binding == 0)
			throw std::runtime_error(name + " at Binding : can't be 0");

		auto it = std::find_if(m_DefaultInputBindings.begin(), m_DefaultInputBindings.end(), [&binding](DefaultInputBinding& v) { return v.mBinding == binding; });

		if (it == m_DefaultInputBindings.end())
		{
			DefaultInputBinding input;
			input.mBinding = binding;
			input.mInputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			input.mInputName.push_back(name);
			m_DefaultInputBindings.push_back(input);
		}
		else
			it->mInputName.push_back(name);
	}

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

} // namespace Soon
