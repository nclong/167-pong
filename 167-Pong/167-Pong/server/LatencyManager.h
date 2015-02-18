//Bad class
#pragma once
class LatencyManager
{
public:
	LatencyManager();
	~LatencyManager();
	static int CurrentServerToClientLatency;
	static int CurrentClientToServerLatency;
	static float AverageServerToClientLatency;
	static float AverageClientToServerLatency;
	static void AddServerToClientLatency(int lat);
	static void AddClientToServerLatency(int lat);

private:
	static int ServerToClientLatencies[60];
	static int ClientToServerLatencies[60];
	static int ServerToClientSize;
	static int ClientToServerSize;
};

