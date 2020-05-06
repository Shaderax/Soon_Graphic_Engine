#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string>

#include "PipelineConf.hpp"

#include "VkMemoryAllocator/vk_mem_alloc.h"

enum DescriptorTypeLayout
{
	CAMERA = 0,
	MODEL = 1,
	IMAGE = 2,
	MATERIAL = 3,
	LIGHT = 4
};


//Swap chain struct
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct BufferRenderer
{
	std::vector<VkBuffer>		buffer;
	std::vector<VkDeviceMemory>	bufferMemory;
};

struct VertexBufferRenderer
{
	VkBuffer		buffer;
	VkDeviceMemory	bufferMemory;
};

struct MeshBufferRenderer
{
	VertexBufferRenderer vertex;
	VertexBufferRenderer indices;
};

struct ImageRenderer
{
	VkImage			_textureImage;
	VkDeviceMemory	_textureImageMemory;
};

struct Image
{
	VkSampler _textureSampler;
	VkImageView _imageView;
};

struct UniformSets
{
	std::vector<VkDescriptorSet>	_descriptorSets;
	BufferRenderer			_uniformRender;
};

namespace Soon
{
	class GraphicsInstance
	{
		public:
			struct WindowAttribute
			{
				uint32_t	_width;
				uint32_t	_height;
				bool		_fullscreen;
				bool		_vsync;
				std::string	_name;

				WindowAttribute( int width = 1280, int height = 720, bool fullscreen = false, bool vsync = false, std::string name = "New Project" ) :
					_width(width),
					_height(height),
					_fullscreen(fullscreen),
					_vsync(vsync),
					_name(name)
				{
				}
			};
		private:

			static GraphicsInstance*				_instance;
			WindowAttribute						_windowAttribute;
			GLFWwindow*						_window;
			VkInstance						_vulkanInstance;
			VkPhysicalDevice				_physicalDevice;
			VkDevice 						_device;
			VkQueue							_graphicsQueue;
			VkSurfaceKHR 					_surface;
			VkQueue							_presentQueue;
			VkSwapchainKHR					_swapChain;
			std::vector<VkImage>			_swapChainImages;
			VkExtent2D						_swapChainExtent;
			VkFormat						_swapChainImageFormat;
			VkDebugUtilsMessengerEXT		_debugMessenger;
			std::vector<VkImageView>		_swapChainImageViews;
//			std::vector<VkDescriptorSetLayout>			_descriptorSetLayout;
//			VkPipelineLayout 				_pipelineLayout;
//			VkPipeline						_graphicsPipeline;
			VkRenderPass					_renderPass;
			std::vector<VkFramebuffer> 		_swapChainFramebuffers;
			VkCommandPool 					_commandPool;
			std::vector<VkCommandBuffer>	_commandBuffers;
			std::vector<VkSemaphore>		_imageAvailableSemaphores;
			std::vector<VkSemaphore>		_renderFinishedSemaphores;
			std::vector<VkFence>			_inFlightFences;
			size_t							_currentFrame = 0;
			uint32_t						_nextSwapChainImageIdx;
			bool 							_framebufferResized = false;
			VkDescriptorPool				_descriptorPool;

			VkImage							_depthImage;
			VkDeviceMemory					_depthImageMemory;
			VkImageView						_depthImageView;

			VmaAllocator					_allocator;
		public:
			enum class ShaderType
			{
				VERTEX_FRAGMENT,
				COMPUTE
			};

			static GraphicsInstance* GetInstance( void );
			static void ReleaseInstance( void );
			GraphicsInstance( void );
			~GraphicsInstance( void );
			void Initialize( void );
			GLFWwindow* GetWindow( void );
			WindowAttribute GetWindowAttribute( void );

