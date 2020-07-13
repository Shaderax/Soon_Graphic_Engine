#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#define VMA_IMPLEMENTATION
#include "GraphicsInstance.hpp"
#include "GraphicsRenderer.hpp"

#include "Utilities/ReadFile.hpp"

#include "Texture.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>
#include <string.h>
#include <string>

#define MAX_FRAMES_IN_FLIGHT 2

#include "GLFWInit/Init.hpp"
#include "GLFWInit/Hints.hpp"

#include "Pipelines/PipelineConf.hpp"
#include "Pipelines/GraphicPipelineConf.hpp"
#include "Pipelines/ComputePipelineConf.hpp"

const std::vector<const char *> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"};

const std::vector<const char *> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
// Validation layer : 
	const bool enableValidationLayers = true;
#endif

bool checkValidationLayerSupport(void)
{
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	std::cout << "num availableLayers : " << layerCount << std::endl;
	for (const auto &layerProperties : availableLayers)
	{
		std::cout << "\t" << layerProperties.layerName << std::endl;
	}

	for (const char *layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}
	return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
	void *pUserData)
{

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
		func(instance, debugMessenger, pAllocator);
}

namespace Soon
{
	/**
	 * UNIFORMS
	 */
	uint32_t GetUniformRuntimeSize(UniformRuntime& uniform, uint32_t id)
	{
		uint32_t size = 0;
		for (UniformRuntimeVar& member : uniform.mMembers)
		{
			if (!member.isRuntime)
				size += member._size;
			else
				size += member._size * member.numInBuffer[id];
		}
		return size;
	}

	const UniformRuntimeVar& FindUniformRuntimeVar(const std::vector<UniformRuntimeVar>& var, std::string name, uint32_t* offset)
	{
		size_t pos = name.find('.');

		for (uint32_t index = 0; index < var.size() ; index++)
		{
			if (var[index]._name == name.substr(0, pos))
			{
				if (pos == std::string::npos)
					return var[index];
				else
					return FindUniformRuntimeVar(var[index].mMembers, name.substr(pos + 1), offset);
			}
			*offset += var[index]._size; // * numIn ?
		}
		throw std::runtime_error("Uniform Not found");
	}

	VertexDescription UniformRuntime::GetVertexDescription(std::vector<std::string> varNames) const
	{
		VertexDescription description;
		uint32_t lastOffset = 0;
		uint32_t offset = 0;
		bool first = true;

		if (varNames.size() == 0)
			throw std::runtime_error("varNames is Empty");

		for (const std::string& var : varNames)
		{
			VertexElement element;

			offset = 0;
			const UniformRuntimeVar& runtime = FindUniformRuntimeVar(mMembers, var, &offset);
			element.mOffset = offset;
			element.type = runtime._type;
			if (!first && lastOffset >= offset)
				throw std::runtime_error("Wrong var order");
			lastOffset = offset;
			if (first)
				first = false;
		}
		return description;
	}

	/**
	 * GRAPHICS INSTANCE
	 */
	GraphicsInstance *GraphicsInstance::_instance = nullptr;

	GraphicsInstance *GraphicsInstance::GetInstance(void)
	{
		if (!_instance)
			_instance = new GraphicsInstance;

		return _instance;
	}

	void GraphicsInstance::ReleaseInstance(void)
	{
		if (_instance)
		{
			delete _instance;
			_instance = nullptr;
		}
	}

	GraphicsInstance::GraphicsInstance(void) : _physicalDevice(VK_NULL_HANDLE), _currentFrame(0), _framebufferResized(false)
	{
		std::cout << "In Constructor GrInstance" << std::endl;
		_window = nullptr;
	}

