#pragma once

#define SINGLETON_CLASS(T) \
public: \
	static inline T &get_instance() { \
		if(!sp_instance) [[unlikely]] { \
			sp_instance = std::make_unique<T>(); \
		} \
		return *sp_instance; \
	} \
	static inline void destroy_instance() { sp_instance.reset(); } \
private: \
	static std::unique_ptr<T> sp_instance;

#define SINGLETON_IMPL(T) \
	std::unique_ptr<T> T::sp_instance = nullptr;
