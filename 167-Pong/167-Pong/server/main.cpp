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

// Game elements //

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
bool bufferInitiated = false;
bool gameStarted;

bool player1Connected = false;
bool player2Connected = false;
bool player1Ready = false;
bool player2Ready = false;
bool ping1Received = false;
bool ping2Received = false;



void startGame();

string FormatPacketString(SendTypes sendType, int clientId, int value1, int value2)
{
	//Packet Format
	//<function>[<clientID>]{<data>}
	// We'll append specific string args 
	// to a packet sent so the recieving client can decipher them.

	string result = ""; 
	switch (sendType) // Use switch statement and break upon appending
	{
		// send packet indicating that paddle position is changing
		case PaddlePosition:
			result += "pp";
			break;
		// send packet indicating that ball position is changing
		case BallPosition:
			result += "bp";
			break;
		// send packet indicating that ball velocity is changing
		case BallVelocity:
			result += "bv";
			break;
		// send packet indicating that player score is changing
		case PlayerScore:
			result += "ps";
			break;
		// send packet indicating that paddle latency is changing
		case PlayerLatency:
			result += "pl";
			break;
		default:
			break;
	}

	// We want to format the packet for the client.
	// Sample format: pp[1]{}#SystemTime

	result += "[";
	result += to_string(clientId); // We add the clientID
	result += "]";
	result += "{";
	result += to_string(value1);

	// For Ballposition and velocity, we need to have additional param
	// Since we have the X and Y parameters that we need to send.

	if (sendType == BallPosition || sendType == BallVelocity)
	{
		result += ",";
		result += to_string(value2);
	}
	result += "}#";

	// Send the exact system time over to the client.

	SYSTEMTIME st;
	GetSystemTime(&st);
	std::ostringstream ossMessage;

	// Some complex stuff magic here. Nick!
	ossMessage << st.wYear << "-"
		<< std::setw(2) << std::setfill('0') << st.wMonth << "-"
		<< std::setw(2) << std::setfill('0') << st.wDay << " "
		<< std::setw(2) << std::setfill('0') << st.wHour << ":"
		<< std::setw(2) << std::setfill('0') << st.wMinute << ":"
		<< std::setw(2) << std::setfill('0') << st.wSecond << "."
		<< std::setw(3) << std::setfill('0') << st.wMilliseconds;
	result += ossMessage.str();
	return result; 
}
/* called when a client connects */
void openHandler(int clientID){

	 // Init the buffer if it hasn't been already

	if (!bufferInitiated)
	{
		PacketBuffer::StartBuffer();
		PacketBuffer::SetServer((void*)&server);
		bufferInitiated = true;
	}

	std::string ConnectedStr = "ci["+to_string(clientID)+"]{0}";
	PacketBuffer::wsSend(clientID, ConnectedStr);

	// Checks connectivity for the players here

	if (clientID == 0)
	{
		player1Connected = true; 
	}
	if (clientID == 1)
	{
		player2Connected = true;
	}

	// Once both clients connected, send packet to both indicating "ready"

	if (player1Connected && player2Connected) 
	{
		PacketBuffer::wsSend(0, "rd[0]{0}");
		PacketBuffer::wsSend(1, "rd[1]{0}");
	}

	// Print 
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

			// Wait for ping 1 and ping 2.

			if (currentClient == 0)
			{
				ping1Received = true;
			}
			else if (currentClient == 1)
			{
				ping2Received = true;
			}

			// If both pings are recieved, set them back to false
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
		else if (currentClient == 1)
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
		// Have the client sleep for 2000 ms to compensate for latency
		if (currentClient == 0){
			player1Name = message.substr(closeBracketIndex + 1); // sets player 1 name here; evidently is not being called when needed.
			Sleep(2000);
		}
		else if (currentClient == 1)
		{
			player2Name = message.substr(closeBracketIndex + 1);
			Sleep(2000);
		}
	}
}


