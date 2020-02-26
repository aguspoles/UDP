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

		//try to receive some data, this is a blocking call
		recvfrom(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & si_other, &slen);

		ReciveMessage(msg);

		//print details of the client/peer and the data received
		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		printf("Operation: %d\n", msg.code);
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
	if (m.code == MSGCODE::LogIn)
	{
		LogInPlayer();
		return;
	}
	if (m.code == MSGCODE::Chat)
	{
		Chat(m);
		return;
	}
	if (m.code == MSGCODE::ClientLogged)
	{
		ClientLogged(m);	
		return;
	}
	if (m.code == MSGCODE::Move)
	{
		Move(m);
		return;
	}
	if (m.code == MSGCODE::Restart)
	{
		ReciveRestart(m);
		return;
	}
}

void GameServer::StartGame()
{
	for (TicTacToe& var : games)
	{
		if (var.player1.sin_port == si_other.sin_port || var.player2.sin_port == si_other.sin_port)
		{
			var.restarted = false;
			var.initialise();

			Message msg;
			Message msg2;
			msg.code = MSGCODE::StartingGame;
			msg2.code = MSGCODE::StartingGame;
			std::stringstream ss;
			ss << "Starting...\n\nYour opponent is " << games.back().player1Name;
			memcpy(msg.message, ss.str().c_str(), sizeof(ss));
			std::stringstream ss2;
			ss2 << "Starting...\n\nYour opponent is " << games.back().player2Name;
			memcpy(msg2.message, ss2.str().c_str(), sizeof(ss2));
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < 3; j++)
				{
					msg.board[i][j] = games.back().board[i][j];
					msg2.board[i][j] = games.back().board[i][j];
				}
			}

			sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & games.back().player2, slen);
			sendto(s, (char*)& msg2, sizeof(Message), 0, (struct sockaddr*) & games.back().player1, slen);

			SendIsYourTurn(&var);
			break;
		}
	}

}

void GameServer::SendMove(int player, int move, char board[3][3], TicTacToe* game)
{
	std::stringstream ss;
	if (player == PLAYER1)
	{
		ss << game->player1Name << " has put a " << PLAYER1MOVE << " in cell " << move;
	}
	else
		ss << game->player2Name << " has put a " << PLAYER2MOVE << " in cell " << move;

	Message reply;
	reply.code = MSGCODE::MoveMade;
	memcpy(reply.message, ss.str().c_str(), sizeof(ss));
	reply.move = move;
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			reply.board[i][j] = board[i][j];
		}
	}
	sendto(s, (char*)& reply, sizeof(Message), 0, (struct sockaddr*) & game->player1, slen);
	sendto(s, (char*)& reply, sizeof(Message), 0, (struct sockaddr*) & game->player2, slen);
}

void GameServer::SendEndGameStatus(std::string status, TicTacToe* game)
{
	Message msg;
	msg.code = MSGCODE::EndGameStatus;
	memcpy(msg.message, status.c_str(), sizeof(status));
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			msg.board[i][j] = game->board[i][j];
		}
	}
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player1, slen);
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player2, slen);

	SendRestart(game);
}

void GameServer::SendWaitForYouTurn(std::string status, TicTacToe* game)
{
	Message msg;
	msg.code = MSGCODE::Other;
	memcpy(msg.message, status.c_str(), sizeof(status));
	if(game->currentTurn == PLAYER1)
		sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player2, slen);
	else
		sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player1, slen);
}

void GameServer::SendInvalidMove(std::string status, TicTacToe* game)
{
	Message msg;
	msg.code = MSGCODE::Other;
	memcpy(msg.message, status.c_str(), sizeof(status));
	if (game->currentTurn == PLAYER1)
		sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player1, slen);
	else
		sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player2, slen);
}

