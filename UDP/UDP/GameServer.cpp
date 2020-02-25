#include "stdafx.h"
#include "GameServer.h"

void GameServer::Run()
{
	this->Init();

	Message msg;
	//keep listening for data
	while (1)
	{
		printf("Waiting for data...\n");
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(&msg.message, '\0', BUFLEN);
		memset(&msg.code, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		recvfrom(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & si_other, &slen);

		ReciveMessage(msg);

		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Operation: %s\n", msg.code.c_str());
	}

	this->Destroy();
}

void GameServer::Init()
{
	slen = sizeof(si_other);

	WSADATA wsa;
	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Bind
	if (bind(s, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done\n");
}

void GameServer::Destroy()
{
	closesocket(s);
	WSACleanup();
}

void GameServer::ReciveMessage(Message m)
{
	if (m.code == "LogIn")
	{
		LogInPlayer();
		return;
	}
	if (m.code == "Chat")
	{
		Chat(m);
		return;
	}
	if (m.code == "ClientLogged")
	{
		ClientLogged(m);	
		return;
	}
	if (m.code == "Move")
	{
		Move(m);
		return;
	}
}

void GameServer::StartGame()
{
	Message msg;
	msg.code = "StartingGame";
	msg.message = "Starting...";
	//now reply the client with the same data
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & games.back().player1, slen);
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & games.back().player2, slen);
}

void GameServer::SendMove(int move, char board[3][3], struct sockaddr_in player)
{
	std::stringstream ss;
	ss << "PLAYER1 has put a " << PLAYER1MOVE << " in cell " << move;
	Message reply;
	reply.code = "MoveMade";
	reply.message = ss.str();
	reply.move = move;
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			reply.board[i][j] = board[i][j];
		}
	}
	sendto(s, (char*)& reply, sizeof(Message), 0, (struct sockaddr*) & si_other, slen);
	//sendto(s, (char*)& reply, sizeof(Message), 0, (struct sockaddr*) & player, slen);
}

void GameServer::LogInPlayer()
{
	if (std::find(playersPorts.begin(), playersPorts.end(), si_other.sin_port) != playersPorts.end())
		return;
	players.push_back(si_other);
	playersPorts.push_back(si_other.sin_port);

	Message reply;
	reply.code = "LogIn";
	reply.message = "UserName: ";
	sendto(s, (char*)&reply, sizeof(Message), 0, (struct sockaddr*) & si_other, slen);
}

void GameServer::ClientLogged(Message m)
{
	printf("Client %s logged\n", m.message.c_str());
	if (games.size() == 0)
	{
		TicTacToe game;
		game.SetServer(this);
		game.player1 = si_other;
		game.player1Name = m.message;
		games.push_back(game);
	}
	else
	{
		if (games.back().readyToplay)
		{
			TicTacToe game;
			game.SetServer(this);
			game.player1 = si_other;
			game.player1Name = m.message;
			games.push_back(game);
		}
		else
		{
			games.back().player2 = si_other;
			games.back().player2Name = m.message;
			games.back().readyToplay = true;
			StartGame();
		}
	}
}

void GameServer::Chat(Message msg)
{
	for(TicTacToe var : games)
	{
		if (var.player1.sin_port == si_other.sin_port)
		{
			std::stringstream ss;
			ss << var.player1Name << ": " << msg.message;
			msg.message = ss.str();
			sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & var.player2, slen);
			break;
		}
		if (var.player2.sin_port == si_other.sin_port)
		{
			std::stringstream ss;
			ss << var.player2Name << ": " << msg.message;
			msg.message = ss.str();
			sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & var.player1, slen);
			break;
		}
	}
}

void GameServer::Move(Message m)
{
	for (TicTacToe var : games)
	{
		if (var.player1.sin_port == si_other.sin_port)
		{
			var.playTicTacToe(PLAYER1, m.move);
			break;
		}
		if (var.player2.sin_port == si_other.sin_port)
		{
			var.playTicTacToe(PLAYER2, m.move);
			break;
		}
	}
}
