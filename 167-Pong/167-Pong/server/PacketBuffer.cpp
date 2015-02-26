#include "PacketBuffer.h"


PacketBuffer::PacketBuffer()
{
}


PacketBuffer::~PacketBuffer()
{
}

std::queue<Packet> PacketBuffer::PacketQueue;
int PacketBuffer::timeToSend = rand() % PACKET_MAX_TIME + PACKET_MIN_TIME;

void PacketBuffer::SendPacket(webSocket server)
{
	while ( !PacketQueue.empty())
	{
		Packet p = PacketQueue.front();
		server.wsSend(p.clientID, p.message);
		PacketQueue.pop();
		timeToSend = rand() % PACKET_MAX_TIME + PACKET_MIN_TIME;
	}
}

void PacketBuffer::wsSend(int client, std::string clientMessage)
{
	Packet p;
	p.clientID = client;
	p.message = clientMessage;
	PacketQueue.push(p);
}

