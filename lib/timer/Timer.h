#include <chrono>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>

class Timer
{
	using Clock = std::chrono::high_resolution_clock;

public:
	explicit Timer(std::ostream& output, std::string name)
		: m_name(name)
		  , m_output(output)
	{
	}

	Timer(const Clock&) = delete;
	Clock& operator=(const Clock&) = delete;

	void Stop()
	{
		if (m_running)
		{
			m_running = false;
			const auto cur = Clock::now();
			auto dur = cur - m_start;
			m_output << m_name << " took "
				<< std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() << "ms"
				<< " (" << std::chrono::duration<double>(dur).count() << "s)" << std::endl;
		}
	}

	~Timer()
	{
		Stop();
	}

private:
	std::ostream& m_output;
	std::string m_name;
	Clock::time_point m_start = Clock::now();
	bool m_running = true;
};

template <typename Fn, typename... Args>
decltype(auto) MeasureTime(std::ostream& output, std::string name, Fn&& fn, Args&&... args)
{
	Timer t{ output, std::move(name) };
	return std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
}
