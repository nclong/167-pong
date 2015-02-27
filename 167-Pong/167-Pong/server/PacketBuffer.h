#pragma once
#include <queue>
#include <random>
#include <string>
#include <iostream>
#include "websocket.h"

struct t_packet
{
	std::string message;
	int clientID;
};

typedef t_packet Packet;

class PacketBuffer
{
public:
	PacketBuffer();
	~PacketBuffer();

	static int timer;
	static int timeToSend;
	static void SendPacket(webSocket*);
	static void wsSend(int, std::string);
	static normal_distribution<> distr;
	static void TickBuffer(webSocket*);

private:
	static std::queue<Packet> PacketQueue;

};

