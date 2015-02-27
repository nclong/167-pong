#include "PacketBuffer.h"


PacketBuffer::PacketBuffer()
{
}


PacketBuffer::~PacketBuffer()
{
}

normal_distribution<> PacketBuffer::distr(30.0, 7.0);
int PacketBuffer::timer = 0;

std::queue<Packet> PacketBuffer::PacketQueue;
int PacketBuffer::timeToSend = rand() % PACKET_MAX_TIME + PACKET_MIN_TIME;

void PacketBuffer::SendPacket(webSocket* server)
{
	while ( !PacketQueue.empty())
	{
		Packet p = PacketQueue.front();
		server->wsSend(p.clientID, p.message);
		PacketQueue.pop();
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	timeToSend = (int)round(distr(gen));
	int x = rand();
	int y = rand();
	while (x < y)
	{
		x = rand();
		y = rand();
	}
	timeToSend = (int)((float)x / (float)y * 7.0 + 30.0);
}

void PacketBuffer::wsSend(int client, std::string clientMessage)
{
	Packet p;
	p.clientID = client;
	p.message = clientMessage;
	PacketQueue.push(p);
}

void PacketBuffer::TickBuffer(webSocket* server)
{
	++timer;
	if (timer > timeToSend)
	{
		SendPacket(server);
		timer = 0;
	}
}

