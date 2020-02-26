#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdio.h>
#include<winsock2.h>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <algorithm>
#include "TicTacToe.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library
 
#define SERVER "127.0.0.1"  //ip address of udp server
#define BUFLEN 1024  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

bool logged = false;
std::string playerName;
TicTacToe game;
enum class MSGCODE : int32_t {LogIn = 1, StartingGame, Chat, MoveMade, EndGameStatus, Other, ClientLogged, Move, Restart };

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

void RecvThread(SOCKET s, struct sockaddr_in si_other)
{
	int slen = sizeof(si_other);
	Message msg;
	while (1)
	{
		//receive a reply and print it
		//try to receive some data, this is a blocking call
		if (recvfrom(s, (char*)&msg, sizeof(Message), 0, (struct sockaddr*)&si_other, &slen) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
			exit(EXIT_FAILURE);
		}


		if (msg.code == MSGCODE::LogIn && !logged)
		{
			char userInput[BUFLEN];
			printf(msg.message);

			fflush(stdout);
			memset(userInput, '\0', BUFLEN);
			gets_s(userInput);

			//send the message
			Message response;
			response.code = MSGCODE::ClientLogged;
			memcpy(response.message, userInput, BUFLEN);
			sendto(s, (char*)& response, sizeof(Message), 0, (struct sockaddr*) & si_other, slen);
			logged = true;
			playerName = userInput;
			printf("Wait players to connect...");
		}
		if (msg.code == MSGCODE::StartingGame)
		{
			printf("%s\n\n", msg.message);
			game.gameEnded = false;
			game.showBoard(msg.board);
		}
		if (msg.code == MSGCODE::Chat)
		{
			std::cout << msg.message << std::endl;
		}
		if (msg.code == MSGCODE::MoveMade)
		{
			game.showBoard(msg.board);
			printf("%s\n", msg.message);
		}
		if (msg.code == MSGCODE::EndGameStatus)
		{
			game.showBoard(msg.board);
			game.gameEnded = true;
			printf("%s\n", msg.message);
		}
		if(msg.code == MSGCODE::Other)
		{
			printf("%s\n", msg.message);
		}
		if (msg.code == MSGCODE::Restart)
		{
			printf("%s\n", msg.message);
		}
	}
}
 
int main(void)
{
    struct sockaddr_in si_other;
    int s, slen=sizeof(si_other);
    char message[BUFLEN];
    WSADATA wsa;
 
    //Initialise winsock
    //printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    //printf("Initialised.\n");
     
    //create socket
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
     
    //setup address structure
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);

	game.showInstructions();

	//first login message
	Message msg;
	msg.code = MSGCODE::LogIn;
	if (sendto(s, (char*)&msg, sizeof(Message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
    
	//Move recive funcion to another thread, so it is not blockable
	//and keep the send one in the current thread
	std::thread recv(RecvThread, s, si_other);

    //start communication
    while(1)
    {
		if (logged && !game.gameEnded)
		{
			fflush(stdout);
			memset(message, '\0', BUFLEN);
			
			gets_s(message);

			Message msg;
			if (std::atoi(message) >= 1 && std::atoi(message) <= 9)
			{
				msg.code = MSGCODE::Move;
				msg.move = std::atoi(message);
			}
			else
			{
				msg.code = MSGCODE::Chat;
				std::stringstream ss;
				ss << playerName << ": " << message;
				memcpy(msg.message, ss.str().c_str(), sizeof(ss));
				std::cout << playerName << ": " << message << std::endl;
			}

			//send the message
			if (sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & si_other, slen) == SOCKET_ERROR)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
					exit(EXIT_FAILURE);
			}
		}
		else if (game.gameEnded)
		{
			fflush(stdout);
			std::string restart;

			std::getline(std::cin, restart);

			if(restart == "y")
			{
				Message msg;
				msg.code = MSGCODE::Restart;
				msg.move = 0;
				sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & si_other, slen);
			}
			else if (restart == "n")
			{
				Message msg;
				msg.code = MSGCODE::Restart;
				msg.move = -1;
				sendto(s, (char*)& msg, sizeof(Message), 0, (struct sockaddr*) & si_other, slen);
			}

		}

	}
 
    closesocket(s);
    WSACleanup();
 
    return 0;
}