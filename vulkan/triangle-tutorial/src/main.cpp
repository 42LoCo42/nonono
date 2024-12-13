#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <fmt/core.h>

#include <err.h>

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
  public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

  private:
	GLFWwindow* window;
	VkInstance  instance;

	static void glfwErrorCB(int id, const char* description) {
		(void) id;
		fprintf(stderr, "glfw error: %s\n", description);
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
	}

	void createInstance() {

		///// get available extensions /////

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(
			nullptr, &extensionCount, nullptr
		);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(
			nullptr, &extensionCount, extensions.data()
		);

		puts("available extensions:");
		for(const auto& e : extensions) {
			printf("\t%s\n", e.extensionName);
		}

		///// get required extensions /////

		uint32_t     glfwExtensionCount = 0;
		const char** glfwExtensions =
			glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> requiredExtensions;

		for(uint32_t i = 0; i < glfwExtensionCount; i++) {
			requiredExtensions.emplace_back(glfwExtensions[i]);
		}

		requiredExtensions.emplace_back(
			VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		);

		puts("required extensions:");
		for(const auto e : requiredExtensions) {
			printf("\t%s\n", e);
		}

		///// check if all required extensions are available /////

		auto missing = std::find_if_not(
			requiredExtensions.begin(), requiredExtensions.end(),
			[&extensions](const char* re) {
				return std::any_of(
					extensions.begin(), extensions.end(),
					[re](const VkExtensionProperties& ae) {
						return strcmp(re, ae.extensionName) == 0;
					}
				);
			}
		);
		if(missing != requiredExtensions.end()) {
			throw std::runtime_error(
				fmt::format("missing extension {}!", *missing)
			);
		}

		puts("found all required extensions!");

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
		createInfo.enabledLayerCount       = 0;
		createInfo.enabledExtensionCount   = requiredExtensions.size();
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vulkan instance!");
		}
	}

	void mainLoop() {
		while(!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};

int main() {
	HelloTriangleApplication app;

	try {
		app.run();
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