void sendPlayerInfo()
{

	// For player info, send PlayerPaddlePositions, score, ball pos/vel, and their lag.

	string Player1Paddle = FormatPacketString(PaddlePosition, 0, PlayerManager::Players[0]->paddle->position.y, 0);
	string Player2Paddle = FormatPacketString(PaddlePosition, 1, PlayerManager::Players[1]->paddle->position.y, 0);
	
	string ballPos = FormatPacketString(BallPosition, 0, ball.position.x, ball.position.y);
	string ballVel = FormatPacketString(BallVelocity, 0, ball.velocity.x, ball.velocity.y);
	
	string Player1Score = FormatPacketString(PlayerScore, 0, PlayerManager::Players[0]->score, 0);
	string Player2Score = FormatPacketString(PlayerScore, 1, PlayerManager::Players[1]->score, 0);
	
	//string Player1Lag = FormatPacketString(PlayerLatency, 0, LatencyManager::AverageClientToServerLatency[0] + LatencyManager::AverageServerToClientLatency[0], 0);
	//string Player2Lag = FormatPacketString(PlayerLatency, 1, LatencyManager::AverageClientToServerLatency[1] + LatencyManager::AverageServerToClientLatency[1], 0);
	
	string Player1Lag = FormatPacketString(PlayerLatency, 0, LatencyManager::GetCurrentServerToClientLatency(0) + LatencyManager::GetCurrentClientToServerLatency(0), 0);
	string Player2Lag = FormatPacketString(PlayerLatency, 1, LatencyManager::GetCurrentClientToServerLatency(1) + LatencyManager::GetCurrentServerToClientLatency(1), 0);

	// Send the same packets to all clients

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

	// Start the game! Is run every .33 seconds.

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
	
	topWall.SetPos(0, -100);			// offset seems to fix the borders issue.
	topWall.SetHeight(HORIZ_WALL_WIDTH);
	topWall.SetWidth(HORIZ_WALL_HEIGHT);
	topWall.name = "TopWall";

	bottomWall.SetPos(0, SCREEN_HEIGHT - HORIZ_WALL_HEIGHT - 50);
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
=======
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
	result += "}#";
	SYSTEMTIME st;
	GetSystemTime(&st);
	std::ostringstream ossMessage;

	ossMessage << st.wYear << "-"
		<< std::setw(2) << std::setfill('0') << st.wMonth << "-"
		<< std::setw(2) << std::setfill('0') << st.wDay << "T"
		<< std::setw(2) << std::setfill('0') << st.wHour << ":"
		<< std::setw(2) << std::setfill('0') << st.wMinute << ":"
		<< std::setw(2) << std::setfill('0') << st.wSecond << "."
		<< std::setw(3) << std::setfill('0') << st.wMilliseconds
		<< "+00:00";
	result += ossMessage.str();
	return result;
}

//string DayOfWeekFromNum(int x)
//{
//	switch (x)
//	{
//	case 0:
//		return "Sun";
//	case 1:
//		return "Mon";
//	case 2:
//		return "Tue";
//	case 3:
//		return "Wed";
//	case 4:
//		return "Thu";
//	case 5":
//		return "Fri";
//	case "6":
//		return "Sat";
//	default:
//		return "NO STOP IT";
//	}
//}
//
//string MonthFromNum(int x)
//{
//	switch (x)
//	{
//		case 0:
//			return "Jan";
//		case 1:
//			return "Feb";
//		case 2:
//			return "Mar";
//		case 3:
//			return "Apr";
//		case 4:
//			return "May";
//		case 5:
//			return "Jun";
//		case 6:
//			return "Jul";
//		case 7:
//			return "Aug";
//		case 8:
//			return "Sep";
//		case 9:
//			return "Oct";
//		case 10:
//			return "Nov";
//		case 11:
//			return "Dec";
//		default:
//			"NO STOP IT";
//	}
//}
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
			else if (currentClient == 1)
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
		else if (currentClient == 1)
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
			player1Name = message.substr(closeBracketIndex + 1); // sets player 1 name here; evidently is not being called when needed.
			Sleep(2000);
		}
		else if (currentClient == 1)
		{
			player2Name = message.substr(closeBracketIndex + 1);
			Sleep(2000);
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
	//string Player1Lag = FormatPacketString(PlayerLatency, 0, LatencyManager::AverageClientToServerLatency[0] + LatencyManager::AverageServerToClientLatency[0], 0);
	//string Player2Lag = FormatPacketString(PlayerLatency, 1, LatencyManager::AverageClientToServerLatency[1] + LatencyManager::AverageServerToClientLatency[1], 0);
	string Player1Lag = FormatPacketString(PlayerLatency, 0, LatencyManager::GetCurrentServerToClientLatency(0) + LatencyManager::GetCurrentClientToServerLatency(0), 0);
	string Player2Lag = FormatPacketString(PlayerLatency, 1, LatencyManager::GetCurrentClientToServerLatency(1) + LatencyManager::GetCurrentServerToClientLatency(1), 0);



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
	
	topWall.SetPos(0, -100);			// offset seems to fix the borders issue.
	topWall.SetHeight(HORIZ_WALL_WIDTH);
	topWall.SetWidth(HORIZ_WALL_HEIGHT);
	topWall.name = "TopWall";

	bottomWall.SetPos(0, SCREEN_HEIGHT - HORIZ_WALL_HEIGHT - 50);
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
