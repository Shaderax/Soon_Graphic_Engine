#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "PipelineConf.hpp"

#include "VkMemoryAllocator/vk_mem_alloc.h"

#include "Texture.hpp"

//Swap chain struct
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct BufferRenderer
{
	std::vector<VkBuffer> buffer;
	std::vector<VmaAllocation> bufferMemory;
};

struct VertexBufferRenderer
{
	VkBuffer buffer;
	VmaAllocation bufferMemory;
};

struct IndiceBufferRenderer
{
	VkBuffer buffer;
	VmaAllocation bufferMemory;
	std::uint32_t numIndices;
};

struct MeshBufferRenderer
{
	VertexBufferRenderer vertex;
	IndiceBufferRenderer indices;
};

struct ImageRenderer
{
	VkImage _textureImage;
	VmaAllocation _textureImageMemory;
};

struct ImageProperties
{
	VkSampler _textureSampler;
	VkImageView _imageView;
};

struct UniformSets
{
	std::vector<VkDescriptorSet> _descriptorSets;
	BufferRenderer _uniformRender;
};

namespace Soon
{
	class Texture;

	class GraphicsInstance
	{
	public:
		struct WindowAttribute
		{
			uint32_t _width;
			uint32_t _height;
			bool _fullscreen;
			bool _vsync;
			std::string _name;

			WindowAttribute(int width = 1280, int height = 720, bool fullscreen = false, bool vsync = false, std::string name = "New Project") : _width(width),
																																				 _height(height),
																																				 _fullscreen(fullscreen),
																																				 _vsync(vsync),
																																				 _name(name)
			{
			}
		};

	private:
		static GraphicsInstance *_instance;
		WindowAttribute _windowAttribute;
		GLFWwindow *_window;
		VkInstance _vulkanInstance;
		VkPhysicalDevice _physicalDevice;
		VkDevice _device;
		VkQueue _graphicsQueue;
		VkSurfaceKHR _surface;
		VkQueue _presentQueue;
		VkSwapchainKHR _swapChain;
		std::vector<VkImage> _swapChainImages;
		VkExtent2D _swapChainExtent;
		VkFormat _swapChainImageFormat;
		VkDebugUtilsMessengerEXT _debugMessenger;
		std::vector<VkImageView> _swapChainImageViews;
		VkRenderPass _renderPass;
		std::vector<VkFramebuffer> _swapChainFramebuffers;
		VkCommandPool _commandPool;
		std::vector<VkCommandBuffer> _commandBuffers;
		std::vector<VkSemaphore> _imageAvailableSemaphores;
		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<VkFence> _inFlightFences;
		size_t _currentFrame = 0;
		uint32_t _nextSwapChainImageIdx = 0;
		bool _framebufferResized = false;
		VkDescriptorPool _descriptorPool;

		VkImage _depthImage;
		VmaAllocation _depthImageMemory;
		VkImageView _depthImageView;

		VmaAllocator m_Allocator;

	public:
		enum class ShaderType
		{
			VERTEX_FRAGMENT,
			COMPUTE
		};

		static GraphicsInstance *GetInstance(void);
		static void ReleaseInstance(void);
		GraphicsInstance(void);
		~GraphicsInstance(void);
		void Initialize(void);

		GLFWwindow *GetWindow(void);
		WindowAttribute GetWindowAttribute(void);

		void PickPhysicalDevice(void);
		void CreateLogicalDevice(void);
		int RateDeviceSuitable(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		void CreateSwapChain(void);
		void SetupDebugMessenger(void);
		void CreateImageViews(void);
		VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType);
		std::vector<VkDescriptorSetLayout> CreateDescriptorSetLayout(std::vector<std::vector<VkDescriptorSetLayoutBinding>> uboLayoutBinding);
		VkPipeline CreateGraphicsPipeline(
			GraphicsPipelineConf &conf,
			std::string FirstPath,
			std::string SecondPath);
		VkPipeline CreateComputePipeline(
			VkPipelineLayout pipelineLayout,
			std::string pathCompute);
		VkShaderModule CreateShaderModule(const std::vector<char> &code);
		void DrawFrame(void);
		void RecreateSwapChain(void);
		void CleanupSwapChain(void);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
		static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void FillCommandBuffer(void);

		void CreateInstance(void);
		void CreateWindow(void);
		void CreateSurface(void);
		void CreateRenderPass(void);
		void CreateFramebuffers(void);
		void CreateCommandPool(void);
		void CreateCommandBuffers(void);
		void CreateSyncObjects(void);
		void CreateDescriptorPool(void);
		VkSampler CreateTextureSampler(Texture* texture);
		BufferRenderer CreateUniformBuffers(size_t size);
		VkPipelineLayout CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayout);
		VertexBufferRenderer CreateStorageBuffer(void *ptrData, uint32_t size);
		VertexBufferRenderer CreateVertexBuffer(uint8_t *ptrData, uint32_t size);
		UniformSets CreateUniform(size_t size, std::vector<VkDescriptorSetLayout> layoutArray, int dlayout);
		IndiceBufferRenderer CreateIndexBuffer(uint32_t *indexData, uint32_t size);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkBuffer &buffer, VmaAllocation &allocation);
		std::vector<VkDescriptorSet> CreateImageDescriptorSets(VkImageView textureImageView, VkSampler textureSampler, VkDescriptorSetLayout layout, uint32_t binding);
		ImageRenderer CreateTextureImage(Texture* texture);
		void CreateImage(VkExtent2D textureExtent, VkFormat format, uint32_t layer, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage properties, VkImage &image, VmaAllocation &imageMemory, VmaAllocationCreateFlags flag = 0);
		std::vector<VkDescriptorSet> CreateDescriptorSets(size_t size, uint32_t binding, uint32_t offset, VkDescriptorSetLayout layout, VkBuffer *gpuBuffers, uint32_t bufferCount);
		void DestroyDescriptorSet(VkDescriptorSet descriptor);
		void CreateAllocator(void);

		void UpdateUniformBuffer(uint32_t currentImage);
		VkCommandBuffer BeginSingleTimeCommands(void);
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageViewType vType);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkImageViewType vType);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		VkFormat FindDepthFormat(void);
		VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		void CreateDepthResources(void);
		bool HasStencilComponent(VkFormat format);
		void PollEvent(void);
		bool ShouldClose(GLFWwindow *window);
		void DestroySwapChain( void );

		void GetPhysicalDeviceInfo(void);
		void GetPhysicalDeviceInfo(VkPhysicalDevice device);
		int GetQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlagBits queue);
		uint32_t GetNextIdImageToRender(void);
		uint32_t GetSwapChainSize(void);
		VkDevice GetDevice(void);
		VkExtent2D GetSwapChainExtent(void); //					_swapChainExtent;
		VkRenderPass GetRenderPass(void);	 //					_renderPass;
		VmaAllocator &GetAllocator(void);

		void DestroyAllocator(void);
	};
	VkFormat TextureFormatToVkFormat( TextureFormat format );
	VkImageViewType TextureTypeToVkImageType( EnumTextureType type );
	VkFilter TextureFilterModeToVk( EnumFilterMode filterMode );

} // namespace Soon