			void	CreateInstance( void );
			void	PickPhysicalDevice( void );
			void	CreateLogicalDevice( void );
			void	CreateWindow( void );
			void	CreateSurface( void );
			void	GetPhysicalDeviceInfo( void );
			void	GetPhysicalDeviceInfo(VkPhysicalDevice device);
			int		GetQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlagBits queue);
			int		RateDeviceSuitable(VkPhysicalDevice device);
			SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
			void	CreateSwapChain( void );
			void 	SetupDebugMessenger( void );
			void 	CreateImageViews( void );
			VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType );
			std::vector<VkDescriptorSetLayout> CreateDescriptorSetLayout( std::vector<std::vector<VkDescriptorSetLayoutBinding>> uboLayoutBinding );
			VkPipeline CreateGraphicsPipeline(
				GraphicsPipelineConf&						conf,
				std::string 							FirstPath,
				std::string							SecondPath);
			VkPipeline CreateComputePipeline(
				VkPipelineLayout                                                pipelineLayout,
				std::string                                                     pathCompute);
			VkShaderModule CreateShaderModule(const std::vector<char>& code);
			void	CreateRenderPass( void );
			void 	CreateFramebuffers( void );
			void 	CreateCommandPool( void );
			void 	CreateCommandBuffers( void );
			void 	CreateSyncObjects( void );
			void 	DrawFrame( void );
			void 	RecreateSwapChain( void );
			void 	CleanupSwapChain( void );
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
			static void FramebufferResizeCallback(GLFWwindow *window, int width, int height);
			VertexBufferRenderer CreateVertexBuffer( uint8_t* ptrData, uint32_t size );
			VertexBufferRenderer CreateStorageBuffer( void* ptrData, uint32_t size );
			uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
			void	FillCommandBuffer( void );
			void 	CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			BufferRenderer CreateUniformBuffers( size_t size );
			void	UpdateUniformBuffer(uint32_t currentImage);
			void 	CreateDescriptorPool( void );
			std::vector<VkDescriptorSet> CreateImageDescriptorSets( VkImageView textureImageView, VkSampler textureSampler, VkDescriptorSetLayout descriptorSetLayout );
			ImageRenderer	CreateTextureImage( uint32_t width, uint32_t height, void* textureData, uint8_t layer, uint8_t pixelSize );
			void 	CreateImage( uint32_t width, uint32_t height, uint8_t layer, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
			VkCommandBuffer BeginSingleTimeCommands( void );
			void	EndSingleTimeCommands(VkCommandBuffer commandBuffer);
			void 	TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageViewType vType);
			void 	CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkImageViewType vType);
			VkSampler	CreateTextureSampler( void );
//			void	CreateTextureImageView( void );
			VertexBufferRenderer CreateIndexBuffer( uint32_t* indexData, uint32_t size);
			void CopyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size );
			VkFormat FindDepthFormat( void );
			VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
			void CreateDepthResources( void );
			bool HasStencilComponent(VkFormat format);
			VkDevice GetDevice( void );
			void PollEvent( void );
			bool ShouldClose( GLFWwindow* window );
			uint32_t GetNextIdImageToRender( void );
			uint32_t GetSwapChainSize( void );


			VkExtent2D GetSwapChainExtent( void );//						_swapChainExtent;
			VkRenderPass GetRenderPass( void ); //					_renderPass;

			VkPipelineLayout CreatePipelineLayout( std::vector<VkDescriptorSetLayout> descriptorSetLayout );
			UniformSets CreateUniform( size_t size, std::vector<VkDescriptorSetLayout> layoutArray, int dlayout );
			//std::vector<VkDescriptorSet> CreateDescriptorSets( size_t size, std::vector<VkDescriptorSetLayout> layoutArray, int dlayout, VkBuffer* gpuBuffers,  VkDescriptorType dType);
			std::vector<VkDescriptorSet> CreateDescriptorSets( size_t size, uint32_t binding, VkDescriptorSetLayout layout, VkBuffer* gpuBuffers, VkDescriptorType dType);


	void CreateAllocator( void );
	void DestroyAllocator( void );
	};
}
