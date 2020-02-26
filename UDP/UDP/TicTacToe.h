#pragma once
#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <vector>
#include <sstream>

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define PLAYER1 1 
#define PLAYER2 2 

#define SIDE 3 // Length of the board 

// Player1 will move with 'O' 
// and Player2 with 'X' 
#define PLAYER1MOVE 'O' 
#define PLAYER2MOVE 'X' 

class GameServer;

class TicTacToe
{
public:
	TicTacToe();

	struct sockaddr_in player1, player2;
	std::string player1Name = "INVALID", player2Name = "INVALID";
	std::string prevPlayer1Name = "INVALID", prevPlayer2Name = "INVALID";
	bool restarted = false;
	int currentTurn = PLAYER1;
	// A 3*3 Tic-Tac-Toe board for playing  
	char board[SIDE][SIDE];
	int moves[SIDE * SIDE];

	void SetServer(GameServer* server) { this->server = server; }

	void playTicTacToe(int whoseTurn, int move);

	bool readyToplay();

	void initialise();

	// A function to declare the winner of the game 
	void declareWinner(int whoseTurn);

	// A function that returns true if any of the row 
	// is crossed with the same player's move 
	bool rowCrossed(char board[][SIDE]);

	// A function that returns true if any of the column 
	// is crossed with the same player's move 
	bool columnCrossed(char board[][SIDE]);

	// A function that returns true if any of the diagonal 
	// is crossed with the same player's move 
	bool diagonalCrossed(char board[][SIDE]);

	// A function that returns true if the game is over 
	// else it returns a false 
	bool gameOver(char board[][SIDE]);

	void restart();

private:
	bool ValidMove(int move);

	GameServer* server;
	int totalMovesDone = 0;
};

