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

#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

struct Message {
	std::string code;
	std::string message;
	int move;
	char board[3][3];
	struct sockaddr_in from;
	struct sockaddr_in to;
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
	void SendMove(int move, char board[3][3], struct sockaddr_in player1);

	//reciveMessages
	void LogInPlayer();
	void ClientLogged(Message m);
	void Chat(Message m);
	void Move(Message m);

	SOCKET s;
	struct sockaddr_in server, si_other;
	std::vector<struct sockaddr_in> players;
	std::vector<int> playersPorts;
	std::vector<TicTacToe> games;
	int slen, recv_len;
	char buf[BUFLEN];

	friend class TicTacToe;
};


