#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Paddle.h"
#include "Ball.h"
#include "Wall.h"
#include "Constants.h"
#include "PlayerManager.h"

using namespace std;

webSocket server;
bool gameStarted;
Ball ball;
Paddle paddle1;
Wall topWall;
Wall bottomWall;
Wall rightWall;

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

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message){
    //ostringstream os;
    //os << "Testing the Message Handler";

    //vector<int> clientIDs = server.getClientIDs();
    //for (int i = 0; i < clientIDs.size(); i++){
    //    if (clientIDs[i] != clientID)
    //        server.wsSend(clientIDs[i], os.str());


	if (message.compare("up") == 0 )
	{
		paddle1.dir = paddle1.MOVING_UP;
    }
	else if (message.compare("down") == 0)
	{
		paddle1.dir = paddle1.MOVING_DOWN;
	}
	else if (message.compare("stop") == 0)
	{
		paddle1.dir = paddle1.NOT_MOVING;
	}
	else if (message.substr(0, 5).compare("Name:") == 0)
	{
		PlayerManager::id = message.substr(5);
	}
}

void sendPlayerInfo()
{
	vector<int> clientIDs = server.getClientIDs();
	std::string paddleString = "pp";
	std::string ballPosString = "bp";
	std::string ballVelString = "bv";
	std::string consecScoreString = "cs" + to_string(PlayerManager::consecutive_hits);
	std::string totalScoreString = "ts" + to_string(PlayerManager::score);
	std::string totalOppString = "to" + to_string(PlayerManager::score + PlayerManager::failures);
	//SEND PADDLE POS
	for (int i = 0; i < EntityManager::AllEntities.size(); ++i)
	{
		Entity* current = &EntityManager::AllEntities[i];
		if (EntityManager::AllEntities[i].name.compare("Paddle1") == 0)
		{
			paddleString += to_string(current->position.y);
		}

		if (EntityManager::AllEntities[i].name.compare("Ball") == 0)
		{
			Ball* ball = (Ball*)current;
			ballPosString += to_string(current->position.x) + "," + to_string(current->position.y);
			ballVelString += to_string(ball->velocity.x) + "," + to_string(ball->velocity.y);
		}
	}

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

/* called once per select() loop */
void periodicHandler(){
	PlayerManager::consecutive_hits++;
	static time_t next = time(NULL) +REFRESH_RATE;
	time_t current = time(NULL);
	if (gameStarted)
	{
		if (current >= next){
			paddle1.Update();
			ball.Update();
			sendPlayerInfo();
		}
	}

	next = time(NULL) +REFRESH_RATE;
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