void GameServer::SendIsYourTurn(TicTacToe* game)
{
	std::stringstream ss;
	ss << "It is ";
	if (game->currentTurn == PLAYER1)
		ss << game->player1Name << " turn";
	else
		ss << game->player2Name << " turn";

	Message msg;
	msg.code = MSGCODE::Other;
	memcpy(msg.message, ss.str().c_str(), sizeof(ss));
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player1, slen);
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player2, slen);
}

void GameServer::SendRestart(TicTacToe* game)
{
	Message msg;
	msg.code = MSGCODE::Restart;
	std::string text = "Do you want to play again? press y/n for yes/no";
	memcpy(msg.message, text.c_str(), sizeof(text));
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player1, slen);
	sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & game->player2, slen);
}

void GameServer::LogInPlayer()
{
	if (std::find(playersPorts.begin(), playersPorts.end(), si_other.sin_port) != playersPorts.end())
		return;
	playersPorts.push_back(si_other.sin_port);

	Message reply;
	reply.code = MSGCODE::LogIn;
	std::string text = "Login to play, pick your user name: ";
	memcpy(reply.message, text.c_str(), sizeof(text));
	sendto(s, (char*)&reply, sizeof(Message), 0, (struct sockaddr*) & si_other, slen);
}

void GameServer::ClientLogged(Message m)
{
	printf("Client %s logged\n", m.message);
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
		//If the last game is reday, i create one
		if (games.back().readyToplay())
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
			StartGame();
		}
	}
}

void GameServer::Chat(Message msg)
{
	//Simple redirect the message
	for(TicTacToe var : games)
	{
		if (var.player1.sin_port == si_other.sin_port)
		{
			sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & var.player2, slen);
			break;
		}
		if (var.player2.sin_port == si_other.sin_port)
		{
			sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & var.player1, slen);
			break;
		}
	}
}

void GameServer::Move(Message m)
{
	for (TicTacToe& var : games)
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

void GameServer::ReciveRestart(Message m)
{
	TicTacToe* gameToRestart = nullptr;
	for (TicTacToe& var : games)
	{
		if (var.player1.sin_port == si_other.sin_port || var.player2.sin_port == si_other.sin_port)
		{
			gameToRestart = &var;
			break;
		}
	}

	if(gameToRestart && !gameToRestart->restarted)
		gameToRestart->restart();

	if (m.move == 0)
	{
		if (gameToRestart->player1.sin_port == si_other.sin_port)
			gameToRestart->player1Name = gameToRestart->prevPlayer1Name;
		else if (gameToRestart->player2.sin_port == si_other.sin_port)
			gameToRestart->player2Name = gameToRestart->prevPlayer2Name;

		if (gameToRestart->readyToplay())
		{
			StartGame();
		}
	}
	else if(m.move == -1)
	{
		std::string prevPlayer1Name = gameToRestart->prevPlayer1Name;
		std::string prevPlayer2Name = gameToRestart->prevPlayer2Name;
		int player1Port = gameToRestart->player1.sin_port, player2Port = gameToRestart->player2.sin_port;

		auto it = games.begin();
		while (it != games.end())
		{
			if (it->player1.sin_port == gameToRestart->player1.sin_port)
			{
				it = games.erase(it);
			}
			else
				++it;
		}
		auto it2 = playersPorts.begin();
		while (it2 != playersPorts.end())
		{
			if (*it2 == si_other.sin_port)
			{
				it2 = playersPorts.erase(it2);
			}
			else
				++it2;
		}

		if (player1Port == si_other.sin_port)
		{
			Message fakeMsg;
			fakeMsg.code = MSGCODE::ClientLogged;
			memcpy(fakeMsg.message, prevPlayer2Name.c_str(), sizeof(prevPlayer2Name));
			ClientLogged(fakeMsg);
		}
		else if (player2Port == si_other.sin_port)
		{
			Message fakeMsg;
			fakeMsg.code = MSGCODE::ClientLogged;
			memcpy(fakeMsg.message, prevPlayer1Name.c_str(), sizeof(prevPlayer1Name));
			ClientLogged(fakeMsg);
		}
	}
}
