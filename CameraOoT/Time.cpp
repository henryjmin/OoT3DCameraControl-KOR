#include "Time.h"

void Time::StartTime()
{
	m_start_time_ = std::chrono::high_resolution_clock::now();
	m_current_time_ = std::chrono::high_resolution_clock::now();
	m_current_time_f_ = std::chrono::high_resolution_clock::now();
}

void Time::FixedUpdateTime()
{
	m_last_time_f_ = m_current_time_f_;
	m_current_time_f_ = std::chrono::high_resolution_clock::now();
	m_time_ = std::chrono::duration<float, std::chrono::seconds::period>(m_current_time_f_ - m_start_time_).count();
	m_fixed_delta_time_ = std::chrono::duration<float, std::chrono::seconds::period>(m_current_time_f_ - m_last_time_f_).count();
}

float Time::GetFixedDeltaTime() const
{
	return m_fixed_delta_time_;
}
