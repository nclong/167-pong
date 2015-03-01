#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <chrono>
#include "websocket.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Paddle.h"
#include "Ball.h"
#include "Wall.h"
#include "Constants.h"
#include "PlayerManager.h"
#include "PongTime.h"
#include "LatencyManager.h"
#include "PacketBuffer.h"
#include "PacketTypes.h"

using namespace std;

webSocket server;
Ball ball;
Paddle paddle1;
Paddle paddle2;
PlayerInfo player1;
PlayerInfo player2;
Wall topWall;
Wall bottomWall;

string player1Name;
string player2Name;
SYSTEMTIME serverSendTime;
bool pingSent = false;
bool packetSent = false;

bool player1Connected = false;
bool player2Connected = false;
bool player1Ready = false;
bool player2Ready = false;
bool ping1Received = false;
bool ping2Received = false;

bool bufferInitiated = false;

bool gameStarted;

void startGame();

string FormatPacketString(SendTypes sendType, int clientId, int value1, int value2)
{
	//Packet Format
	//<function>[<clientID>]{<data>}
	string result = "";
	switch (sendType)
	{
		case PaddlePosition:
			result += "pp";
			break;
		case BallPosition:
			result += "bp";
			break;
		case BallVelocity:
			result += "bv";
			break;
		case PlayerScore:
			result += "ps";
			break;
		case PlayerLatency:
			result += "pl";
			break;
		default:
			break;
	}

	result += "[";
	result += to_string(clientId);
	result += "]";
	result += "{";
	result += to_string(value1);
	if (sendType == BallPosition || sendType == BallVelocity)
	{
		result += ",";
		result += to_string(value2);
	}
	result += "}";
	return result;
}
/* called when a client connects */
void openHandler(int clientID){
	if (!bufferInitiated)
	{
		PacketBuffer::StartBuffer();
		PacketBuffer::SetServer((void*)&server);
		bufferInitiated = true;
	}

	std::string ConnectedStr = "ci["+to_string(clientID)+"]{0}";
	PacketBuffer::wsSend(clientID, ConnectedStr);
	if (clientID == 0)
	{
		player1Connected = true;
	}
	if (clientID == 1)
	{
		player2Connected = true;
	}

	if (player1Connected && player2Connected)
	{
		PacketBuffer::wsSend(0, "rd[0]{0}");
		PacketBuffer::wsSend(1, "rd[1]{0}");
	}

	cout << "OpenHandler called on client " << clientID << endl;
}

/* called when a client disconnects */
void closeHandler(int clientID){
    //ostringstream os;
    //os << "Stranger " << clientID << " has leaved.";

    //vector<int> clientIDs = server.getClientIDs();
    //for (int i = 0; i < clientIDs.size(); i++){
    //    if (clientIDs[i] != clientID)
    //        server.wsSend(clientIDs[i], os.str());
    //}

	startGame();
	gameStarted = false;
}

SYSTEMTIME TimeFromString(string s)
{
	//FORMAT: HH:MM:SS:MS
	int colonIndex = s.find_first_of(':');
	string hourStr = s.substr(0, colonIndex);
	s = s.substr(colonIndex + 1);
	colonIndex = s.find_first_of(':');
	string minuteStr = s.substr(0, colonIndex);
	s = s.substr(colonIndex + 1);
	colonIndex = s.find_first_of(':');
	string secondStr = s.substr(0, colonIndex);
	s = s.substr(colonIndex + 1);
	colonIndex = s.find_first_of(':');
	string msStr = s;

	istringstream hoursstream(hourStr);
	istringstream minutesstream(minuteStr);
	istringstream secondsstream(secondStr);
	istringstream mssstream(msStr);
	SYSTEMTIME result;
	GetSystemTime(&result);
	hoursstream >> result.wHour;
	minutesstream >> result.wMinute;
	secondsstream >> result.wSecond;
	mssstream >> result.wMilliseconds;

	return result;
}

