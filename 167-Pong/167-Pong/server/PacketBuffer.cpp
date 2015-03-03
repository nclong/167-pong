#include "PacketBuffer.h"


PacketBuffer::PacketBuffer()
{
}


PacketBuffer::~PacketBuffer()
{
}

int PacketBuffer::timer = 0;
void* PacketBuffer::server = NULL;
bool PacketBuffer::gameStarted = false;

std::queue<Packet> PacketBuffer::PacketQueue;
int PacketBuffer::timeToSend = rand() % PACKET_MAX_TIME + PACKET_MIN_TIME;

void PacketBuffer::SendPacket()
{
	if (gameStarted)
	{
		while (!PacketQueue.empty())
		{
			Packet p = PacketQueue.front();
			((webSocket*)server)->wsSend(p.clientID, p.message);
			PacketQueue.pop();
		}
		
	}
}

void PacketBuffer::wsSend(int client, std::string clientMessage)
{
	Packet p;
	p.clientID = client;
	p.message = clientMessage;
	PacketQueue.push(p);
}

void PacketBuffer::TickBuffer(int time)
{
	timer += time;
	bool packetSent = false;
	while (timer > timeToSend)
	{
		if (!packetSent)
		{
			SendPacket();
			packetSent = true;
		}
		timer = timer - timeToSend;
	}
}

void PacketBuffer::SetServer(void* s)
{
	server = s;
}

void PacketBuffer::StartBuffer()
{
	gameStarted = true;
	int range = 50;
	float runningRandom = 0.0;
	for (int i = 0; i < range; i++)
	{
		int x;
		int y;
		do
		{
			x = rand();
			y = rand();
		} while (x < y);

		runningRandom += (float)x / (float)y;
	}
	float normalizedRand = (runningRandom - ((float)range / 2.0)) / ((float)range / 2.0);
	timeToSend = round(normalizedRand + 75);
}

