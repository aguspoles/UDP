#include "stdafx.h"
#include "TicTacToe.h"
#include "GameServer.h"

void TicTacToe::playTicTacToe(int whoseTurn, int moveIndex)
{
	if (currentTurn != whoseTurn)
		return;
	// A 3*3 Tic-Tac-Toe board for playing  
	char board[SIDE][SIDE];

	int moves[SIDE * SIDE];

	// Initialise the game 
	initialise(board, moves);

	int x, y;

	// Keep playing till the game is over or it is a draw 
	if (gameOver(board) == false &&
		moveIndex != SIDE * SIDE)
	{
		if (whoseTurn == PLAYER1)
		{
			x = moves[moveIndex] / SIDE;
			y = moves[moveIndex] % SIDE;
			board[x][y] = PLAYER1MOVE;

			server->SendMove(moves[moveIndex] + 1, board, player1);
			
			moveIndex++;
			whoseTurn = PLAYER2;
			currentTurn = PLAYER2;
		}

		else if (whoseTurn == PLAYER2)
		{
			x = moves[moveIndex] / SIDE;
			y = moves[moveIndex] % SIDE;
			board[x][y] = PLAYER2MOVE;

			server->SendMove(moves[moveIndex] + 1, board, player2);

			moveIndex++;
			whoseTurn = PLAYER1;
			currentTurn = PLAYER1;
		}
	}

	// If the game has drawn 
	/*if (gameOver(board) == false &&
		moveIndex == SIDE * SIDE)
		printf("It's a draw\n");
	else
	{
		// Toggling the user to declare the actual 
		// winner 
		if (whoseTurn == PLAYER1)
			whoseTurn = PLAYER2;
		else if (whoseTurn == PLAYER2)
			whoseTurn = PLAYER1;

		// Declare the winner 
		declareWinner(whoseTurn);
	}*/
	return;
}

void TicTacToe::initialise(char board[][SIDE], int moves[])
{
	// Initiate the random number generator so that  
		// the same configuration doesn't arises 
	//srand(time(NULL));

	// Initially the board is empty 
	for (int i = 0; i < SIDE; i++)
	{
		for (int j = 0; j < SIDE; j++)
			board[i][j] = ' ';
	}

	for (int i = 0; i < SIDE * SIDE; i++)
		moves[i] = i;

	return;
}

void TicTacToe::declareWinner(int whoseTurn)
{
	if (whoseTurn == PLAYER1)
		printf("PLAYER1 has won\n");
	else
		printf("PLAYER2 has won\n");
	return;
}

bool TicTacToe::rowCrossed(char board[][SIDE])
{
	for (int i = 0; i < SIDE; i++)
	{
		if (board[i][0] == board[i][1] &&
			board[i][1] == board[i][2] &&
			board[i][0] != ' ')
			return (true);
	}
	return(false);
}

bool TicTacToe::columnCrossed(char board[][SIDE])
{
	for (int i = 0; i < SIDE; i++)
	{
		if (board[0][i] == board[1][i] &&
			board[1][i] == board[2][i] &&
			board[0][i] != ' ')
			return (true);
	}
	return(false);
}

bool TicTacToe::diagonalCrossed(char board[][SIDE])
{
	if (board[0][0] == board[1][1] &&
		board[1][1] == board[2][2] &&
		board[0][0] != ' ')
		return(true);

	if (board[0][2] == board[1][1] &&
		board[1][1] == board[2][0] &&
		board[0][2] != ' ')
		return(true);

	return(false);
}

bool TicTacToe::gameOver(char board[][SIDE])
{
	return(rowCrossed(board) || columnCrossed(board)
		|| diagonalCrossed(board));
}
