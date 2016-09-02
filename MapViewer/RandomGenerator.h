#pragma once

class RandomGenerator
{

public:
	RandomGenerator(int seed = 0) : m_CurVal(seed)
	{
	};

	void Reset(int seed)
	{
		m_CurVal = seed;
	};

	void Reset();

	// random number between 0 - 32767
	int Get()
	{
		return (((m_CurVal = m_CurVal * 214013L + 2531011L) >> 16) & 0x7fff);
	};

	// random number between 0.0 and 1.0
	float GetFloat()
	{
		return float(Get())*(1.0f / 32767.0f);
	};

	int Get(int minvalue, int maxvalue)
	{
		return (Get() % (maxvalue - minvalue + 1)) + minvalue;
	}

	float GetFloat(float minvalue, float maxvalue)
	{
		return GetFloat()*(maxvalue - minvalue) + minvalue;
	}


private:
	int m_CurVal;
};
