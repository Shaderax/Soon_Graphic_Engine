#include <vector>
#include "Utilities/Error.hpp"

namespace Soon
{
	class BasePipeline;

	class SubPass
	{
	public:
		std::vector<BasePipeline> mPipelines;
		VkSubpassDescription mSubpassDesc;
		uint32_t mRenderPassId = Soon::IdError;
		std::vector<VkAttachmentDescription> mAttachements;
		std::vector<VkSubpassDependency> mDependencies = {};
	};
} // namespace Soon