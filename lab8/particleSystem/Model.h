#pragma once
#include "../gpuRunner/GPURunner.h"
#include "../Random.h"
#include "../vector/VectorMath.h"
#include <algorithm>
#include <array>
#include <functional>
#include <ranges>
#include <vector>

inline const char* KERNEL_SOURCE = R"(
__kernel void compute_gravity(
	const float G,
	const int num_particles,
	__global const float4* positions,
	__global float3* accelerations
) {
	int i = get_global_id(0);

	float4 p1 = positions[i];
	float3 acc = (float3)(0.0f, 0.0f, 0.0f);

	for (int j = 0; j < num_particles; j++)
	{
		if (i == j) continue;

		float4 p2 = positions[j];
		float3 delta = p2.xyz - p1.xyz;
		float distSqr = dot(delta, delta);
		const float minDist = 0.01f;
		distSqr = fmax(distSqr, minDist);

		float3 force = G * p2.w / distSqr;
		acc += force * normalize(delta);
	}

	accelerations[i] = acc;
}
)";

struct ParticleView
{
	Vector4f pos;
	Vector4f color;
};

constexpr float BOUNDARY = 50.0;
constexpr float MIN_WEIGHT = 1.0;
constexpr float MAX_WEIGHT = 10.0;
constexpr float SUN_WEIGHT = 330'000;

class Particles
{
public:
	void SetKernelArgs()
	{
		m_size = static_cast<int>(m_accelerations.size());
		m_kernelArgs.argValues = { m_G, m_size };
		m_accelerations.resize(m_size, { 0, 0, 0 });
		m_kernelArgs.inputBuffers = { m_gpuRunner.GetInputBuffer(m_positions) };
		m_kernelArgs.outputBuffer = m_gpuRunner.GetOutputBuffer(m_accelerations);
		m_kernelArgs.globalSize = cl::NDRange(m_size);
	}

	void Update(double deltaTime)
	{
		SetKernelArgs();
		m_gpuRunner.Run("compute_gravity", m_kernelArgs, m_accelerations);

		const float scaledDelta = static_cast<float>(deltaTime) * m_timeCoef;
		for (int i = 0; i < m_accelerations.size(); ++i)
		{
			const auto& [ax, ay, az, aw] = m_accelerations[i].s;
			auto& speed = m_speeds[i];
			speed += Vector3f(ax, ay, az) * scaledDelta;

			auto& [x, y, z, m] = m_positions[i].s;
			x += speed.x * scaledDelta;
			y += speed.y * scaledDelta;
			z += speed.z * scaledDelta;

			//			if (std::abs(x) > BOUNDARY)
			//			{
			//				x = std::copysign(BOUNDARY, x);
			//				speed.x *= -0.8;
			//			}
			//			if (std::abs(y) > BOUNDARY)
			//			{
			//				y = std::copysign(BOUNDARY, y);
			//				speed.y *= -0.8;
			//			}
			//			if (std::abs(z) > BOUNDARY)
			//			{
			//				z = std::copysign(BOUNDARY, z);
			//				speed.z *= -0.8;
			//			}
		}
	}

	void ForEach(const std::function<void(ParticleView const&)>& callback) const
	{
		for (int i = 0; i < m_accelerations.size(); ++i)
		{
			auto& [x, y, z, m] = m_positions[i].s;
			Vector4f pos(x, y, z, m);
			callback({ pos, m_colors[i] });
		}
	}

	void Add()
	{
		cl_float4 pos = {
			RandomFloat(-BOUNDARY, BOUNDARY),
			RandomFloat(-BOUNDARY, BOUNDARY),
			RandomFloat(-BOUNDARY, BOUNDARY),
			RandomFloat(MIN_WEIGHT, MAX_WEIGHT),
		};

		Vector3f direction = Normalize(Vector3f{ -pos.s[1], pos.s[0], 0 });
		Vector3f speed = direction * RandomFloat(0.1, 1.0);

		cl_float4 acceleration = { 0, 0, 0, 0 };

		Vector4f color = {
			static_cast<float>(RandomFloat(0.2, 1.0)),
			static_cast<float>(RandomFloat(0.2, 1.0)),
			static_cast<float>(RandomFloat(0.2, 1.0)),
			1.0f
		};

		m_accelerations.push_back(acceleration);
		m_positions.push_back(pos);
		m_speeds.push_back(speed);
		m_colors.push_back(color);
	}

	void Delete()
	{
		if (!m_accelerations.empty())
		{
			m_accelerations.pop_back();
			m_positions.pop_back();
			m_colors.pop_back();
			m_speeds.pop_back();
		}
	}

	void IncreaseGravity() { m_G *= 3.5; }
	void DecreaseGravity() { m_G /= 1.5; }
	void SpeedUpTime() { m_timeCoef *= 1.5; }
	void SlowDownTime() { m_timeCoef /= 1.5; }

	[[nodiscard]] size_t GetNumParticles() const { return m_accelerations.size(); }
	[[nodiscard]] float GetGravityConstant() const { return m_G; }
	[[nodiscard]] float GetTime() const { return m_timeCoef; }

private:
	std::vector<cl_float4> m_positions = { { 0, 0, 0, SUN_WEIGHT } };
	std::vector<cl_float3> m_accelerations = { { 0, 0, 0 } };
	std::vector<Vector4f> m_colors = { { 1.0, 1.0, 0, 1.0 } };
	std::vector<Vector3f> m_speeds = { { 0, 0, 0 } };
	float m_timeCoef = 1.0;
	float m_G = 6.67430e-11;
	KernelArgs m_kernelArgs = {};
	GPURunner m_gpuRunner{ KERNEL_SOURCE };
	int m_size;
};