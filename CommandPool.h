#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

class CommandPool {

public:

    CommandPool(VkCommandPool commandPool = VK_NULL_HANDLE, VkDevice device = VK_NULL_HANDLE)
        : mCommandPool(commandPool), mDevice(device) { }
    CommandPool(const CommandPool&) = delete;
    CommandPool(CommandPool&& other) noexcept: mCommandPool(other.mCommandPool), mDevice(other.mDevice) {
        other.mCommandPool = VK_NULL_HANDLE;
        other.mDevice = VK_NULL_HANDLE;
    }
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&& other) noexcept {
        if (this != &other) {
            if (mCommandPool != VK_NULL_HANDLE)
                vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

            mCommandPool = other.mCommandPool;
            mDevice = other.mDevice;

            other.mCommandPool = VK_NULL_HANDLE;
            other.mDevice = VK_NULL_HANDLE;
        }
        return *this;
    }
    ~CommandPool() noexcept {
        if (mCommandPool != VK_NULL_HANDLE)
            vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    }

    operator VkCommandPool() const { return mCommandPool; }


private:

    VkCommandPool mCommandPool;
    VkDevice mDevice;

};
