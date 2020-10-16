#include <vector>
#include "Subpass.hpp"

#include <vulkan/vulkan.h>

bool operator==(const VkAttachmentDescription& lhs, const VkAttachmentDescription& rhs)
{
	return lhs.flags == rhs.flags &&
			lhs.format == rhs.format &&
			lhs.samples == rhs.samples &&
			lhs.loadOp == rhs.loadOp &&
			lhs.storeOp == rhs.storeOp &&
			lhs.stencilLoadOp == rhs.stencilLoadOp &&
			lhs.stencilStoreOp == rhs.stencilStoreOp &&
			lhs.initialLayout == rhs.initialLayout &&
			lhs.finalLayout == rhs.finalLayout;
}

bool operator==(const VkSubpassDependency& lhs, const VkSubpassDependency& rhs)
{
	return lhs.srcSubpass == rhs.srcSubpass &&
			lhs.dstSubpass == rhs.dstSubpass &&
			lhs.srcStageMask == rhs.srcStageMask &&
			lhs.srcAccessMask == rhs.srcAccessMask &&
			lhs.dstStageMask == rhs.dstStageMask &&
			lhs.dstAccessMask == rhs.dstAccessMask;
}

namespace Soon
{
	class RenderPass
	{
	public:
		std::vector<SubPass> mSubpass;
		VkRenderPass mRenderPass;
		VkFramebuffer mFrameBuffer = VK_NULL_HANDLE;
		VkAttachmentDescription mDepthAttachement;

		//bool operator==(const RenderPass& other)
		//{
		//	//TODO:
		//	return subp.size() == other.subp.size() && depth_attachement == other.depth_attachement;
		//}

		/*
			if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
				throw std::runtime_error("failed to create render pass!");
			Pour les FrameBuffers ?
				C'est propre a rien ? genre ca peut utiliser les memes sur plusieurs renderpass ? -> La raison de pourquoi j'utilise plusieurs renderpass ?
				Genre dans le cas d'une shadow map je peux pas utilisé la meme car les depency et tout sont pas les memes.
				Mais du coup une framebuffer envoyé, Shadow Map -> Normal render. Donc framebuffer ne sont pas propre a une renderpass.
		*/
	};
} // namespace Soon

namespace std
{
	template<>
	struct hash<Soon::RenderPass>
	{
		std::size_t operator()(const Soon::RenderPass& pass) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			return ((hash<string>()(pass.first)
					^ (hash<string>()(pass.second) << 1)) >> 1)
					^ (hash<int>()(pass.third) << 1);
    }
		}
	};
}
