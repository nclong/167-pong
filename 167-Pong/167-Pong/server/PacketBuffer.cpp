#include "PacketBuffer.h"


PacketBuffer::PacketBuffer()
{
}


PacketBuffer::~PacketBuffer()
{
}

std::vector<PongTime[4]> PacketBuffer::theBuffer;
std::vector<bool[2]> PacketBuffer::bufferFlags;
std::vector<int> PacketBuffer::bufferElementIds;
int PacketBuffer::maxSize = 5;
int PacketBuffer::currentSize = 0;
int PacketBuffer::bufferCounter = 0;

int PacketBuffer::GetNewPacketId()
{
	if (currentSize >= maxSize)
	{
		return -1;
	}
	return bufferCounter++;
}
int PacketBuffer::EnqueueNewId(PongTime pt, int id)
{
	if (currentSize >= maxSize )
	{
		return -1;
	}
	else
	{
		theBuffer.at(currentSize++)[0] = pt;
		bufferElementIds.at(currentSize) = id;
		bufferFlags.at(currentSize)[0] = true;
	}
}

int PacketBuffer::EnqueueById(PongTime pt, int order, int id)
{
	for (int i = 0; i < currentSize; ++i)
	{
		if (bufferElementIds.at(i) == id)
		{
			theBuffer.at(i)[order] = pt;
			if (order == 3)
			{
				bufferFlags.at(i)[1] == true;
			}

			return 1;
		}
	}

	return -1;
}

bool PacketBuffer::hasId(int id)
{
	for (int i = 0; i < currentSize; ++i)
	{
		if (bufferElementIds.at(i) == id)
		{
			return true;
		}
	}

	return false;
}

void PacketBuffer::CalcPackets()
{
	for (int i = 0; i < bufferCounter; ++i)
	{
		bool allTrue = true;
		for (int j = 0; j < 2; j++)
		{
			if (!bufferFlags.at(i)[j])
			{
				allTrue = false;
			}
		}

		if (allTrue)
		{
			LatencyManager::AddServerToClientLatency(theBuffer.at(i)[1] - theBuffer.at(i)[0]);
			LatencyManager::AddClientToServerLatency(theBuffer.at(i)[3] - theBuffer.at(i)[2]);
			theBuffer.pop_back();
			bufferElementIds.pop_back();
			bufferFlags.pop_back();
			currentSize--;
		}
	}
}