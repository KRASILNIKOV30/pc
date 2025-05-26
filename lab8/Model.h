#include "Random.h"
#include "vector/VectorMath.h"
#include <functional>
#include <vector>

struct Particle
{
	Vector3d pos;
	Vector3d speed;
	Vector3d acceleration;
	double mass;
	Vector4f color;
};

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
				double distanceSq = delta.GetLengthSquared();
				const double minDistance = 0.01;

				distanceSq = std::max(distanceSq, minDistance * minDistance);

				const auto m1 = m_particles[i].mass;
				const auto m2 = m_particles[j].mass;

				double force = m_G * m1 * m2 / distanceSq;
				Vector3d forceDir = Normalize(delta);

				m_particles[i].acceleration += forceDir * (force / m1);
				m_particles[j].acceleration -= forceDir * (force / m2);
			}
		}

		// Интегрирование движения (явный метод Эйлера)
		const double scaledDelta = deltaTime * m_timeCoef;
		for (auto& p : m_particles)
		{
			p.speed += p.acceleration * scaledDelta;
			p.pos += p.speed * scaledDelta;
		}
	}

	void ForEach(std::function<void(Particle const&)> callback) const
	{
		for (const auto& p : m_particles)
		{
			callback(p);
		}
	}

	void Add()
	{
		Particle newParticle;
		// Генерация позиции в сфере радиусом 50 единиц
		newParticle.pos = {
			RandomDouble(-10.0, 10.0),
			RandomDouble(-10.0, 10.0),
			RandomDouble(-10.0, 10.0)
		};

		// Начальная скорость для орбитального движения
		Vector3d direction = Normalize(Vector3d{ -newParticle.pos.y, newParticle.pos.x, 0 });
		newParticle.speed = direction * RandomDouble(0.5, 2.0);

		newParticle.acceleration = { 0, 0, 0 };
		newParticle.mass = RandomDouble(1.0, 10.0);
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

	void IncreaseGravity() { m_G *= 1.1; }
	void DecreaseGravity() { m_G /= 1.1; }
	void SpeedUpTime() { m_timeCoef *= 1.1; }
	void SlowDownTime() { m_timeCoef /= 1.1; }

private:
	std::vector<Particle> m_particles;
	double m_timeCoef = 1.0;
	double m_G = 6.67430e-3;
};