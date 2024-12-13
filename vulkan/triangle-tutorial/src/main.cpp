#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <err.h>

#include <string>

void glfwDie(std::string msg) {
	char* description = nullptr;
	glfwGetError((const char**) &description);
	err(1, "error: %s: %s", msg.c_str(), description);
}

int main() {
	if(glfwInit() != GLFW_TRUE) glfwDie("glfwInit");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window =
		glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
	if(window == nullptr) glfwDie("glfwCreateWindow");

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	printf("%d extensions supported\n", extensionCount);

	// glm::mat4 matrix;
	// glm::vec4 vec;
	// auto      test = matrix * vec;

	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
