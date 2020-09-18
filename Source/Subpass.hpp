#include <vector>

namespace Soon
{
	class BasePipeline;
	class SubPass
	{
	public:
		std::vector<BasePipeline> pip;
		std::vector<VkAttachmentDescription> i;
		std::vector<VkAttachmentReference> j;
		std::vector<VkSubpassDependency> dep;
		VkSubpassDescription subpassDesc;
	};
} // namespace Soon