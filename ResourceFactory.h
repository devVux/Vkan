#pragma once  

#include <vulkan/vulkan.h>  
#include <vector>  

struct GLFWwindow;  

struct ResourceFactory {  

   static VkInstance createContext(const VkInstanceCreateInfo& info);  
   static VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window);  

   static std::vector<VkPhysicalDevice> tellGPUs(VkInstance instance);  
   static VkDevice selectGPU(VkPhysicalDevice device, const VkDeviceCreateInfo& info);  

   static VkQueue createQueue(VkDevice device, uint32_t queueIndex);  

   static VkSwapchainKHR createSwapChain(VkDevice device, const VkSwapchainCreateInfoKHR& info);  

   static VkRenderPass createRenderPass(VkDevice device, const VkRenderPassCreateInfo& info);  

   static VkPipeline createGraphicsPipeline(VkDevice device, const VkGraphicsPipelineCreateInfo& info);  
   static VkPipelineLayout createPipelineLayout(VkDevice device, const VkPipelineLayoutCreateInfo& info);  

};