int TimeDifference(SYSTEMTIME a, SYSTEMTIME b)
{
	if (a.wSecond == b.wSecond && a.wMilliseconds > b.wMilliseconds)
	{
		return (int)(a.wMilliseconds - b.wMilliseconds);
	}

	//a must be the later time
	if (a.wHour != b.wHour && b.wHour != 59)
	{
		return -1;
	}
	int hourDifference;
	if (a.wHour < b.wHour)
	{
		hourDifference = (24 + a.wHour) - b.wHour;
	}
	else
	{
		hourDifference = a.wHour - b.wHour;
	}
	int minuteDifference = (a.wMinute + 60 * hourDifference) - b.wMinute;
	int secondDifference = (a.wSecond + 60 * minuteDifference) - b.wSecond;
	int msDifference = (a.wMilliseconds + 1000 * secondDifference) - b.wMilliseconds;
	return msDifference;
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
	int openBracketIndex = message.find_first_of('[');
	int closeBracketIndex = message.find_first_of(']');
	string typeString = message.substr(0, openBracketIndex);
	istringstream clientIDsstream(message.substr(openBracketIndex + 1, closeBracketIndex - 1));
	int currentClient;
	clientIDsstream >> currentClient;

	if (typeString.compare("time") == 0)
	{
		if ((currentClient == 0 && !ping1Received)
			|| (currentClient == 1 && !ping2Received))
		{
			SYSTEMTIME serverReceiveTime;
			GetSystemTime(&serverReceiveTime);
			string dataStr = message.substr(closeBracketIndex + 1);
			int commaIndex = dataStr.find_first_of(',');
			string receiveTimeStr = dataStr.substr(0, commaIndex);
			string sendTimeStr = dataStr.substr(commaIndex + 1);
			SYSTEMTIME receiveTime = TimeFromString(receiveTimeStr);
			receiveTime.wDay = serverReceiveTime.wDay;
			receiveTime.wDayOfWeek = serverReceiveTime.wDayOfWeek;
			receiveTime.wMonth = serverReceiveTime.wMonth;
			receiveTime.wYear = serverReceiveTime.wYear;

			SYSTEMTIME sendTime = TimeFromString(sendTimeStr);
			sendTime.wDay = serverReceiveTime.wDay;
			sendTime.wDayOfWeek = serverReceiveTime.wDayOfWeek;
			sendTime.wMonth = serverReceiveTime.wMonth;
			sendTime.wYear = serverReceiveTime.wYear;



			//FILETIME serverSendTimef, receiveTimef, sendTimef, serverReceiveTimef;
			//SystemTimeToFileTime(&serverSendTime, &serverSendTimef);
			//std::cout << " Server Send: " << serverSendTime.wMilliseconds << std::endl;
			//SystemTimeToFileTime(&receiveTime, &receiveTimef);
			//std::cout << "Client Receive: " << receiveTime.wMilliseconds << std::endl;
			//SystemTimeToFileTime(&sendTime, &sendTimef);
			//std::cout << "Client Send: " << sendTime.wMilliseconds << std::endl;
			//SystemTimeToFileTime(&serverReceiveTime, &serverReceiveTimef);
			//std::cout << "Server Receive: " << serverReceiveTime.wMilliseconds << std::endl;
			//ULARGE_INTEGER serverSendTimeui, receiveTimeui, sendTimeui, serverReceiveTimeui;
			//unsigned long long int serverSendTimei, receiveTimei, sendTimei, serverReceiveTimei;
			//serverSendTimeui.LowPart = serverSendTimef.dwLowDateTime;
			//serverSendTimeui.HighPart = serverSendTimef.dwHighDateTime;
			//receiveTimeui.LowPart = receiveTimef.dwLowDateTime;
			//receiveTimeui.HighPart = receiveTimef.dwHighDateTime;
			//sendTimeui.LowPart = sendTimef.dwLowDateTime;
			//sendTimeui.HighPart = sendTimef.dwHighDateTime;
			//serverReceiveTimeui.LowPart = serverReceiveTimef.dwLowDateTime;
			//serverReceiveTimeui.HighPart = serverReceiveTimef.dwHighDateTime;
			//serverSendTimei = serverSendTimeui.QuadPart;
			//receiveTimei = receiveTimeui.QuadPart;
			//sendTimei = receiveTimeui.QuadPart;
			//serverReceiveTimei = serverReceiveTimeui.QuadPart;

			int ServerToClient = 0;
			int ClientToServer = 0;

			//SUBTRACT THE TIMES!
			LatencyManager::AddServerToClientLatency(currentClient, TimeDifference(receiveTime, serverSendTime));
			LatencyManager::AddClientToServerLatency(currentClient, TimeDifference(serverReceiveTime, sendTime));

			if (currentClient == 0)
			{
				ping1Received = true;
			}
			else
			{
				ping2Received = true;
			}

			if (ping1Received && ping2Received)
			{
				pingSent = false;
				ping1Received = false;
				ping2Received = false;
			}
		}
	}

	if (typeString.compare("ready") == 0)
	{
		if (currentClient == 0)
		{
			player1Ready = true;
		}
		if (currentClient == 1)
		{
			player2Ready = true;
		}
	}

	if (typeString.compare("up") == 0 )
	{
		PlayerManager::Players[currentClient]->clientMovementDirection = Paddle::MOVING_UP;
		PlayerManager::Players[currentClient]->packetReceived = true;

    }
	if (typeString.compare("down") == 0)
	{
		PlayerManager::Players[currentClient]->clientMovementDirection = Paddle::MOVING_DOWN;
		PlayerManager::Players[currentClient]->packetReceived = true;
	}
	if (typeString.compare("stop") == 0)
	{
		PlayerManager::Players[currentClient]->clientMovementDirection = Paddle::NOT_MOVING;
		PlayerManager::Players[currentClient]->packetReceived = true;
	}
	if (typeString.compare("name") == 0)
	{
		if (currentClient == 0){
			player1Name = message.substr(closeBracketIndex + 1);
		}
		else
		{
			player2Name = message.substr(closeBracketIndex + 1);
		}
	}
}


