#include <vector>
#include "Subpass.hpp"

#include <vulkan/vulkan.h>

namespace Soon
{
	class RenderPass
	{
	public:
		std::vector<SubPass> subp;

		VkRenderPass m_RenderPass;

		VkFramebuffer m_FrameBuffer;
	};
} // namespace Soon