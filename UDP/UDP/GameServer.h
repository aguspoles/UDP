#pragma once
#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "TicTacToe.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 1024  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

enum class MSGCODE : int32_t { LogIn = 1, StartingGame, Chat, MoveMade, EndGameStatus, Other, ClientLogged, Move, Restart };

struct Message {
	Message()
	{
		ZeroMemory(message, BUFLEN);
	}
	MSGCODE code;
	char message[BUFLEN];
	int move;
	char board[3][3];
};

class GameServer
{
public:
	void Run();
private:
	void Init();
	void Destroy();
	void ReciveMessage(Message m);

	//SendMessages
	void StartGame();
	void SendMove(int player, int move, char board[3][3], TicTacToe* game);
	void SendEndGameStatus(std::string status, TicTacToe* game);
	void SendWaitForYouTurn(std::string status, TicTacToe* game);
	void SendInvalidMove(std::string status, TicTacToe* game);
	void SendIsYourTurn(TicTacToe* game);
	void SendRestart(TicTacToe* game);

	//ReciveMessages
	void LogInPlayer();
	void ClientLogged(Message m);
	void Chat(Message m);
	void Move(Message m);
	void ReciveRestart(Message m);

	SOCKET s;
	struct sockaddr_in server, si_other;
	std::vector<int> playersPorts;
	std::vector<TicTacToe> games;
	std::vector<struct sockaddr_in> playersWaiting;
	int slen, recv_len;

	friend class TicTacToe;
};


