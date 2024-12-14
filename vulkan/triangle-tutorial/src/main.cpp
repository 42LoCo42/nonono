#include <algorithm>
#include <optional>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fmt/core.h>

#include <err.h>

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

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
	VkDebugUtilsMessageTypeFlagsEXT             type,
	const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
	void*                                       userData //
) {
	(void) severity;
	(void) type;
	(void) userData;

	fmt::println(stderr, "validation layer: {}", callbackData->pMessage);

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

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
};

static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
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

		if(indices.isComplete()) break;

		i++;
	}

	return indices;
}

static bool isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.isComplete();
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
	GLFWwindow*              window;
	VkInstance               instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	static void glfwErrorCB(int id, const char* description) {
		(void) id;
		fmt::println(stderr, "glfw error: {}", description);
	}

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
		pickPhysicalDevice();
	}

	void createInstance() {

		///// get required validation layers & extensions /////

		std::vector<const char*> validationLayers = getValidationLayers();
		std::vector<const char*> extensions       = getExtensions();

		///// create instance /////

		VkApplicationInfo appInfo  = {};
		appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName   = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName        = "No Engine";
		appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion         = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
		createInfo.pApplicationInfo        = &appInfo;
		createInfo.enabledLayerCount       = validationLayers.size();
		createInfo.ppEnabledLayerNames     = validationLayers.data();
		createInfo.enabledExtensionCount   = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
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
			[](const VkPhysicalDevice& device) {
				return isDeviceSuitable(device);
			}
		);

		if(maybeDevice == devices.end()) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		VkPhysicalDevice device = *maybeDevice;
	}

	void mainLoop() {
		// while(!glfwWindowShouldClose(window)) {
		// 	glfwPollEvents();
		// }
	}

	void cleanup() {
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
