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

void PacketBuffer::TickBuffer()
{
	++timer;
	if (timer > timeToSend)
	{
		SendPacket();
		timer = 0;
	}
}

void PacketBuffer::SetServer(void* s)
{
	server = s;
}

void PacketBuffer::StartBuffer()
{
	gameStarted = true;
	int x;
	int y;
	do
	{
		x = rand();
		y = rand();
	} while (x < y);
	timeToSend = ((int)((float)x / (float)y) * (rand() % 1 == 0 ? -1 : 1) * 7.0 + 30.0);
}

