#include "gpuRunner/GPURunner.h"
#include "Random.h"
#include "vector/VectorMath.h"
#include <array>
#include <functional>
#include <vector>

struct Particle
{
	Vector3d pos;
	Vector3d speed;
	Vector3d acceleration;
	double mass{};
	Vector4f color;
};

constexpr double BOUNDARY = 50.0;
constexpr double MIN_WEIGHT = 1.0;
constexpr double MAX_WEIGHT = 10.0;
constexpr double SUN_WEIGHT = 330'000;
class Particles
{
public:
	void Update(double deltaTime)
	{
		for (auto& p : m_particles)
		{
			p.acceleration = Vector3d(0, 0, 0);
		}

		for (size_t i = 0; i < m_particles.size(); ++i)
		{
			for (size_t j = i + 1; j < m_particles.size(); ++j)
			{
				Vector3d delta = m_particles[j].pos - m_particles[i].pos;
				double distanceCube = delta.GetLengthSquared() * delta.GetLength();
				const double minDistance = 0.01;

				distanceCube = std::max(distanceCube, minDistance * minDistance);

				const auto m1 = m_particles[i].mass;
				const auto m2 = m_particles[j].mass;

				Vector3d force = m_G * m1 * m2 / distanceCube * delta;

				m_particles[i].acceleration += force / m1;
				m_particles[j].acceleration -= force / m2;
			}
		}

		const double scaledDelta = deltaTime * m_timeCoef;
		for (auto& p : m_particles)
		{
			p.speed += p.acceleration * scaledDelta;
			p.pos += p.speed * scaledDelta;

			if (std::abs(p.pos.x) > BOUNDARY)
			{
				p.pos.x = std::copysign(BOUNDARY, p.pos.x);
				p.speed.x *= -0.8;
			}
			if (std::abs(p.pos.y) > BOUNDARY)
			{
				p.pos.y = std::copysign(BOUNDARY, p.pos.y);
				p.speed.y *= -0.8;
			}
			if (std::abs(p.pos.z) > BOUNDARY)
			{
				p.pos.z = std::copysign(BOUNDARY, p.pos.z);
				p.speed.z *= -0.8;
			}
		}
	}

	void ForEach(const std::function<void(Particle const&)>& callback) const
	{
		for (const auto& p : m_particles)
		{
			callback(p);
		}
	}

	void Add()
	{
		Particle newParticle;
		newParticle.pos = {
			RandomDouble(-BOUNDARY, BOUNDARY),
			RandomDouble(-BOUNDARY, BOUNDARY),
			RandomDouble(-BOUNDARY, BOUNDARY)
		};

		Vector3d direction = Normalize(Vector3d{ -newParticle.pos.y, newParticle.pos.x, 0 });
		newParticle.speed = direction * RandomDouble(0.5, 2.0);

		newParticle.acceleration = { 0, 0, 0 };
		newParticle.mass = RandomDouble(MIN_WEIGHT, MAX_WEIGHT);
		newParticle.color = {
			static_cast<float>(RandomDouble(0.2, 1.0)),
			static_cast<float>(RandomDouble(0.2, 1.0)),
			static_cast<float>(RandomDouble(0.2, 1.0)),
			1.0f
		};

		m_particles.push_back(newParticle);
	}

	void Delete()
	{
		if (!m_particles.empty())
		{
			m_particles.pop_back();
		}
	}

	void IncreaseGravity() { m_G *= 3.5; }
	void DecreaseGravity() { m_G /= 1.5; }
	void SpeedUpTime() { m_timeCoef *= 1.5; }
	void SlowDownTime() { m_timeCoef /= 1.5; }

private:
	std::vector<Particle> m_particles = { {
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		{ 0, 0, 0 },
		SUN_WEIGHT,
		{ 1.0, 1.0, 0, 1.0 },
	} };
	double m_timeCoef = 1.0;
	double m_G = 6.67430e-11;
};