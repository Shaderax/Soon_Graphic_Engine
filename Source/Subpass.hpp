#include <vector>
#include "Utilities/Error.hpp"

namespace Soon
{
	class BasePipeline;

	class Subpass
	{
	public:
		std::vector<BasePipeline> mPipelines;
		VkSubpassDescription mSubpassDesc;
		uint32_t mRenderPassId = Soon::IdError;
		std::vector<VkAttachmentDescription> mAttachements;
		std::vector<VkSubpassDependency> mDependencies = {};
	private:
		ObjectsManager<BasePipeline> pipelines;
	};
} // namespace Soon


namespace std
{
	template<>
	struct hash<Soon::Subpass>
	{
		std::size_t operator()(const Soon::Subpass& inSubpass) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			return ((hash<string>()(inSubpass.first)
					^ (hash<string>()(inSubpass.second) << 1)) >> 1)
					^ (hash<int>()(inSubpass.third) << 1);
    }
		}
	};
}
