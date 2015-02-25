#include "LatencyManager.h"


LatencyManager::LatencyManager()
{
}


LatencyManager::~LatencyManager()
{
}

int LatencyManager::QueueMaxSize = 60;
int LatencyManager::ServerToClientSize[2];
int LatencyManager::ClientToServerSize[2];
int LatencyManager::CurrentServerToClientLatency = 0;
int LatencyManager::CurrentClientToServerLatency = 0;
int LatencyManager::AverageServerToClientLatency[2];
int LatencyManager::AverageClientToServerLatency[2];
std::queue<int> LatencyManager::ServerToClientLatencies[2];
std::queue<int> LatencyManager::ClientToServerLatencies[2];

void LatencyManager::AddServerToClientLatency(int clientID, int lat)
{
	if (ServerToClientSize[clientID] >= QueueMaxSize)
	{
		ServerToClientLatencies[clientID].pop();
		ServerToClientLatencies[clientID].push(lat);
	}
	else
	{
		ServerToClientLatencies[clientID].push(lat);
		ServerToClientSize[clientID]++;
	}

	int sum = 0;
	std::queue<int> qCopy = ServerToClientLatencies[clientID];
	for (int i = 0; i < ServerToClientSize[clientID]; ++i)
	{
		sum += qCopy.front();
		qCopy.pop();
	}

	int average = sum / ServerToClientSize[clientID];
	AverageServerToClientLatency[clientID] = average;
}


void LatencyManager::AddClientToServerLatency(int clientID, int lat)
{
	if (ClientToServerSize[clientID] >= QueueMaxSize)
	{
		ClientToServerLatencies[clientID].pop();
		ClientToServerLatencies[clientID].push(lat);
	}
	else
	{
		ClientToServerLatencies[clientID].push(lat);
		ClientToServerSize[clientID]++;
	}

	int sum = 0;
	std::queue<int> qCopy = ClientToServerLatencies[clientID];
	for (int i = 0; i < ClientToServerSize[clientID]; ++i)
	{
		sum += qCopy.front();
		qCopy.pop();
	}

	int average = sum / ClientToServerSize[clientID];
	AverageClientToServerLatency[clientID] = average;

}
