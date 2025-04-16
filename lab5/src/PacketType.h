#pragma once
#include <cstdint>

inline constexpr uint8_t AUDIO_PACKET = 0x01;
inline constexpr uint8_t VIDEO_PACKET = 0x02;

struct MediaHeader
{
	uint8_t type;
	uint64_t timestamp;
	uint32_t sequence;
};