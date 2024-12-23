#include <algorithm>
#include <optional>
#include <set>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fmt/core.h>

#include <err.h>

#include "shaders.h"

#define ARRSIZE(x) (sizeof(x) / sizeof(x[0]))

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

#ifdef DEBUG
#define ENABLE_VALIDATION_LAYERS true
#else
#define ENABLE_VALIDATION_LAYERS false
#endif

static std::vector<const char*> getValidationLayers() {
	if(!ENABLE_VALIDATION_LAYERS) return {};

	///// get available validation layers /////

	uint32_t availableLayerCount;
	vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(availableLayerCount);
	vkEnumerateInstanceLayerProperties(
		&availableLayerCount, availableLayers.data()
	);

	///// get required validation layers /////

	const std::vector<const char*> requiredLayers = {
		"VK_LAYER_KHRONOS_validation",
	};

	///// print & verify /////

	fmt::println("available validation layers:");
	for(const VkLayerProperties& prop : availableLayers) {
		fmt::println("\t{}", prop.layerName);
	}

	fmt::println("required validation layers:");
	for(const char* layerName : requiredLayers) {
		fmt::println("\t{}", layerName);
	}

	auto missing = std::find_if_not(
		requiredLayers.begin(), requiredLayers.end(),
		[&availableLayers](const char* layerName) {
			return std::any_of(
				availableLayers.begin(), availableLayers.end(),
				[layerName](const VkLayerProperties& prop) {
					return strcmp(layerName, prop.layerName) == 0;
				}
			);
		}
	);

	if(missing != requiredLayers.end()) {
		throw std::runtime_error(
			fmt::format("missing validation layer {}", *missing)
		);
	}

	fmt::println("found all required validation layers!");

	return requiredLayers;
}

static std::vector<const char*> getExtensions() {

	///// get available extensions /////

	uint32_t availableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(
		nullptr, &availableExtensionCount, nullptr
	);

	std::vector<VkExtensionProperties> availableExtensions(
		availableExtensionCount
	);
	vkEnumerateInstanceExtensionProperties(
		nullptr, &availableExtensionCount, availableExtensions.data()
	);

	///// get required extensions /////

	uint32_t     glfwExtensionCount = 0;
	const char** glfwExtensions =
		glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> requiredExtensions(
		glfwExtensions, glfwExtensions + glfwExtensionCount
	);

	if(ENABLE_VALIDATION_LAYERS) {
		requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	requiredExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

	///// print & verify /////

	fmt::println("available extensions:");
	for(const VkExtensionProperties& e : availableExtensions) {
		fmt::println("\t{}", e.extensionName);
	}

	fmt::println("required extensions:");
	for(const char* e : requiredExtensions) {
		fmt::println("\t{}", e);
	}

	auto missing = std::find_if_not(
		requiredExtensions.begin(), requiredExtensions.end(),
		[&availableExtensions](const char* re) {
			return std::any_of(
				availableExtensions.begin(), availableExtensions.end(),
				[re](const VkExtensionProperties& ae) {
					return strcmp(re, ae.extensionName) == 0;
				}
			);
		}
	);

	if(missing != requiredExtensions.end()) {
		throw std::runtime_error(fmt::format("missing extension {}!", *missing)
		);
	}

	fmt::println("found all required extensions!");

	return requiredExtensions;
}

static void glfwErrorCB(int id, const char* description) {
	(void) id;
	fmt::println(stderr, "glfw error: {}", description);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
	VkDebugUtilsMessageTypeFlagsEXT             type,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void*                                       userData //
) {
	(void) severity;
	(void) type;
	(void) userData;

	if(severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
		fmt::println(stderr, "validation layer: {}", callbackData->pMessage);
	}

	return VK_FALSE;
}

static VkResult CreateDebugUtilsMessengerEXT(
	VkInstance                                instance, //
	const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
	const VkAllocationCallbacks*              allocator,
	VkDebugUtilsMessengerEXT*                 debugMessenger
) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT
	) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if(func != nullptr) {
		return func(instance, createInfo, allocator, debugMessenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void DestroyDebugUtilsMessengerEXT(
	VkInstance                   instance, //
	VkDebugUtilsMessengerEXT     debugMessenger,
	const VkAllocationCallbacks* allocator
) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT
	) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func != nullptr) { func(instance, debugMessenger, allocator); }
}

