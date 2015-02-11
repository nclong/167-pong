#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include "websocket.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Ball.h"
#include "Wall.h"
#include "Constants.h"

using namespace std;

webSocket server;

/* called when a client connects */
void openHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has joined.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
    server.wsSend(clientID, "Welcome!");
}

/* called when a client disconnects */
void closeHandler(int clientID){
    ostringstream os;
    os << "Stranger " << clientID << " has leaved.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }
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
		//Set Paddle Velocity to up
    }
	else if (message.compare("down") == 0)
	{
		//Set Paddle Velocity to down
	}
}

void sendPlayerInfo()
{
	vector<int> clientIDs = server.getClientIDs();
	std::string paddleString;
	//SEND PADDLE POS
	for (int i = 0; clientIDs.size(); ++i)
	{
		server.wsSend(clientIDs[i], )
	}
	//SEND BALL POS
	//SEND BALL VEL
	//SEND CONSECUTIVE SCORE
	//SEND TOTAL SCORE
	//SEND TOTAL OPPORTUNITY SCORE
	return;
}

/* called once per select() loop */
void periodicHandler(){
    static time_t next = time(NULL) + REFRESH_RATE;
    time_t current = time(NULL);
    if (current >= next){
		//ADD ENTITY UPDATES
		//SEND PADDLE AND BALL POSITIONS
		//SEND SCORES
		for (int i = 0; i < EntityManager::AllEntities.size; ++i)
		{
			EntityManager::AllEntities[i].Update();
		}
		sendPlayerInfo();

        next = time(NULL) + REFRESH_RATE;
    }
}

void startGame()
{
	//Start the gameplay
	//EntityManager::AddEntity(new Ball(new Vector2(5, 10) "The Ball"));

}

int main(int argc, char *argv[]){
    int port;

    cout << "Please set server port: ";
    cin >> port;

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(port);

    return 1;
}
