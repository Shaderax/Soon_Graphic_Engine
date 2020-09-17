#include <vector>
#include "Subpass.hpp"

namespace Soon
{
	class RenderPass
	{
	public:
		std::vector<SubPass> subp;
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		// VkRenderPass 
		// FrameBuffer
	};
} // namespace Soon