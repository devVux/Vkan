#include "Context.h"
 
#include "ResourceFactory.h"
#include <iostream>

#include <GLFW/glfw3.h>
#include <vector>
#include <cassert>


static VkDebugUtilsMessengerEXT debugMessenger;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}



std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (1) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		extensions.push_back("VK_MVK_macos_surface");
		extensions.push_back("VK_KHR_portability_enumeration");
		extensions.push_back("VK_KHR_get_physical_device_properties2");
    }

    return extensions;
}



Context& Context::operator=(Context&& other) noexcept {
	if (this != &other) {
		if (mInstance != VK_NULL_HANDLE) {
			DestroyDebugUtilsMessengerEXT(mInstance, debugMessenger, nullptr);
			vkDestroyInstance(mInstance, nullptr);
		}

		mInstance = other.mInstance;
		mSurface = other.mSurface;

		other.mInstance= VK_NULL_HANDLE;
		other.mSurface = VK_NULL_HANDLE;
	}
	return *this;
}

Context::~Context() {
	if(mInstance == VK_NULL_HANDLE)
		return;

	DestroyDebugUtilsMessengerEXT(mInstance, debugMessenger, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

VkSurfaceKHR Context::createSurface(const Window& window) {
	return mSurface = ResourceFactory::createSurface(mInstance, window.native());
}






ContextBuilder::ContextBuilder() {
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;
}

Context ContextBuilder::create() {
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    createInfo.pApplicationInfo = &appInfo;
        

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};	


    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
    if (1) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }


    VkInstance context = ResourceFactory::createContext(createInfo);
    return Context(context);
}

ContextBuilder & ContextBuilder::applicationName(const char * name) {
    appInfo.pApplicationName = name;
    return *this;    
}

ContextBuilder & ContextBuilder::applicationVersion(uint32_t major, uint32_t minor, uint32_t patch) {
    appInfo.applicationVersion = VK_MAKE_VERSION(major, minor, patch);
    return *this;
}

ContextBuilder & ContextBuilder::engineName(const char * name) {
    appInfo.pEngineName = name;
    return *this;
}

ContextBuilder & ContextBuilder::engineVersion(uint32_t major, uint32_t minor, uint32_t patch) {
    appInfo.engineVersion = VK_MAKE_VERSION(major, minor, patch);
    return *this;
}

ContextBuilder & ContextBuilder::apiVersion(uint32_t variant, uint32_t major, uint32_t minor) {
    appInfo.apiVersion = VK_MAKE_API_VERSION(variant, major, minor, 0);
    return *this;
}
