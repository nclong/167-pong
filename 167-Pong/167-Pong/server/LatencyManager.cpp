#include "LatencyManager.h"


LatencyManager::LatencyManager()
{
}


LatencyManager::~LatencyManager()
{
}

int LatencyManager::ServerToClientSize = 0;
int LatencyManager::ClientToServerSize = 0;
int LatencyManager::CurrentServerToClientLatency = 0;
int LatencyManager::CurrentClientToServerLatency = 0;
float LatencyManager::AverageServerToClientLatency = 0;
float LatencyManager::AverageClientToServerLatency = 0;
int LatencyManager::ServerToClientLatencies[60];
int LatencyManager::ClientToServerLatencies[60];

void LatencyManager::AddServerToClientLatency(int lat)
{
	if (ServerToClientSize < 60)
	{
		++ServerToClientSize;
		ServerToClientLatencies[ServerToClientSize - 1] = lat;
	}
	else
	{
		for (int i = 1; i < ServerToClientSize; ++i)
		{
			ServerToClientLatencies[i - 1] = ServerToClientLatencies[i];
		}
		ServerToClientLatencies[59] = lat;
	}

	int sum = 0;
	for (int i = 0; i < ServerToClientSize; ++i)
	{
		sum += ServerToClientLatencies[i];
	}
	AverageServerToClientLatency = (float)sum / (float)ServerToClientSize;
}
void LatencyManager::AddClientToServerLatency(int lat)
{
	if (ClientToServerSize < 60)
	{
		++ClientToServerSize;
		ClientToServerLatencies[ClientToServerSize - 1] = lat;
	}
	else
	{
		for (int i = 1; i < ClientToServerSize; ++i)
		{
			ClientToServerLatencies[i - 1] = ClientToServerLatencies[i];
		}
		ClientToServerLatencies[59] = lat;
	}

	int sum = 0;
	for (int i = 0; i < ClientToServerSize; ++i)
	{
		sum += ClientToServerLatencies[i];
	}
	AverageClientToServerLatency = (float)sum / (float)ClientToServerSize;
}