	GraphicsInstance::~GraphicsInstance(void)
	{
		vkDeviceWaitIdle(_device);
		DestroySwapChain();

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(_device, _inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(_device, _commandPool, nullptr);
		DestroyAllocator();
		vkDestroyDevice(_device, nullptr);
		if (enableValidationLayers)
			DestroyDebugUtilsMessengerEXT(_vulkanInstance, _debugMessenger, nullptr);
		vkDestroySurfaceKHR(_vulkanInstance, _surface, nullptr);
		vkDestroyInstance(_vulkanInstance, nullptr);
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void GraphicsInstance::PollEvent(void)
	{
		glfwPollEvents();
	}

	bool GraphicsInstance::ShouldClose(GLFWwindow *window)
	{
		return (glfwWindowShouldClose(window));
	}

	GraphicsInstance::WindowAttribute GraphicsInstance::GetWindowAttribute(void)
	{
		return (_windowAttribute);
	}

	VkPhysicalDeviceProperties GraphicsInstance::GetPhysicalDeviceInfo(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		//VkPhysicalDeviceFeatures deviceFeatures;
		//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		return (deviceProperties);
	}

	int GraphicsInstance::GetQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlagBits queue)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto &queueFamily : queueFamilies)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & queue && presentSupport)
				return i;

			i++;
		}
		return (-1);
	}

	bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto &extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}

	int GraphicsInstance::RateDeviceSuitable(VkPhysicalDevice device)
	{
		int score = 0;

		VkPhysicalDeviceProperties deviceProperties;
		//VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);
		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		if (GetQueueFamilyIndex(device, VK_QUEUE_GRAPHICS_BIT) == -1 || !extensionsSupported || !swapChainAdequate /* || supportedFeatures.samplerAnisotropy*/)
			return (score);
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 10;
		else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
			score += 5;
		return score;
	}

	void GraphicsInstance::PickPhysicalDevice(void)
	{
		std::map<int, VkPhysicalDevice> scoreDevice;

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, nullptr);

		if (deviceCount == 0)
			std::cout << "failed to find GPUs with Vulkan support!" << std::endl;

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_vulkanInstance, &deviceCount, devices.data());

		std::cout << "Devices : " << std::endl;
		for (const auto &device : devices)
		{
			int score = 0;

			score = RateDeviceSuitable(device);
			std::cout << "\tScore : " << score << " of Available device : ";
			std::cout << GetPhysicalDeviceInfo(device).deviceName << std::endl;
			scoreDevice.insert(std::make_pair(score, device));
		}

		if (scoreDevice.rbegin()->first > 0)
			_physicalDevice = scoreDevice.rbegin()->second;
		else
			std::cout << "failed to find a suitable GPU!" << std::endl;

		if (_physicalDevice == VK_NULL_HANDLE)
			std::cout << "failed to find a suitable GPU!" << std::endl;

		std::cout << "Device Chosen : ";
		std::cout << GetPhysicalDeviceInfo(_physicalDevice).deviceName << std::endl;
	}

	void GraphicsInstance::CreateWindow(void)
	{
		_window = glfwCreateWindow(_windowAttribute._width, _windowAttribute._height, _windowAttribute._name.c_str(), NULL, NULL);
		if (!_window)
		{
			std::cout << "Can't Initialize Window" << std::endl;
			exit(-1);
		}
		glfwSetWindowUserPointer(_window, this);
		glfwSetFramebufferSizeCallback(_window, FramebufferResizeCallback);
	}

	std::vector<const char *> GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char **glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		std::cout << "GLFW - EXTENSIONS : " << glfwExtensionCount << std::endl;
		for (size_t i = 0; i < glfwExtensionCount; ++i)
		{
			std::cout << "\t" << glfwExtensions[i] << std::endl;
		}

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void GraphicsInstance::CreateInstance(void)
	{
		if (enableValidationLayers && !checkValidationLayerSupport())
			throw std::runtime_error("validation layers requested, but not available!");

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = nullptr;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Soon Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// Check Exten
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "AVAILABLE EXTENSIONS : " << extensionCount << std::endl;
		std::vector<char const *> extNames(extensionCount);
		for (uint32_t i = 0; i < extensionCount; ++i)
		{
			extNames[i] = extensions[i].extensionName;
			std::cout << "\t" << extNames[i] << std::endl;
		}
		//// end Check Exten

		// Get  Required Extensions
		std::vector<const char *> vecExtensions = GetRequiredExtensions();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(vecExtensions.size());
		createInfo.ppEnabledExtensionNames = vecExtensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
			createInfo.enabledLayerCount = 0;

		VkResult result;
		if ((result = vkCreateInstance(&createInfo, nullptr, &_vulkanInstance)) != VK_SUCCESS)
			std::cout << "Can't Initialize a Vulkan Instance" << std::endl;
		;
	}

	void GraphicsInstance::CreateLogicalDevice(void)
	{
		// TODO //Get queue for drawing and queue for presentation
		int index = GetQueueFamilyIndex(_physicalDevice, VK_QUEUE_GRAPHICS_BIT);

		//		std::cout << index << std::endl;
		//		SOON_ERR_THROW((index != -1), "Did not find FamilyIndex");

		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = index;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		//TODO
		deviceFeatures.fillModeNonSolid = true;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
			std::cout << "failed to create logical device!" << std::endl;

		vkGetDeviceQueue(_device, index, 0, &_graphicsQueue);
		vkGetDeviceQueue(_device, index, 0, &_presentQueue);
	}

	void GraphicsInstance::CreateSurface(void)
	{
		VkResult ret;
		if ((ret = glfwCreateWindowSurface(_vulkanInstance, _window, nullptr, &_surface)) != VK_SUCCESS)
			std::cout << "failed to create window surface!" << std::endl;
	}

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
	{
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
			return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};

		for (const auto &availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return availableFormats[0];
	}

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto &availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				std::cout << "VK_PRESENT_MODE_MAILBOX_KHR" << std::endl;
				return availablePresentMode;
			}
			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				// TODO
				//std::cout << "VK_PRESENT_MODE_IMMEDIATE_KHR" << std::endl;
				//bestMode = availablePresentMode;
			}
		}

		std::cout << "VK_PRESENT_MODE_FIFO_KHR" << std::endl;
		return bestMode;
	}

	VkExtent2D GraphicsInstance::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		else
		{
			int width, height;
			glfwGetFramebufferSize(_window, &width, &height);
			VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}

	//SWAP CHAIN
	SwapChainSupportDetails GraphicsInstance::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	void GraphicsInstance::CreateSwapChain(void)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(_physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		//		std::cout << "Surface Format" << surfaceFormat.format << std::endl;
		//		std::cout << "Surface Color space : " << surfaceFormat.colorSpace << std::endl;
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		//		std::cout << "Present Mode : " << presentMode << std::endl;
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);
		std::cout << "Extent Width : " << extent.width << std::endl;
		std::cout << "Extent Height : " << extent.height << std::endl;

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;	  // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
			std::cout << "Cant Create SwapChain" << std::endl;

		vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
		_swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());

		_swapChainImageFormat = surfaceFormat.format;
		_swapChainExtent = extent;
	}

	void GraphicsInstance::SetupDebugMessenger(void)
	{
		if (!enableValidationLayers)
			return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;

		if (CreateDebugUtilsMessengerEXT(_vulkanInstance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
			throw std::runtime_error("failed to set up debug messenger!");
	}

	void GraphicsInstance::CreateImageViews(void)
	{
		_swapChainImageViews.resize(_swapChainImages.size());

		for (size_t i = 0; i < _swapChainImages.size(); i++)
			_swapChainImageViews[i] = CreateImageView(_swapChainImages[i], _swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D);
	}

	VkImageView GraphicsInstance::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType)
	{
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = viewType; //VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		//		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		if (viewType == VK_IMAGE_VIEW_TYPE_2D)
		{
			viewInfo.subresourceRange.layerCount = 1;
		}
		else if (viewType == VK_IMAGE_VIEW_TYPE_CUBE)
		{
			viewInfo.subresourceRange.layerCount = 6;
		}
		viewInfo.subresourceRange.aspectMask = aspectFlags;

		VkImageView imageView;
		if (vkCreateImageView(_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture image view!");

		return imageView;
	}

	VkShaderModule GraphicsInstance::CreateShaderModule(const std::vector<char> &code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

		VkShaderModule shaderModule;

		if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("failed to create shader module!");

		return shaderModule;
	}

	VkPipeline GraphicsInstance::CreatePipeline(PipelineConf* conf)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		std::vector<VkShaderModule> shaderModules;
		VkPipeline pipeline;

		for (PipelineStage stage : conf->GetStages())
		{
			auto shaderCode = ReadFile(stage.shaderPath);
			shaderModules.push_back(CreateShaderModule(shaderCode));

			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = PipelineStageToVk(stage.type);
			shaderStageInfo.module = shaderModules.back();
			shaderStageInfo.pName = "main";

			shaderStages.push_back(shaderStageInfo);
		}

		if (conf->GetType() == EPipelineType::GRAPHIC)
		{
			GraphicPipelineConf* graphicConf = reinterpret_cast<GraphicPipelineConf*>(conf);
			graphicConf->pipelineInfo.stageCount = shaderStages.size();
			graphicConf->pipelineInfo.pStages = shaderStages.data();

			if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &graphicConf->pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
				throw std::runtime_error("failed to create graphics pipeline!");
		}
		else if (conf->GetType() == EPipelineType::COMPUTE)
		{
			ComputePipelineConf* computeConf = reinterpret_cast<ComputePipelineConf*>(conf);
			computeConf->pipelineInfo.stage = shaderStages.back();

			if (vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &(computeConf->pipelineInfo), nullptr, &pipeline) != VK_SUCCESS)
				throw std::runtime_error("failed to create compute pipeline!");
		}

		for (VkShaderModule& shader : shaderModules)
			vkDestroyShaderModule(_device, shader, nullptr);

		return (pipeline);
	}

	VkPipelineLayout GraphicsInstance::CreatePipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayout)
	{
		VkPipelineLayout pipelineLayout;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = descriptorSetLayout.size();
		pipelineLayoutInfo.pSetLayouts = (descriptorSetLayout.data());
		//		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout!");

		return pipelineLayout;
	}

	void GraphicsInstance::CreateRenderPass(void)
	{
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = _swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachment = {};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
			throw std::runtime_error("failed to create render pass!");
		std::cout << "Created render pass" << std::endl;
	}

	void GraphicsInstance::CreateFramebuffers(void)
	{
		_swapChainFramebuffers.resize(_swapChainImageViews.size());

		for (size_t i = 0; i < _swapChainImageViews.size(); i++)
		{
			std::array<VkImageView, 2> attachments;
			attachments[0] = _swapChainImageViews[i],
			attachments[1] = _depthImageView;

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = _renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = _swapChainExtent.width;
			framebufferInfo.height = _swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void GraphicsInstance::CreateCommandPool(void)
	{
		int index = GetQueueFamilyIndex(_physicalDevice, VK_QUEUE_GRAPHICS_BIT);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = index;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create command pool!");
	}

	void GraphicsInstance::CreateCommandBuffers(void)
	{
		_commandBuffers.resize(_swapChainFramebuffers.size());

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = _commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

		if (vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate command buffers!");

		for (size_t i = 0; i < _commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			//
			if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS)
				throw std::runtime_error("failed to begin recording command buffer!");

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = _renderPass;
			renderPassInfo.framebuffer = _swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = _swapChainExtent;

			std::array<VkClearValue, 2> clearValues = {};
			clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdEndRenderPass(_commandBuffers[i]);
			if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to record command buffer!");
		}
	}

	void GraphicsInstance::FillCommandBuffer(void)
	{
		for (size_t i = 0; i < _commandBuffers.size(); i++)
			vkResetCommandBuffer(_commandBuffers[i], VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

		for (size_t i = 0; i < _commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			if (vkBeginCommandBuffer(_commandBuffers[i], &beginInfo) != VK_SUCCESS)
				throw std::runtime_error("failed to begin recording command buffer!");

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = _renderPass;
			renderPassInfo.framebuffer = _swapChainFramebuffers[i];
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = _swapChainExtent;

			std::array<VkClearValue, 2> clearValues = {};
			//clearValues[0].color = {0.87f, 0.12f, 0.3642f, 1.0f};
			clearValues[0].color = {0.32f, 0.07f, 0.75f, 1.0f};
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			GraphicsRenderer::GetInstance()->GraphicPipelinesBindCaller(_commandBuffers[i], i);

			vkCmdEndRenderPass(_commandBuffers[i]);

			GraphicsRenderer::GetInstance()->ComputePipelinesBindCaller(_commandBuffers[i], i);

			if (vkEndCommandBuffer(_commandBuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("failed to record command buffer!");
		}
	}

	void GraphicsInstance::CreateSyncObjects(void)
	{
		_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	#include "Utilities/ShowFps.hpp"

	void GraphicsInstance::DrawFrame(void)
	{
		vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(_device, _swapChain, std::numeric_limits<uint64_t>::max(), _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);
		_nextSwapChainImageIdx = imageIndex;

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire swap chain image!");

		GraphicsRenderer::GetInstance()->UpdateAllDatas(imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffers[imageIndex];

		VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(_device, 1, &_inFlightFences[_currentFrame]);

		if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS)
			throw std::runtime_error("failed to submit draw command buffer!");

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {_swapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(_presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized)
		{
			_framebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to present swap chain image!");

		//std::cout << "DRAW !" << std::endl;
		_currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void GraphicsInstance::RecreateSwapChain(void)
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(_window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(_window, &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(_device);

		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();

		CreateDepthResources();
		CreateFramebuffers();
		CreateDescriptorPool();
		CreateCommandBuffers();

		GraphicsRenderer::GetInstance()->RecreateAllPipelines();
		GraphicsRenderer::GetInstance()->RecreateAllUniforms();
		FillCommandBuffer();
	}

	void GraphicsInstance::DestroySwapChain( void )
	{
		vkDestroyImageView(_device, _depthImageView, nullptr);
		vmaDestroyImage(m_Allocator, _depthImage, _depthImageMemory);
		//vkDestroyImage(_device, _depthImage, nullptr);
		//vkFreeMemory(_device, _depthImageMemory, nullptr);

		for (auto framebuffer : _swapChainFramebuffers)
			vkDestroyFramebuffer(_device, framebuffer, nullptr);

		vkFreeCommandBuffers(_device, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

		vkDestroyRenderPass(_device, _renderPass, nullptr);

		for (auto imageView : _swapChainImageViews)
			vkDestroyImageView(_device, imageView, nullptr);

		vkDestroySwapchainKHR(_device, _swapChain, nullptr);

		vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
	}

	void GraphicsInstance::CleanupSwapChain(void)
	{
		vkDestroyImageView(_device, _depthImageView, nullptr);
		vmaDestroyImage(m_Allocator, _depthImage, _depthImageMemory);
		//vkDestroyImage(_device, _depthImage, nullptr);
		//vkFreeMemory(_device, _depthImageMemory, nullptr);

		for (auto framebuffer : _swapChainFramebuffers)
			vkDestroyFramebuffer(_device, framebuffer, nullptr);

		vkFreeCommandBuffers(_device, _commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

		GraphicsRenderer::GetInstance()->DestroyAllPipelines();

		vkDestroyRenderPass(_device, _renderPass, nullptr);

		for (auto imageView : _swapChainImageViews)
			vkDestroyImageView(_device, imageView, nullptr);

		vkDestroySwapchainKHR(_device, _swapChain, nullptr);

		GraphicsRenderer::GetInstance()->DestroyAllUniforms();

		vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
	}

	uint32_t GraphicsInstance::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	VertexBufferRenderer GraphicsInstance::CreateStorageBuffer(void *ptrData, uint32_t size )
	{
		VertexBufferRenderer stagingBuffer;
		VertexBufferRenderer bufRenderer;
		std::cout << "Storage BUFFER CREATION : " << size << std::endl;

		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer.buffer, stagingBuffer.bufferMemory);

		if (ptrData)
		{
			// TODO: Broken
			//void *data = MapGpuMemory();
			//vkMapMemory(_device, stagingBuffer.bufferMemory, 0, size, 0, &data);
			//memcpy(data, ptrData, (size_t)size);
			//vkUnmapMemory(_device, stagingBuffer.bufferMemory);
			//UnMapGpuMemory();
		}

		CreateBuffer(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, bufRenderer.buffer, bufRenderer.bufferMemory);

		CopyBuffer(stagingBuffer.buffer, bufRenderer.buffer, size);

		vmaDestroyBuffer(m_Allocator, stagingBuffer.buffer, stagingBuffer.bufferMemory);
		//vkDestroyBuffer(_device, stagingBuffer.buffer, nullptr);
		//vkFreeMemory(_device, stagingBuffer.bufferMemory, nullptr);

		return (bufRenderer);
	}

	VertexBufferRenderer GraphicsInstance::CreateVertexBuffer(uint8_t *ptrData, uint32_t size )
	{
		VertexBufferRenderer stagingBuffer;
		VertexBufferRenderer bufRenderer;
		std::cout << "Vertex BUFFER CREATION : " << size << std::endl;

		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer.buffer, stagingBuffer.bufferMemory);

		if (ptrData)
		{
			void *data;
			vmaMapMemory(m_Allocator, stagingBuffer.bufferMemory, &data);
			memcpy(data, ptrData, (size_t)size);
			vmaUnmapMemory(m_Allocator, stagingBuffer.bufferMemory);
		}

		CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, bufRenderer.buffer, bufRenderer.bufferMemory);

		CopyBuffer(stagingBuffer.buffer, bufRenderer.buffer, size);

		vmaDestroyBuffer(m_Allocator, stagingBuffer.buffer, stagingBuffer.bufferMemory);
		//vmaFreeMemory(m_Allocator, staginAlloc);
		return (bufRenderer);
	}

	IndiceBufferRenderer GraphicsInstance::CreateIndexBuffer(uint32_t *indexData, uint32_t size )
	{
		IndiceBufferRenderer bufRenderer;
		VkDeviceSize bufferSize = sizeof(uint32_t) * size;

		std::cout << "INDEX BUFFER CREATION : " << bufferSize << std::endl;

		bufRenderer.numIndices = size;

		VkBuffer stagingBuffer;
		VmaAllocation staginAlloc;
		//VkDeviceMemory stagingBufferMemory;

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer, staginAlloc);

		void *data;
		vmaMapMemory(m_Allocator, staginAlloc, &data);
		memcpy(data, indexData, (size_t)bufferSize);
		vmaUnmapMemory(m_Allocator, staginAlloc);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, bufRenderer.buffer, bufRenderer.bufferMemory);

		CopyBuffer(stagingBuffer, bufRenderer.buffer, bufferSize);

		vmaDestroyBuffer(m_Allocator, stagingBuffer, staginAlloc);
		//vmaFreeMemory(m_Allocator, staginAlloc);

		return (bufRenderer);
	}

	void GraphicsInstance::FramebufferResizeCallback(GLFWwindow *window, int width, int height)
	{
		auto app = reinterpret_cast<GraphicsInstance *>(glfwGetWindowUserPointer(window));
		app->_framebufferResized = true;
		std::cout << width << " " << height << std::endl;
		app->_windowAttribute._width = width;
		app->_windowAttribute._height = height;
	}

	VkSampler GraphicsInstance::CreateTextureSampler(Texture* texture)
	{
		VkSampler textureSampler;

		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = TextureFilterModeToVk(texture->GetFilterMode());
		samplerInfo.minFilter = TextureFilterModeToVk(texture->GetFilterMode());
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = texture->GetAnisotropLevel();
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(_device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture sampler!");

		// TODO
		//     vkDestroySampler(device, textureSampler, nullptr);
		//    vkDestroyImageView(device, textureImageView, nullptr);
		return textureSampler;
	}

	VkFilter TextureFilterModeToVk( EnumFilterMode filterMode )
	{
		switch (filterMode)
		{
			case EnumFilterMode::LINEAR:
				return (VK_FILTER_LINEAR);
			case EnumFilterMode::NEAREST:
				return (VK_FILTER_NEAREST);
			default:
				return VK_FILTER_LINEAR;
		}
		return VK_FILTER_LINEAR;
	}

	VkImageViewType TextureTypeToVkImageType( EnumTextureType type )
	{
		switch (type)
		{
			case EnumTextureType::TEXTURE_2D:
				return (VK_IMAGE_VIEW_TYPE_2D);
			case EnumTextureType::TEXTURE_CUBE:
				return (VK_IMAGE_VIEW_TYPE_CUBE);
			default:
				return (VK_IMAGE_VIEW_TYPE_2D);
		}
		return (VK_IMAGE_VIEW_TYPE_2D);
	}

	ImageRenderer GraphicsInstance::CreateTextureImage(Texture* texture)
	{
		ImageRenderer ir;
		size_t imageSize = texture->GetArrayLayer() * texture->mWidth * texture->mHeight * texture->GetFormat().GetSize();

		VkBuffer stagingBuffer;
		VmaAllocation staginAlloc;

		std::cout << "ImageSize BUFFER CREATION : " << imageSize << std::endl;

		CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer, staginAlloc);


		void *data = nullptr;
		vmaMapMemory(m_Allocator, staginAlloc, &data);
		memcpy(data, texture->GetData(), imageSize);
		vmaUnmapMemory(m_Allocator, staginAlloc);

		CreateImage({texture->mWidth, texture->mHeight}, TextureFormatToVkFormat(texture->GetFormat()), texture->GetArrayLayer(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_GPU_ONLY, ir._textureImage, ir._textureImageMemory);

		TransitionImageLayout(ir._textureImage, TextureFormatToVkFormat(texture->GetFormat()), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, TextureTypeToVkImageType(texture->GetType()));
		CopyBufferToImage(stagingBuffer, ir._textureImage, texture->mWidth, texture->mHeight, TextureTypeToVkImageType(texture->GetType()));
		TransitionImageLayout(ir._textureImage, TextureFormatToVkFormat(texture->GetFormat()), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, TextureTypeToVkImageType(texture->GetType()));

		vmaDestroyBuffer(m_Allocator, stagingBuffer, staginAlloc);

		return (ir);
	}

	VkFormat TextureFormatToVkFormat( TextureFormat format )
	{
		switch (format.mFormat)
		{
			case EnumTextureFormat::G:
				return (VK_FORMAT_R8_SNORM);
			case EnumTextureFormat::GA:
				return (VK_FORMAT_R8G8_UNORM);
			case EnumTextureFormat::RGB:
				return (VK_FORMAT_R8G8B8_UNORM);
			case EnumTextureFormat::RGBA:
				return (VK_FORMAT_R8G8B8A8_UNORM);
			default:
				return (VK_FORMAT_UNDEFINED);
		};
		return (VK_FORMAT_UNDEFINED);
	}

	void GraphicsInstance::CreateImage(VkExtent2D textureExtent, VkFormat format, uint32_t layer, VkImageTiling tiling, VkImageUsageFlags usage, VmaMemoryUsage properties, VkImage &image, VmaAllocation &imageMemory, VmaAllocationCreateFlags flag )
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = textureExtent.width;
		imageInfo.extent.height = textureExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.arrayLayers = layer;
		if (imageInfo.arrayLayers == 6)
			imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = properties;
		allocInfo.flags = flag;

		vmaCreateImage(m_Allocator, &imageInfo, &allocInfo, &image, &imageMemory, nullptr);

/*
		if (vkCreateImage(_device, &imageInfo, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(_device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate image memory!");

		vkBindImageMemory(_device, image, imageMemory, 0);
*/
	}

	VkCommandBuffer GraphicsInstance::BeginSingleTimeCommands(void)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void GraphicsInstance::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_graphicsQueue);

		vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
	}

	void GraphicsInstance::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageViewType vType)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(format))
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = vType == VK_IMAGE_VIEW_TYPE_2D ? 1 : 6;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		EndSingleTimeCommands(commandBuffer);
	}

	void GraphicsInstance::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkImageViewType vType)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = vType == VK_IMAGE_VIEW_TYPE_2D ? 1 : 6;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = {
			width,
			height,
			1};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndSingleTimeCommands(commandBuffer);
	}

	void GraphicsInstance::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VkBuffer &buffer, VmaAllocation& allocation )
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = memoryUsage;

		vmaCreateBuffer(m_Allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);

		/*
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate buffer memory!");
		std::cout << "Memory Created : " << bufferMemory << " of size : " << size << std::endl;
		vkBindBufferMemory(_device, buffer, bufferMemory, 0);
		*/
	}

	void GraphicsInstance::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = _commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(_graphicsQueue);

		vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
	}

	std::vector<VkDescriptorSetLayout> GraphicsInstance::CreateDescriptorSetLayout(std::vector<std::vector<VkDescriptorSetLayoutBinding>> uboLayoutBinding)
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayout;
		std::vector<VkDescriptorSetLayoutCreateInfo> layoutInfo(uboLayoutBinding.size());

		descriptorSetLayout.resize(uboLayoutBinding.size());

		for (uint32_t index = 0; index < descriptorSetLayout.size(); index++)
		{
			layoutInfo[index].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo[index].bindingCount = uboLayoutBinding[index].size();
			layoutInfo[index].pBindings = uboLayoutBinding[index].data();
			if (vkCreateDescriptorSetLayout(_device, &layoutInfo[index], nullptr, &(descriptorSetLayout[index])) != VK_SUCCESS)
				throw std::runtime_error("failed to create descriptor set layout!");
		}
		return (descriptorSetLayout);
	}

	void GraphicsInstance::CreateDescriptorPool(void)
	{
		VkDescriptorPoolSize poolSize[3] = {};
		poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize[0].descriptorCount = 50 * static_cast<uint32_t>(_swapChainImages.size());
		poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize[1].descriptorCount = 50 * static_cast<uint32_t>(_swapChainImages.size());
		poolSize[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSize[2].descriptorCount = 10 * static_cast<uint32_t>(_swapChainImages.size());

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 3; // number of elements in pPoolSizes.
		poolInfo.pPoolSizes = &poolSize[0];
		poolInfo.maxSets = poolSize[0].descriptorCount + poolSize[1].descriptorCount + poolSize[2].descriptorCount;

		if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS)
			throw std::runtime_error("failed to create descriptor pool!");
	}

	std::vector<VkDescriptorSet> GraphicsInstance::AllocateDescriptorSet( VkDescriptorSetLayout layout )
	{
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<VkDescriptorSetLayout> layouts(_swapChainImages.size(), layout);

		descriptorSets.resize(_swapChainImages.size());

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = _swapChainImages.size(); // number of descriptor sets to be allocated from the pool.
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(_device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate descriptor sets!");

		return (descriptorSets);
	}

	void GraphicsInstance::UpdateImageDescriptorSets(uint32_t* matIds, uint32_t count, DescriptorSetDescription& description, VkDescriptorSet* descriptorSets)
	{
		for (uint32_t index = 0 ; index < description.uniformsTexture.size() ; index++)
		{
			for (uint32_t countId = 0 ; countId < count ; countId++)
			{
				ImageProperties& image = GraphicsRenderer::GetInstance()->GetImageProperties(description.uniformsTexture[index]._textureId[matIds[countId]]);
				for (size_t i = 0; i < _swapChainImages.size(); i++)
				{
					VkDescriptorImageInfo imageInfo = {};
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = image._imageView;
					imageInfo.sampler = image._textureSampler;

					VkWriteDescriptorSet descriptorWrite = {};
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = descriptorSets[i];
					descriptorWrite.dstBinding = description.uniformsTexture[index]._binding;
					descriptorWrite.dstArrayElement = 0;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWrite.descriptorCount = 1;
					descriptorWrite.pImageInfo = &imageInfo;

					vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
				}
			}
		}
	}

	void GraphicsInstance::UpdateRuntimeDescriptorSets(uint32_t* matIds, uint32_t count, DescriptorSetDescription& description, VkDescriptorSet* descriptorSets)
	{
		for (uint32_t index = 0 ; index < description.uniformsRuntime.size() ; index++)
		{
			for (uint32_t countId = 0 ; countId < count ; countId++)
			{
				uint32_t bufferId = description.uniformsRuntime[index].mBuffers[matIds[countId]].GetId();
				BufferRenderer& buffer = GraphicsRenderer::GetInstance()->GetBufferRenderer(bufferId);

				for (size_t i = 0; i < _swapChainImages.size(); i++)
				{
					VkDescriptorBufferInfo bufferInfo = {};
					bufferInfo.buffer = buffer.GetBuffer();
					bufferInfo.offset = 0;//offset + offsetUniform;
					bufferInfo.range = buffer.GetSize();

					VkWriteDescriptorSet descriptorWrite = {};
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = descriptorSets[i];
					descriptorWrite.dstBinding = description.uniformsRuntime[index].mBinding;
					descriptorWrite.dstArrayElement = 0;
					descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					descriptorWrite.descriptorCount = 1;
					descriptorWrite.pBufferInfo = &bufferInfo;

					vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
				}
			}
		}
	}

	void GraphicsInstance::UpdateDescriptorSets(DescriptorSetDescription& description, uint32_t offset, VkDescriptorSet* descriptorSets, VkBuffer* gpuBuffers, uint32_t bufferCount)
	{
		// TODO: bufferCount
		// TODO: DIM
		uint32_t offsetUniform = 0;

		for (uint32_t index = 0 ; index < description.uniforms.size() ; index++)
		{
			for (size_t i = 0; i < _swapChainImages.size(); i++)
			{
				VkDescriptorBufferInfo bufferInfo = {};
				bufferInfo.buffer = gpuBuffers[i];
				bufferInfo.offset = offset + offsetUniform;
				bufferInfo.range = description.uniforms[index]._size;

				VkWriteDescriptorSet descriptorWrite = {};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = descriptorSets[i];
				descriptorWrite.dstBinding = description.uniforms[index]._binding;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;

				vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);
			}
			offsetUniform = description.uniforms[index]._size;
		}
	}

	void GraphicsInstance::DestroyDescriptorSet(VkDescriptorSet descriptor)
	{
		vkFreeDescriptorSets(_device, _descriptorPool, 1, &descriptor);
	}

	///////////// DEPTH BUFFER / STENCIL BUFFER ///////////////

	VkFormat GraphicsInstance::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(_physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat GraphicsInstance::FindDepthFormat(void)
	{
		return FindSupportedFormat(
			{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	bool GraphicsInstance::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void GraphicsInstance::CreateDepthResources(void)
	{
		VkFormat depthFormat = FindDepthFormat();

		CreateImage({_swapChainExtent.width, _swapChainExtent.height}, depthFormat, 1, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VMA_MEMORY_USAGE_GPU_ONLY, _depthImage, _depthImageMemory, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
		_depthImageView = CreateImageView(_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D);

		TransitionImageLayout(_depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_IMAGE_VIEW_TYPE_2D);
	}

	//////////// GETTER ////////////

	GLFWwindow *GraphicsInstance::GetWindow(void)
	{
		return (_window);
	}

	VkDevice GraphicsInstance::GetDevice(void)
	{
		return (_device);
	}

	VkExtent2D GraphicsInstance::GetSwapChainExtent(void)
	{
		return _swapChainExtent;
	}

	VkRenderPass GraphicsInstance::GetRenderPass(void)
	{
		return (_renderPass);
	}
	////////// INIT ////////////

	void GraphicsInstance::Initialize(void)
	{
		InitGLFW();
		InitGLFWHints();

		if (!glfwVulkanSupported())
			std::cout << "VULKAN NOT SUPPORTED maybe because of MOLTENVK or bad icd, check your drivers" << std::endl;

		CreateWindow();
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateAllocator();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();

		CreateCommandPool();

		CreateDepthResources();
		CreateFramebuffers();
		CreateDescriptorPool();
		CreateSyncObjects();

		CreateCommandBuffers();
	}

	uint32_t GraphicsInstance::GetNextIdImageToRender(void)
	{
		return (_nextSwapChainImageIdx);
	}

	VmaAllocator& GraphicsInstance::GetAllocator( void )
	{
		return m_Allocator;
	}

	void GraphicsInstance::CreateAllocator(void)
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = _physicalDevice;
		allocatorInfo.device = _device;
		allocatorInfo.instance = _vulkanInstance;

		vmaCreateAllocator(&allocatorInfo, &m_Allocator);
	}

	void GraphicsInstance::DestroyAllocator(void)
	{
		//Destroy
		//vmaDestroyBuffer(allocator, buffer, allocation);
		vmaDestroyAllocator(m_Allocator);
	}

	uint32_t GraphicsInstance::GetSwapChainSize(void)
	{
		return (_swapChainImages.size());
	}
} // namespace Soon
