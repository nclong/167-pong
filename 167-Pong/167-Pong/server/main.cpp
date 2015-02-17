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
bool gameStarted;
Ball ball;
PlayerInfo player1;
PlayerInfo player2;
Wall topWall;
Wall bottomWall;
Wall rightWall;
PongTime PacketSentFromServer;
PongTime PacketReceivedAtClient;
PongTime PacketSentFromClient;
PongTime PacketReceivedAtServer;

float latencyRatio;

bool packetSent = false;
bool packetReceived = false;

bool player1Connected = false;
bool player2Connected = false;

Paddle::MovementDirection clientPaddleDirection;

void startGame();
string FormatPacketString(SendTypes sendType, int clientId, int value1, int value2)
{
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
		startGame();
	}
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

void updateLatencyInfo()
{
	static bool cycleComplete = true;
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

	if (message.compare("up") == 0 )
	{
		//latencyRatio = LatencyManager::AverageClientToServerLatency / (float)REFRESH_RATE;
		//if (paddle1.dir == paddle1.MOVING_DOWN)
		//{
		//	paddle1.position.y -= (int)((float)PADDLE_SPEED * latencyRatio);
		//}
		clientPaddleDirection = Paddle::MOVING_UP;
		packetReceived = true;

    }
	if (message.compare("down") == 0)
	{
		//latencyRatio = LatencyManager::AverageClientToServerLatency / (float)REFRESH_RATE;
		//if (paddle1.dir == paddle1.MOVING_UP)
		//{
		//	paddle1.position.y += (int)((float)PADDLE_SPEED * latencyRatio);
		//}
		clientPaddleDirection = Paddle::MOVING_DOWN;
		packetReceived = true;
	}
	if (message.compare("stop") == 0)
	{
		latencyRatio = LatencyManager::AverageClientToServerLatency / (float)REFRESH_RATE;
		//if (paddle1.dir == paddle1.MOVING_UP)
		//{
		//	paddle1.position.y += (int)((float)PADDLE_SPEED * latencyRatio);
		//}
		//else if (paddle1.dir == paddle1.MOVING_DOWN)
		//{
		//	paddle1.position.y -= (int)((float)PADDLE_SPEED * latencyRatio);
		//}
		clientPaddleDirection = Paddle::NOT_MOVING;
		packetReceived = true;
	}
	if (message.substr(0, 5).compare("Name:") == 0)
	{
		//Set Player Stuff
	}
	if (message.substr(0, 5).compare("Time:") == 0)
	{
		char packetIdChar = message.at(5);
		int packetId = packetIdChar - '0';
		PacketReceivedAtServer.GetNow();
		std::string times = message.substr(6);
		int commaIndex = times.find_first_of(",");
		std::string clientRecived = times.substr(0, commaIndex);
		std::string clientSent = times.substr(commaIndex + 1);
		PacketReceivedAtClient.SetFromString(clientRecived);
		PacketSentFromClient.SetFromString(clientSent);

		PacketBuffer::EnqueueById(PacketReceivedAtClient, 1, packetId);
		PacketBuffer::EnqueueById(PacketSentFromClient, 2, packetId);
		PacketBuffer::EnqueueById(PacketReceivedAtServer, 3, packetId);

	}


		//if (packetSent && packetReceived)
		//{
		//	LatencyManager::CurrentServerToClientLatency = PacketReceivedAtClient - PacketSentFromServer;
		//	LatencyManager::CurrentClientToServerLatency = PacketReceivedAtServer - PacketSentFromClient;
		//	LatencyManager::AddServerToClientLatency(PacketReceivedAtClient - PacketSentFromServer);
		//	LatencyManager::AddClientToServerLatency(PacketReceivedAtServer - PacketSentFromClient);

		//	cout << endl;
		//	cout << "Current Server To Client: " << LatencyManager::CurrentServerToClientLatency << endl;
		//	cout << "Current Client To Server: " << LatencyManager::CurrentClientToServerLatency << endl;
		//	cout << "Estimated Server To Client: " << LatencyManager::AverageServerToClientLatency << endl;
		//	cout << "Estimated Client To Server: " << LatencyManager::AverageClientToServerLatency << endl;
		//	cout << endl;
		//	packetSent = false;
		//	packetReceived = false;
		//}
}

