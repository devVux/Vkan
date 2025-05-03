cmake_minimum_required(VERSION 3.14)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

include(FetchContent)

FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG latest
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG master
)

FetchContent_MakeAvailable(glfw glm)


add_executable(Vkan 
	main2.cpp 
	"Application.h" 
	"Window.h" 
	"GPU.h" 
	"SwapChain.h" 
	"ResourceFactory.h" 
	"Common.h" 
	"Context.cpp" 
	"ResourceFactory.cpp"
	"GPU.cpp" 
	"CommandBuffer.h"
	"SwapChain.cpp"
	"RenderPass.h"
	"RenderPass.cpp"
	"Window.cpp"
	"GraphicsPipeline.h" 
	"GraphicsPipeline.cpp" 
	"Framebuffer.h" 
	"Framebuffer.cpp" 
	"Fence.h" 
	"Fence.cpp" 
	"CommandBuffer.cpp" 
	"CommandQueue.h" 
	"CommandQueue.cpp" 
)



if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(Vkan PRIVATE -fsanitize=address)
    target_link_options(Vkan PRIVATE -fsanitize=address)
endif()

target_link_libraries(Vkan PRIVATE Vulkan::Vulkan glfw)
target_include_directories(Vkan PRIVATE ${glm_SOURCE_DIR})
