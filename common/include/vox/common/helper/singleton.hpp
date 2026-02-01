#pragma once

#define SINGLETON_CLASS(T)\
public:\
	static inline T &get_instance() {\
		static T instance;\
		return instance;\
	}
