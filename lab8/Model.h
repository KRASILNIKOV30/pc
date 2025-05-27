#pragma once
#include "gpuRunner/GPURunner.h"
#include "Random.h"
#include "vector/VectorMath.h"
#include <array>
#include <functional>
#include <vector>

inline const char* KERNEL_SOURCE = R"(
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#pragma OPENCL EXTENSION cl_khr_fp32_atomic_add : enable

__kernel void computeGravity(
	const double G,
	const int num_particles,
	__global const float4* positions,
	__global float4* accelerations
) {
	int i = get_global_id(0);
	int j = get_global_id(1);

	if (j >= num_particles || j <= i)
	{
		return;
	}

	float4 p1 = positions[i];
	float4 p2 = positions[j];

	float3 delta = p1.xyz - p2.xyz;

	float distSqrt = dot(delta, delta);
	float distCube = distSqrt * sqrt(distSqrt);

	const float minDist = 0.01f;
	float minDistCube = minDist * minDist * minDist;
	distCube = fmax(distCube, minDistCube);

	float m1 = p1.w;
	float m2 = p2.w;

	float3 force = G * m1 * m2 / distCube * delta;

	float3 a1 = force / m1;
	float3 a2 = force / m2;

	atomic_add(&accelerations[i].x, a1.x);
    atomic_add(&accelerations[i].y, a1.y);
    atomic_add(&accelerations[i].z, a1.z);

    atomic_add(&accelerations[j].x, a2.x);
    atomic_add(&accelerations[j].y, a2.y);
    atomic_add(&accelerations[j].z, a2.z);
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
class Particles final : public GPURunner
{
public:
	[[nodiscard]] const char* GetKernelSource() const override
	{
		return KERNEL_SOURCE;
	}

	KernelArgs& GetKernelArgs() override
	{
		m_size = static_cast<int>(m_accelerations.size());
		m_kernelArgs.argValues = std::vector<ArgValue>{
			{ sizeof(double), (void*)&m_G },
			{ sizeof(int), (void*)&m_size }
		};
		m_accelerations.resize(m_size, { 0, 0, 0, 0 });
		m_kernelArgs.inputBuffers = { GetInputBuffer(m_positions) };
		m_kernelArgs.outputBuffer = GetOutputBuffer(m_accelerations);
		m_kernelArgs.globalSize = cl::NDRange(m_size, m_size);

		return m_kernelArgs;
	}

	void Update(double deltaTime)
	{
		Run("computeGravity", m_accelerations);

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

			if (std::abs(x) > BOUNDARY)
			{
				x = std::copysign(BOUNDARY, x);
				speed.x *= -0.8;
			}
			if (std::abs(y) > BOUNDARY)
			{
				y = std::copysign(BOUNDARY, y);
				speed.y *= -0.8;
			}
			if (std::abs(z) > BOUNDARY)
			{
				z = std::copysign(BOUNDARY, z);
				speed.z *= -0.8;
			}
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
		Vector3f speed = direction * RandomFloat(0.5, 2.0);

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
	[[nodiscard]] double GetGravityConstant() const { return m_G; }
	[[nodiscard]] float GetTime() const { return m_timeCoef; }

private:
	std::vector<cl_float4> m_positions = { { 0, 0, 0, SUN_WEIGHT } };
	std::vector<cl_float4> m_accelerations = { { 0, 0, 0, 0 } };
	std::vector<Vector4f> m_colors = { { 1.0, 1.0, 0, 1.0 } };
	std::vector<Vector3f> m_speeds = { { 0, 0, 0 } };
	float m_timeCoef = 1.0;
	double m_G = 6.67430e-11;
	KernelArgs m_kernelArgs = {};
	int m_size;
};