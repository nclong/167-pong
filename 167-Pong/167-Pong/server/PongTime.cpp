#define _CRT_SECURE_NO_WARNINGS
#include "PongTime.h"


PongTime::PongTime()
{
}


PongTime::~PongTime()
{
}

int PongTime::operator-(PongTime pt)
{
	unsigned long long int theseMs = (unsigned long long int)pTime * (unsigned long long int)1000 + (unsigned long long int)fractionalSeconds;
	unsigned long long int thoseMs = (unsigned long long int)pt.pTime * (unsigned long long int)1000 + (unsigned long long int)pt.fractionalSeconds;
	return (int)(theseMs - thoseMs);
}
void PongTime::GetNow()
{
	std::chrono::system_clock::time_point rawNow;
	rawNow = std::chrono::system_clock::now();
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(rawNow.time_since_epoch());
	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>(ms);
	fractionalSeconds = ms.count() & 1000;
	pTime = std::chrono::system_clock::to_time_t(rawNow);
}

std::string PongTime::ToString()
{
	std::string result = "";
	std::ostringstream os;
	char buff[12];
	struct tm* timeinfo;
	timeinfo = localtime(&pTime);
	strftime(buff, 12, "%H:%M:%S", timeinfo);
	os << buff << ":";
	os << fractionalSeconds;

	return os.str();
}

void PongTime::SetFromString(std::string s)
{
	std::string timeStr;
	std::string msStr;
	int lastColon = s.find_last_of(":");
	timeStr = s.substr(0, lastColon);
	msStr = s.substr(lastColon + 1);
	struct std::tm t;
	std::istringstream ss(timeStr);
	ss >> std::get_time(&t, "%H:%M:%S");
	std::istringstream mss(msStr);
	struct std::tm* today;
	time_t now = time(NULL);
	today = std::localtime(&now);
	t.tm_mday = today->tm_mday;
	t.tm_mon = today->tm_mon;
	t.tm_year = today->tm_year;
	t.tm_wday = today->tm_wday;
	t.tm_yday = today->tm_yday;
	t.tm_isdst = today->tm_isdst;

	time_t temp = std::mktime(&t);
	pTime = std::mktime(&t);
	mss >> fractionalSeconds;
}
