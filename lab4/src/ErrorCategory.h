#pragma once
#include "../lib/miniaudio.h"
#include <system_error>

inline const std::error_category& ErrorCategory()
{
	class AudioErrorCategory : public std::error_category
	{
	public:
		[[nodiscard]] const char* name() const noexcept override
		{
			return "Audio error category";
		}

		[[nodiscard]] std::string message(int errorCode) const override
		{
			return ma_result_description(static_cast<ma_result>(errorCode));
		}
	};

	static AudioErrorCategory errorCategory;
	return errorCategory;
}