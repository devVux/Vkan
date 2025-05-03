#pragma once

#include <optional>

template<typename T, typename Deleter = std::nullptr_t>
class VulkanHandle {
    T handle = nullptr;
    Deleter deleter;
public:
    VulkanHandle(T handle): handle(handle) {}
    ~VulkanHandle() {
        if constexpr (!std::is_same_v<Deleter, std::nullptr_t>)
            if (handle) deleter(handle);
    }

    T get() const { return handle; }
};


template<typename T>
class Handle {
    T* ptr;
public:
    Handle(T* p = nullptr) : ptr(p) {}
	Handle(const Handle&) = default;
    ~Handle() = default;

    const T* get() const { return ptr; }

    T& operator*() const { return *ptr; }
    T* operator->() const { return ptr; }
};


enum class QueueType: uint8_t {
    Graphics,
    Present,
    Compute,
    Transfer
};
constexpr inline QueueType operator|(QueueType lhs, QueueType rhs) {
	return static_cast<QueueType>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}
constexpr inline bool operator&(QueueType lhs, QueueType rhs) {
	return static_cast<bool>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}
constexpr inline QueueType& operator|=(QueueType& lhs, QueueType rhs) {
	lhs = lhs | rhs;
	return lhs;
}


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	std::optional<uint32_t> computeFamily;
	std::optional<uint32_t> transferFamily;
   
	bool isComplete(QueueType required) const {
		bool complete = true;
		if (required & QueueType::Graphics) complete &= graphicsFamily.has_value();
		if (required & QueueType::Present) complete &= presentFamily.has_value();
		if (required & QueueType::Compute) complete &= computeFamily.has_value();
		if (required & QueueType::Transfer) complete &= transferFamily.has_value();
		return complete;
	}
};

struct Size {
	uint32_t width;
	uint32_t height;
};


