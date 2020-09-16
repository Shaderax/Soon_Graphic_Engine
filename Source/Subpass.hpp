#include <vector>

namespace Soon
{
	class BasePipeline;
	class SubPass
	{
	public:
		std::vector<BasePipeline> pip;
	};
} // namespace Soon