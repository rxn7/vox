#pragma once

#define SINGLETON_CLASS(T)\
public:\
	T(const T &) = delete;\
	static inline T &get_instance() {\
		static T instance;\
		return instance;\
	} \
private: \
	T() {}\
