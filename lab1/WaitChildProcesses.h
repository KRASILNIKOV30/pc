#pragma once
#include <vector>
#include <sys/wait.h>
#include <stdexcept>

inline void WaitChildProcesses(std::vector<pid_t>& childPids)
{
	while (!empty(childPids))
	{
		auto childPid = waitpid(-1, nullptr, 0);
		std::erase(childPids, childPid);
		if (childPid == -1)
		{
			throw std::runtime_error("Error waiting for child processes");
		}
	}
}
