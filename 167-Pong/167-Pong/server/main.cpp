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

using namespace std;

webSocket server;
bool gameStarted;
Ball ball;
Paddle paddle1;
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

void startGame();
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
	startGame();
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
		latencyRatio = LatencyManager::AverageClientToServerLatency / (float)REFRESH_RATE;
		if (paddle1.dir == paddle1.MOVING_DOWN)
		{
			paddle1.position.y -= (int)((float)PADDLE_SPEED * latencyRatio);
		}
		paddle1.dir = paddle1.MOVING_UP;
    }
	if (message.compare("down") == 0)
	{
		latencyRatio = LatencyManager::AverageClientToServerLatency / (float)REFRESH_RATE;
		if (paddle1.dir == paddle1.MOVING_UP)
		{
			paddle1.position.y += (int)((float)PADDLE_SPEED * latencyRatio);
		}
		paddle1.dir = paddle1.MOVING_DOWN;
	}
	if (message.compare("stop") == 0)
	{
		latencyRatio = LatencyManager::AverageClientToServerLatency / (float)REFRESH_RATE;
		if (paddle1.dir == paddle1.MOVING_UP)
		{
			paddle1.position.y += (int)((float)PADDLE_SPEED * latencyRatio);
		}
		else if (paddle1.dir == paddle1.MOVING_DOWN)
		{
			paddle1.position.y -= (int)((float)PADDLE_SPEED * latencyRatio);
		}
		paddle1.dir = paddle1.NOT_MOVING;
	}
	if (message.substr(0, 5).compare("Name:") == 0)
	{
		PlayerManager::id = message.substr(5);
	}
	if (message.substr(0, 5).compare("Time:") == 0)
	{
		if (packetSent && !packetReceived)
		{
			PacketReceivedAtServer.GetNow();
			std::string times = message.substr(5);
			int commaIndex = times.find_first_of(",");
			std::string clientRecived = times.substr(0, commaIndex);
			std::string clientSent = times.substr(commaIndex + 1);
			PacketReceivedAtClient.SetFromString(clientRecived);
			PacketSentFromClient.SetFromString(clientSent);
			packetReceived = true;
		}


		if (packetSent && packetReceived)
		{
			LatencyManager::AddServerToClientLatency(PacketReceivedAtClient - PacketSentFromServer);
			LatencyManager::AddClientToServerLatency(PacketReceivedAtServer - PacketSentFromClient);

			cout << endl;
			cout << "Estimated Server To Client: " << LatencyManager::AverageServerToClientLatency << endl;
			cout << "Estimated Client To Server: " << LatencyManager::AverageClientToServerLatency << endl;
			cout << endl;
			packetSent = false;
			packetReceived = false;
		}
	}
}

void sendPlayerInfo()
{
	latencyRatio = LatencyManager::AverageServerToClientLatency / (float)REFRESH_RATE;
	Vector2 estimatedPaddlePos;
	if (paddle1.dir == paddle1.NOT_MOVING)
	{
		estimatedPaddlePos = paddle1.position;
	}
	else if (paddle1.dir == paddle1.MOVING_UP)
	{
		int estY = paddle1.position.y - (int)((float)PADDLE_SPEED * latencyRatio );
		if (estY < 0)
		{
			estY = 0;
		}
		estimatedPaddlePos.x = paddle1.position.x;
		estimatedPaddlePos.y = estY;
	}
	else
	{
		int estY = paddle1.position.y + (int)((float)PADDLE_SPEED * latencyRatio);
		if (estY + paddle1.height > SCREEN_HEIGHT)
		{
			estY = SCREEN_HEIGHT - paddle1.height;
		}
		estimatedPaddlePos.x = paddle1.position.x;
		estimatedPaddlePos.y = estY;
	}
	Vector2 estimatedBallPos;
	estimatedBallPos.x = ball.position.x + (int)((float)(ball.velocity.x) * latencyRatio);
	estimatedBallPos.y = ball.position.y + (int)((float)(ball.velocity.y) * latencyRatio);
	vector<int> clientIDs = server.getClientIDs();
	std::string paddleString = "pp" + to_string(estimatedPaddlePos.y);
	std::string ballPosString = "bp" + to_string(estimatedBallPos.x) + "," + to_string(estimatedBallPos.y);
	std::string ballVelString = "bv" + to_string(estimatedBallPos.x) + "," + to_string(estimatedBallPos.y);
	std::string consecScoreString = "cs" + to_string(PlayerManager::consecutive_hits);
	std::string totalScoreString = "ts" + to_string(PlayerManager::score);
	std::string totalOppString = "to" + to_string(PlayerManager::score + PlayerManager::failures);
	//SEND PADDLE POS

	for (int i = 0; i < clientIDs.size(); ++i)
	{
		server.wsSend(clientIDs[i], paddleString);
		server.wsSend(clientIDs[i], ballPosString);
		server.wsSend(clientIDs[i], ballVelString);
		server.wsSend(clientIDs[i], consecScoreString);
		server.wsSend(clientIDs[i], totalScoreString);
		server.wsSend(clientIDs[i], totalOppString);
	}
}

void sendLatencyTest()
{
	PacketSentFromServer.GetNow();
	std::string timeStamp = "ti";
	timeStamp += PacketSentFromServer.ToString();
	vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); ++i)
	{
		server.wsSend(clientIDs[i], timeStamp);
	}
	packetSent = true;
}

/* called once per select() loop */
void periodicHandler(){
	static clock_t baseClock = clock();
	clock_t t = clock();
	if (gameStarted)
	{
		if (((int)t - (int)baseClock) % REFRESH_RATE == 0 ){
			if ( !packetSent && !packetReceived)
			{
				sendLatencyTest();

			}
			
			paddle1.Update();
			ball.Update();
			sendPlayerInfo();
		}
	}
}

void startGame()
{
	//Start the gameplay
	ball.SetPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);
	ball.startingPos.x = SCREEN_WIDTH / 2;
	ball.startingPos.y = SCREEN_WIDTH / 4;
	ball.startingVel.x = -2;
	ball.startingVel.y = 1;
	ball.velocity.x = -2;
	ball.velocity.y = 1;
	ball.name = "Ball";
	paddle1.SetPos(0, SCREEN_HEIGHT / 2);
	paddle1.dir = paddle1.NOT_MOVING;
	paddle1.SetHeight(PADDLE_HEIGHT);
	paddle1.SetWidth(PADDLE_WIDTH);
	paddle1.name = "Paddle1";
	topWall.SetPos(PADDLE_WIDTH, 0);
	topWall.SetHeight(HORIZ_WALL_WIDTH);
	topWall.SetWidth(HORIZ_WALL_HEIGHT);
	topWall.name = "TopWall";
	bottomWall.SetPos(PADDLE_WIDTH, SCREEN_HEIGHT - HORIZ_WALL_HEIGHT);
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
	PlayerManager::consecutive_hits = 0;
	PlayerManager::failures = 0;
	PlayerManager::score;
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
