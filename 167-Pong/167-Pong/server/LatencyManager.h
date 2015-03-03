//Bad class
#pragma once
#include <queue>
class LatencyManager
{
public:
	LatencyManager();
	~LatencyManager();
	static int CurrentServerToClientLatency;
	static int CurrentClientToServerLatency;
	static int AverageServerToClientLatency[2];
	static int AverageClientToServerLatency[2];
	static void AddServerToClientLatency(int clientID, int lat);
	static void AddClientToServerLatency(int clientID, int lat);
	static int GetCurrentServerToClientLatency(int client);
	static int GetCurrentClientToServerLatency(int client);

private:
	static std::queue<int> ServerToClientLatencies[2];
	static std::queue<int> ClientToServerLatencies[2];
	static int ServerToClientSize[2];
	static int ClientToServerSize[2];
	static int QueueMaxSize;
};

