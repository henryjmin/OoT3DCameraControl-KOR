#ifndef ENGINE_TIME
#define ENGINE_TIME

#include <chrono>

class Time
{
public:
	void StartTime();
	void FixedUpdateTime();
	float GetFixedDeltaTime() const;

private:
	std::chrono::steady_clock::time_point m_start_time_;
	std::chrono::steady_clock::time_point m_current_time_f_;
	std::chrono::steady_clock::time_point m_last_time_f_;
	std::chrono::steady_clock::time_point m_current_time_;
	float m_time_ = 0.0f;
	float m_fixed_delta_time_ = 0.0f;
};

#endif
