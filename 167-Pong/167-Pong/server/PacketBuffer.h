#pragma once
#include <vector>
#include "PongTime.h"
#include "LatencyManager.h"
class PacketBuffer
{
public:
	PacketBuffer();
	~PacketBuffer();
	static std::vector<PongTime[4]> theBuffer;
	static std::vector<bool[2]> bufferFlags;
	static std::vector<int> bufferElementIds;
	static int maxSize;
	static int currentSize;
	static int bufferCounter;
	static int GetNewPacketId();
	static int GetPacketNum();
	static bool hasId(int id);
	static int EnqueueNewId(PongTime pt, int id);
	static int EnqueueById(PongTime pt, int order, int id);
	static void CalcPackets();
};

