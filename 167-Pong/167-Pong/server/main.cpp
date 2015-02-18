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

bool packetSent = false;

bool player1Connected = false;
bool player2Connected = false;
bool player1Ready = false;
bool player2Ready = false;

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
    //ostringstream os;
    //os << "Stranger " << clientID << " has joined.";

    //vector<int> clientIDs = server.getClientIDs();
    //for (int i = 0; i < clientIDs.size(); i++){
    //    if (clientIDs[i] != clientID)
    //        server.wsSend(clientIDs[i], os.str());
    //}
    //server.wsSend(clientID, "Welcome!");
	//if (clientID == 0)
	//{
		std::string ConnectedStr = "ci["+to_string(clientID)+"]{0}";
		server.wsSend(clientID, ConnectedStr);
		if (clientID == 0)
		{
			player1Connected = true;
		}
		if (clientID == 1)
		{
			player2Connected = true;
		}
	//}
	//if (clientID == 1)
	//{
	//	player2Connected = true;
	//	std::string ConnectedStr = "ci[1]{0}";
	//	server.wsSend(1, ConnectedStr);
	//}

	if (player1Connected && player2Connected)
	{
		server.wsSend(0, "rd[0]{0}");
		server.wsSend(1, "rd[1]{0}");
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

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
    //ostringstream os;
    //os << "Testing the Message Handler";

    //vector<int> clientIDs = server.getClientIDs();
    //for (int i = 0; i < clientIDs.size(); i++){
    //    if (clientIDs[i] != clientID)
    //        server.wsSend(clientIDs[i], os.str());
	cout << "Received Message: " << message << endl;

	int openBracketIndex = message.find_first_of('[');
	int closeBracketIndex = message.find_first_of(']');
	string typeString = message.substr(0, openBracketIndex);
	istringstream clientIDsstream(message.substr(openBracketIndex + 1, closeBracketIndex - 1));
	int currentClient;
	clientIDsstream >> currentClient;

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
//	if (message.substr(0, 5).compare("Time:") == 0)
//	{
//		char packetIdChar = message.at(5);
//		int packetId = packetIdChar - '0';
//		PacketReceivedAtServer.GetNow();
//		std::string times = message.substr(6);
//		int commaIndex = times.find_first_of(",");
//		std::string clientRecived = times.substr(0, commaIndex);
//		std::string clientSent = times.substr(commaIndex + 1);
//		PacketReceivedAtClient.SetFromString(clientRecived);
//		PacketSentFromClient.SetFromString(clientSent);
//
//		PacketBuffer::EnqueueById(PacketReceivedAtClient, 1, packetId);
//		PacketBuffer::EnqueueById(PacketSentFromClient, 2, packetId);
//		PacketBuffer::EnqueueById(PacketReceivedAtServer, 3, packetId);
//
//	}
//
//
//		//if (packetSent && packetReceived)
//		//{
//		//	LatencyManager::CurrentServerToClientLatency = PacketReceivedAtClient - PacketSentFromServer;
//		//	LatencyManager::CurrentClientToServerLatency = PacketReceivedAtServer - PacketSentFromClient;
//		//	LatencyManager::AddServerToClientLatency(PacketReceivedAtClient - PacketSentFromServer);
//		//	LatencyManager::AddClientToServerLatency(PacketReceivedAtServer - PacketSentFromClient);
//
//		//	cout << endl;
//		//	cout << "Current Server To Client: " << LatencyManager::CurrentServerToClientLatency << endl;
//		//	cout << "Current Client To Server: " << LatencyManager::CurrentClientToServerLatency << endl;
//		//	cout << "Estimated Server To Client: " << LatencyManager::AverageServerToClientLatency << endl;
//		//	cout << "Estimated Client To Server: " << LatencyManager::AverageClientToServerLatency << endl;
//		//	cout << endl;
//		//	packetSent = false;
//		//	packetReceived = false;
//		//}
}


void sendPlayerInfo()
{

	string Player1Paddle = FormatPacketString(PaddlePosition, 0, PlayerManager::Players[0]->paddle->position.y, 0);
	string Player2Paddle = FormatPacketString(PaddlePosition, 1, PlayerManager::Players[1]->paddle->position.y, 0);
	string ballPos = FormatPacketString(BallPosition, 0, ball.position.x, ball.position.y);
	string ballVel = FormatPacketString(BallVelocity, 0, ball.velocity.x, ball.velocity.y);
	string Player1Score = FormatPacketString(PlayerScore, 0, PlayerManager::Players[0]->score, 0);
	string Player2Score = FormatPacketString(PlayerScore, 1, PlayerManager::Players[1]->score, 0);

	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); ++i)
	{
		server.wsSend(i, Player1Paddle);
		server.wsSend(i, Player2Paddle);
		server.wsSend(i, ballPos);
		server.wsSend(i, ballVel);
		server.wsSend(i, Player1Score);
		server.wsSend(i, Player2Score);
	}
}

//void sendLatencyTest()
//{
//	PacketSentFromServer.GetNow();
//	std::string timeStamp = "ti";
//	int packetId = PacketBuffer::GetNewPacketId();
//	//if (!PacketBuffer::hasId(packetId) && )
//	timeStamp += PacketSentFromServer.ToString();
//	vector<int> clientIDs = server.getClientIDs();
//	for (int i = 0; i < clientIDs.size(); ++i)
//	{
//		server.wsSend(clientIDs[i], timeStamp);
//	}
//	//packetSent = true;
//}

/* called once per select() loop */
void periodicHandler(){
	if (player1Ready && player2Ready && !gameStarted)
	{
		startGame();
	}

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
	}
}

void startGame()
{
	//Start the gameplay
	ball.SetPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3);
	ball.startingPos.x = SCREEN_WIDTH / 2;
	ball.startingPos.y = SCREEN_WIDTH / 3;
	ball.startingVel.x = -7;
	ball.startingVel.y = 3;
	ball.velocity.x = -7;
	ball.velocity.y = 3;
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

	server.wsSend(0, "na[0]{" + PlayerManager::Players[0]->userName + "}");
	server.wsSend(0, "na[1]{" + PlayerManager::Players[1]->userName + "}");
	server.wsSend(1, "na[0]{" + PlayerManager::Players[0]->userName + "}");
	server.wsSend(1, "na[1]{" + PlayerManager::Players[1]->userName + "}");
	
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
}
