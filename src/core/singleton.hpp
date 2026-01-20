#pragma once

#define SINGLETON_CLASS(T)\
public:\
	T(const T &) = delete;\
	static inline T &get_instance() {\
		static T instance;\
		return instance;\
	} \
	void init() {}\
	void destroy() {}\
private: \
	T() {}\