static void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo
) {
	createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	createInfo.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

	createInfo.messageType = //
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

	createInfo.pfnUserCallback = debugCallback;
}

class HelloTriangleApplication {

  public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

  private:
	GLFWwindow*                window;
	std::vector<const char*>   validationLayers;
	std::vector<const char*>   extensions;
	VkInstance                 instance;
	VkDebugUtilsMessengerEXT   debugMessenger;
	VkSurfaceKHR               surface;
	VkPhysicalDevice           physicalDevice;
	VkDevice                   device;
	VkQueue                    graphicsQueue;
	VkQueue                    presentQueue;
	VkSwapchainKHR             swapChain;
	std::vector<VkImage>       swapChainImages;
	VkFormat                   swapChainImageFormat;
	VkExtent2D                 swapChainExtent;
	std::vector<VkImageView>   swapChainImageViews;
	VkRenderPass               renderPass;
	VkPipelineLayout           pipelineLayout;
	VkPipeline                 graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool              commandPool;
	VkCommandBuffer            commandBuffer;
	VkSemaphore                imageAvailableSemaphore;
	VkSemaphore                renderFinishedSemaphore;
	VkFence                    inFlightFence;

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return                            //
				graphicsFamily.has_value() && //
				presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR        capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR>   presentModes;
	};

	void initWindow() {
		glfwSetErrorCallback(glfwErrorCB);

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandPool();
		createCommandBuffer();
		createSyncObjects();
	}

	void createInstance() {

		///// get required validation layers & extensions /////

		validationLayers = getValidationLayers();
		extensions       = getExtensions();

		///// create instance /////

		VkApplicationInfo appInfo{};
		appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName   = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName        = "No Engine";
		appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion         = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		createInfo.pApplicationInfo        = &appInfo;
		createInfo.enabledLayerCount       = validationLayers.size();
		createInfo.ppEnabledLayerNames     = validationLayers.data();
		createInfo.enabledExtensionCount   = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if(ENABLE_VALIDATION_LAYERS) {
			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}

		if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vulkan instance!");
		}
	}

	void setupDebugMessenger() {
		if(!ENABLE_VALIDATION_LAYERS) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if(CreateDebugUtilsMessengerEXT(
			   instance, &createInfo, nullptr, &debugMessenger
		   ) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void createSurface() {
		if(glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			device, &queueFamilyCount, nullptr
		);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(
			device, &queueFamilyCount, queueFamilies.data()
		);

		int i = 0;
		for(const VkQueueFamilyProperties& queueFamily : queueFamilies) {
			if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(
				device, i, surface, &presentSupport
			);
			if(presentSupport) { indices.presentFamily = i; }

			if(indices.isComplete()) break;

			i++;
		}

		return indices;
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if(extensionsSupported) {
			SwapChainSupportDetails details = querySwapChainSupport(device);
			swapChainAdequate               = //
				!details.formats.empty() &&   //

				!details.presentModes.empty();
		}

		return indices.isComplete() && extensionsSupported && swapChainAdequate;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(
			device, nullptr, &extensionCount, nullptr
		);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(
			device, nullptr, &extensionCount, availableExtensions.data()
		);

		std::set<std::string> requiredExtensions(
			deviceExtensions.begin(), deviceExtensions.end()
		);
		for(const VkExtensionProperties& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			device, surface, &details.capabilities
		);

		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(
			device, surface, &formatCount, nullptr
		);

		if(formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				device, surface, &formatCount, details.formats.data()
			);
		}

		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(
			device, surface, &presentModeCount, nullptr
		);

		if(presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device, surface, &presentModeCount, details.presentModes.data()
			);
		}

		return details;
	}

	void pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if(deviceCount == 0) {
			throw std::runtime_error("failed to find a GPU with Vulkan support!"
			);
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		auto maybeDevice = std::find_if(
			devices.begin(), devices.end(),
			[this](const VkPhysicalDevice& device) {
				return isDeviceSuitable(device);
			}
		);

		if(maybeDevice == devices.end()) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		physicalDevice = *maybeDevice;
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t>                   uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value(),
        };

		float queuePriority = 1.0f;
		for(uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount       = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType                 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount  = queueCreateInfos.size();
		createInfo.pQueueCreateInfos     = queueCreateInfos.data();
		createInfo.pEnabledFeatures      = &deviceFeatures;
		createInfo.enabledLayerCount     = validationLayers.size();
		createInfo.ppEnabledLayerNames   = validationLayers.data();
		createInfo.enabledExtensionCount = deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(
			device, indices.graphicsFamily.value(), 0, &graphicsQueue
		);

		vkGetDeviceQueue(
			device, indices.presentFamily.value(), 0, &presentQueue
		);
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& availableFormats
	) {
		for(const VkSurfaceFormatKHR& availableFormat : availableFormats) {
			if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
			   availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& availablePresentModes
	) {
		for(const auto& availablePresentMode : availablePresentModes) {
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabalities) {
		if(capabalities.currentExtent.width !=
		   std::numeric_limits<uint32_t>::max())
			return capabalities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			(uint32_t) width,
			(uint32_t) height,
		};

		actualExtent.width = std::clamp(
			actualExtent.width, capabalities.minImageExtent.width,
			capabalities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(
			actualExtent.height, capabalities.minImageExtent.height,
			capabalities.maxImageExtent.height
		);

		return actualExtent;
	}

	void createSwapChain() {
		SwapChainSupportDetails details = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat =
			chooseSwapSurfaceFormat(details.formats);
		VkPresentModeKHR presentMode =
			chooseSwapPresentMode(details.presentModes);
		VkExtent2D extent = chooseSwapExtent(details.capabilities);

		uint32_t imageCount = details.capabilities.minImageCount + 1;

		if(details.capabilities.maxImageCount > 0 &&
		   imageCount > details.capabilities.maxImageCount) {
			imageCount = details.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType         = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface       = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat   = surfaceFormat.format;
		createInfo.imageColorSpace  = surfaceFormat.colorSpace;
		createInfo.imageExtent      = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t           queueFamilyIndices[] = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value(),
        };

		if(indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices   = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform   = details.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode    = presentMode;
		createInfo.clipped        = VK_TRUE;
		createInfo.oldSwapchain   = VK_NULL_HANDLE;

		if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(
			device, swapChain, &imageCount, swapChainImages.data()
		);

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent      = extent;
	}

	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());

		for(size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image    = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format   = swapChainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel   = 0;
			createInfo.subresourceRange.levelCount     = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount     = 1;

			if(vkCreateImageView(
				   device, &createInfo, nullptr, &swapChainImageViews[i]
			   ) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	VkShaderModule createShaderModule(const char* code, size_t size) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = size;
		createInfo.pCode    = (uint32_t*) code;

		VkShaderModule shaderModule;
		if(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	void createRenderPass() {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format         = swapChainImageFormat;
		colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments    = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass   = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass   = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments    = &colorAttachment;
		renderPassInfo.subpassCount    = 1;
		renderPassInfo.pSubpasses      = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies   = &dependency;

		if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void createGraphicsPipeline() {
		VkShaderModule fragShaderModule = createShaderModule(
			build_triangle_frag_spv, build_triangle_frag_spv_len
		);

		VkShaderModule vertShaderModule = createShaderModule(
			build_triangle_vert_spv, build_triangle_vert_spv_len
		);

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName  = "main";

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName  = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {
			vertShaderStageInfo,
			fragShaderStageInfo,
		};

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount   = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType =
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount  = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType =
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable        = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth               = 1.0f;
		rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable         = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType =
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable  = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = //
			VK_COLOR_COMPONENT_R_BIT |        //
			VK_COLOR_COMPONENT_G_BIT |        //
			VK_COLOR_COMPONENT_B_BIT |        //
			VK_COLOR_COMPONENT_A_BIT;         //
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType =
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable   = VK_FALSE;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments    = &colorBlendAttachment;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType =
			VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = dynamicStates.size();
		dynamicState.pDynamicStates    = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType =
			VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		if(vkCreatePipelineLayout(
			   device, &pipelineLayoutInfo, nullptr, &pipelineLayout
		   ) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount          = ARRSIZE(shaderStages);
		pipelineInfo.pStages             = shaderStages;
		pipelineInfo.pVertexInputState   = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState      = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState   = &multisampling;
		pipelineInfo.pColorBlendState    = &colorBlending;
		pipelineInfo.pDynamicState       = &dynamicState;
		pipelineInfo.layout              = pipelineLayout;
		pipelineInfo.renderPass          = renderPass;
		pipelineInfo.subpass             = 0;

		if(vkCreateGraphicsPipelines(
			   device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
			   &graphicsPipeline
		   ) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	void createFramebuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for(size_t i = 0; i < swapChainImageViews.size(); i++) {
			VkImageView attachments[] = {swapChainImageViews[i]};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass      = renderPass;
			framebufferInfo.attachmentCount = ARRSIZE(attachments);
			framebufferInfo.pAttachments    = attachments;
			framebufferInfo.width           = swapChainExtent.width;
			framebufferInfo.height          = swapChainExtent.height;
			framebufferInfo.layers          = 1;

			if(vkCreateFramebuffer(
				   device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]
			   ) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void createCommandPool() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

		if(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void createCommandBuffer() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffer!");
		}
	}

	void createSyncObjects() {
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if(vkCreateSemaphore(
			   device, &semaphoreInfo, nullptr, &imageAvailableSemaphore
		   ) != VK_SUCCESS ||
		   vkCreateSemaphore(
			   device, &semaphoreInfo, nullptr, &renderFinishedSemaphore
		   ) != VK_SUCCESS ||
		   vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) !=
		       VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects!"
			);
		}
	}

	void mainLoop() {
		while(!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(device);
	}

	void drawFrame() {
		vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(device, 1, &inFlightFence);

		uint32_t imageIndex;
		vkAcquireNextImageKHR(
			device, swapChain, UINT64_MAX, imageAvailableSemaphore,
			VK_NULL_HANDLE, &imageIndex
		);

		vkResetCommandBuffer(commandBuffer, 0);
		recordCommandBuffer(commandBuffer, imageIndex);

		VkSubmitInfo submitInfo{};
		submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers    = &commandBuffer;

		VkSemaphore          waitSemaphores[] = {imageAvailableSemaphore};
		VkPipelineStageFlags waitStages[]     = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
		submitInfo.waitSemaphoreCount = ARRSIZE(waitSemaphores);
		submitInfo.pWaitSemaphores    = waitSemaphores;
		submitInfo.pWaitDstStageMask  = waitStages;

		VkSemaphore signalSemaphores[]  = {renderFinishedSemaphore};
		submitInfo.signalSemaphoreCount = ARRSIZE(signalSemaphores);
		submitInfo.pSignalSemaphores    = signalSemaphores;

		if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) !=
		   VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores    = signalSemaphores;
		presentInfo.pImageIndices      = &imageIndex;

		VkSwapchainKHR swapChains[] = {swapChain};
		presentInfo.swapchainCount  = ARRSIZE(swapChains);
		presentInfo.pSwapchains     = swapChains;

		vkQueuePresentKHR(presentQueue, &presentInfo);
	}

	void
	recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!"
			);
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType       = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass  = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor        = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues    = &clearColor;

		vkCmdBeginRenderPass(
			commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE
		);

		vkCmdBindPipeline(
			commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline
		);

		VkViewport viewport{};
		viewport.x        = 0.0f;
		viewport.y        = 0.0f;
		viewport.width    = swapChainExtent.width;
		viewport.height   = swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void cleanup() {
		vkDestroyFence(device, inFlightFence, nullptr);
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);

		vkDestroyCommandPool(device, commandPool, nullptr);

		for(VkFramebuffer framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);

		for(VkImageView imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);

		if(ENABLE_VALIDATION_LAYERS) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	} catch(const std::exception& e) {
		fmt::println(stderr, "{}", e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
