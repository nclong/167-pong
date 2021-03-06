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
	static void SetServer(void*);
	static void wsSend(int, std::string);
	static void TickBuffer(int time);
	static void StartBuffer();

private:
	static std::queue<Packet> PacketQueue;
	static void* server;
	static void SendPacket();
	static bool gameStarted;

};

