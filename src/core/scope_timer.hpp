#pragma once

class ScopeTimer {
public:
	ScopeTimer(std::string_view name);
	~ScopeTimer();

private:
	std::string m_name;
};