void sendPlayerInfo()
{
	//latencyRatio = LatencyManager::AverageServerToClientLatency / (float)REFRESH_RATE;
	//Vector2 estimatedPaddlePos;
	//if (paddle1.dir == paddle1.NOT_MOVING)
	//{
	//	estimatedPaddlePos = paddle1.position;
	//}
	//else if (paddle1.dir == paddle1.MOVING_UP)
	//{
	//	int estY = paddle1.position.y - (int)((float)PADDLE_SPEED * latencyRatio );
	//	if (estY < 0)
	//	{
	//		estY = 0;
	//	}
	//	estimatedPaddlePos.x = paddle1.position.x;
	//	estimatedPaddlePos.y = estY;
	//}
	//else
	//{
	//	int estY = paddle1.position.y + (int)((float)PADDLE_SPEED * latencyRatio);
	//	if (estY + paddle1.height > SCREEN_HEIGHT)
	//	{
	//		estY = SCREEN_HEIGHT - paddle1.height;
	//	}
	//	estimatedPaddlePos.x = paddle1.position.x;
	//	estimatedPaddlePos.y = estY;
	//}
	//Vector2 estimatedBallPos;
	//estimatedBallPos.x = ball.position.x + (int)((float)(ball.velocity.x) * latencyRatio);
	//estimatedBallPos.y = ball.position.y + (int)((float)(ball.velocity.y) * latencyRatio);
	
	string Player1Paddle = FormatPacketString(PaddlePosition, 0, PlayerManager::Players[0].paddle.position.y, 0);
	string Player2Paddle = FormatPacketString(PaddlePosition, 1, PlayerManager::Players[1].paddle.position.y, 0);
	string ballPos = FormatPacketString(BallPosition, 0, ball.position.x, ball.position.y);
	string ballVel = FormatPacketString(BallVelocity, 0, ball.velocity.x, ball.velocity.y);
	string Player1Score = FormatPacketString(PlayerScore, 0, PlayerManager::Players[0].score, 0);
	string Player2Score = FormatPacketString(PlayerScore, 0, PlayerManager::Players[1].score, 0);

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

void sendLatencyTest()
{
	PacketSentFromServer.GetNow();
	std::string timeStamp = "ti";
	int packetId = PacketBuffer::GetNewPacketId();
	//if (!PacketBuffer::hasId(packetId) && )
	timeStamp += PacketSentFromServer.ToString();
	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); ++i)
	{
		server.wsSend(clientIDs[i], timeStamp);
	}
	//packetSent = true;
}

/* called once per select() loop */
void periodicHandler(){
	if (gameStarted)
	{
		if (packetReceived)
		{
			paddle1.dir = clientPaddleDirection;
			packetReceived = false;
		}
		paddle1.Update();
		ball.BallUpdate(paddle1, topWall, bottomWall, rightWall);
		sendPlayerInfo();
			

		//if (((int)t - (int)baseClock) % LATENCY_POLL_RATE == 0){
		//	if (!packetSent && !packetReceived)
		//	{
		//		sendPlayerInfo();
		//		sendLatencyTest();

		//	}
		//}
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
	paddle1.SetPos(0, SCREEN_HEIGHT / 2);
	paddle1.dir = paddle1.NOT_MOVING;
	paddle1.SetHeight(PADDLE_HEIGHT);
	paddle1.SetWidth(PADDLE_WIDTH);
	paddle1.name = "Paddle1";
	topWall.SetPos(0, 0);
	topWall.SetHeight(HORIZ_WALL_WIDTH);
	topWall.SetWidth(HORIZ_WALL_HEIGHT);
	topWall.name = "TopWall";
	bottomWall.SetPos(0, SCREEN_HEIGHT - HORIZ_WALL_HEIGHT);
	bottomWall.SetHeight(HORIZ_WALL_HEIGHT);
	bottomWall.SetWidth(HORIZ_WALL_WIDTH);
	bottomWall.name = "BottomWall";
	rightWall.SetPos(SCREEN_WIDTH - VERT_WALL_WIDTH, 0);
	rightWall.SetHeight(VERT_WALL_HEIGHT);
	rightWall.SetWidth(VERT_WALL_WIDTH);
	rightWall.name = "RightWall";
	EntityManager::AddEntity((Entity*)&ball);
	EntityManager::AddEntity((Entity*)&paddle1);
	EntityManager::AddEntity((Entity*)&topWall);
	EntityManager::AddEntity((Entity*)&bottomWall);
	EntityManager::AddEntity((Entity*)&rightWall);
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
