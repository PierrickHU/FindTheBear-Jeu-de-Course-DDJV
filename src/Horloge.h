#pragma once

namespace PM3D
{

class Horloge
{
public:
	Horloge();

	int64_t GetTimeCount() const;
	double GetSecPerCount() const { return m_SecPerCount; }
	// Retourne le temps en seconde entre deux marques.
	float GetTimeBetweenCounts(int64_t start, int64_t stop) const;

private:
	double m_SecPerCount;
};

} // namespace PM3D
