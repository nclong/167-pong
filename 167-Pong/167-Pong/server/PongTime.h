#pragma once
#include <ctime>
#include <chrono>
#include <string>
#include <sstream>
#include<iomanip>
class PongTime
{
public:
	PongTime();
	~PongTime();
	time_t pTime;
	int fractionalSeconds;
	int operator-(PongTime pt);
	void GetNow();
	std::string ToString();
	void SetFromString(std::string s);
};