void sendPlayerInfo()
{

	string Player1Paddle = FormatPacketString(PaddlePosition, 0, PlayerManager::Players[0]->paddle->position.y, 0);
	string Player2Paddle = FormatPacketString(PaddlePosition, 1, PlayerManager::Players[1]->paddle->position.y, 0);
	string ballPos = FormatPacketString(BallPosition, 0, ball.position.x, ball.position.y);
	string ballVel = FormatPacketString(BallVelocity, 0, ball.velocity.x, ball.velocity.y);
	string Player1Score = FormatPacketString(PlayerScore, 0, PlayerManager::Players[0]->score, 0);
	string Player2Score = FormatPacketString(PlayerScore, 1, PlayerManager::Players[1]->score, 0);
	string Player1Lag = FormatPacketString(PlayerLatency, 0, LatencyManager::AverageClientToServerLatency[0] + LatencyManager::AverageServerToClientLatency[0], 0);
	string Player2Lag = FormatPacketString(PlayerLatency, 1, LatencyManager::AverageClientToServerLatency[1] + LatencyManager::AverageServerToClientLatency[1], 0);

	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); ++i)
	{
		PacketBuffer::wsSend(i, Player1Paddle);
		PacketBuffer::wsSend(i, Player2Paddle);
		PacketBuffer::wsSend(i, ballPos);
		PacketBuffer::wsSend(i, ballVel);
		PacketBuffer::wsSend(i, Player1Score);
		PacketBuffer::wsSend(i, Player2Score);
		PacketBuffer::wsSend(i, Player1Lag);
		PacketBuffer::wsSend(i, Player2Lag);
	}
}

void SendTimePing()
{
	if (!pingSent)
	{
		GetSystemTime(&serverSendTime);
		ostringstream ostream;
		ostream << serverSendTime.wHour << ":" << serverSendTime.wMinute << ":";
		ostream << serverSendTime.wSecond << ":" << serverSendTime.wMilliseconds;
		std::string timePacket = "pg[0]{";
		timePacket += ostream.str() + "}";
		vector<int> clients = server.getClientIDs();
		for (int i = 0; i < clients.size(); ++i)
		{
			PacketBuffer::wsSend(i, timePacket);
		}
		pingSent = true;
	}
}

/* called once per select() loop */
void periodicHandler(){
	if (player1Ready && player2Ready && !gameStarted)
	{
		startGame();
	}

	//cout << " % 33 " << (gameClock - startClock) % 33 << endl;
	if (gameStarted)
	{
		for (int i = 0; i < PlayerManager::playerCount; ++i)
		{
			if (PlayerManager::Players[i]->packetReceived)
			{
				PlayerManager::Players[i]->paddle->dir = PlayerManager::Players[i]->clientMovementDirection;
				PlayerManager::Players[i]->packetReceived = false;
			}
		}

		for (int i = 0; i < PlayerManager::playerCount; ++i)
		{
			PlayerManager::Players[i]->paddle->Update();
		}
		ball.BallUpdate(PlayerManager::Players[0]->paddle, PlayerManager::Players[1]->paddle, topWall, bottomWall);
		sendPlayerInfo();
		SendTimePing();
	}
}

void startGame()
{
	//Start the gameplay
	ball.SetPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
	ball.startingPos.x = SCREEN_WIDTH / 2;
	ball.startingPos.y = SCREEN_WIDTH / 3;
	ball.startingVel.x = -17; //changed the values slightly - J
	ball.startingVel.y = 13;
	ball.velocity.x = -17;
	ball.velocity.y = 13;
	ball.name = "Ball";

	player1.paddle = &paddle1;
	player2.paddle = &paddle2;
	
	PlayerManager::Players[0] = &player1;
	PlayerManager::Players[1] = &player2;

	PlayerManager::Players[0]->paddle->SetPos(0, SCREEN_HEIGHT / 2);
	PlayerManager::Players[0]->paddle->dir = Paddle::NOT_MOVING;
	PlayerManager::Players[0]->paddle->SetHeight(PADDLE_HEIGHT);
	PlayerManager::Players[0]->paddle->SetWidth(PADDLE_WIDTH);
	PlayerManager::Players[0]->paddle->name = "Paddle1";
	PlayerManager::Players[0]->userName = player1Name;
							 		 
	PlayerManager::Players[1]->paddle->SetPos(SCREEN_WIDTH - PADDLE_WIDTH, SCREEN_HEIGHT / 2);
	PlayerManager::Players[1]->paddle->dir = Paddle::NOT_MOVING;
	PlayerManager::Players[1]->paddle->SetHeight(PADDLE_HEIGHT);
	PlayerManager::Players[1]->paddle->SetWidth(PADDLE_WIDTH);
	PlayerManager::Players[1]->paddle->name = "Paddle2";
	PlayerManager::Players[1]->userName = player2Name;

	PacketBuffer::wsSend(0, "na[0]{" + PlayerManager::Players[0]->userName + "}");
	PacketBuffer::wsSend(0, "na[1]{" + PlayerManager::Players[1]->userName + "}");
	PacketBuffer::wsSend(1, "na[0]{" + PlayerManager::Players[0]->userName + "}");
	PacketBuffer::wsSend(1, "na[1]{" + PlayerManager::Players[1]->userName + "}");
	
	topWall.SetPos(0, 0);
	topWall.SetHeight(HORIZ_WALL_WIDTH);
	topWall.SetWidth(HORIZ_WALL_HEIGHT);
	topWall.name = "TopWall";

	bottomWall.SetPos(0, SCREEN_HEIGHT - HORIZ_WALL_HEIGHT);
	bottomWall.SetHeight(HORIZ_WALL_HEIGHT);
	bottomWall.SetWidth(HORIZ_WALL_WIDTH);
	bottomWall.name = "BottomWall";

	//EntityManager::AddEntity((Entity*)&ball);
	//EntityManager::AddEntity((Entity*)&paddle1);
	//EntityManager::AddEntity((Entity*)&topWall);
	//EntityManager::AddEntity((Entity*)&bottomWall);
	//EntityManager::AddEntity((Entity*)&rightWall);
	gameStarted = true;
	pingSent = false;
}

int main(int argc, char *argv[]){
	gameStarted = false;
    int port;

    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
	PacketBuffer::StartBuffer();
	PacketBuffer::SetServer((void*)&server);
}
