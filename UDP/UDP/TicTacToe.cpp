#include "stdafx.h"
#include "TicTacToe.h"
#include "GameServer.h"

TicTacToe::TicTacToe() : restarted(false)
{
}

void TicTacToe::playTicTacToe(int whoseTurn, int moveIndex)
{
	if (currentTurn != whoseTurn)
	{
		server->SendWaitForYouTurn("Wait for your turn!", this);
		return;
	}
	else if (!ValidMove(moveIndex))
	{
		server->SendInvalidMove("Invalid move!", this);
		return;
	}

	int x, y;

	// Keep playing till the game is over or it is a draw 
	if (gameOver(board) == false && totalMovesDone != SIDE * SIDE)
	{
		if (whoseTurn == PLAYER1)
		{
			x = moves[moveIndex - 1] / SIDE;
			y = moves[moveIndex - 1] % SIDE;
			board[x][y] = PLAYER1MOVE;

			server->SendMove(PLAYER1, moveIndex, board, this);
			
			whoseTurn = PLAYER2;
			currentTurn = PLAYER2;
			totalMovesDone++;

			server->SendIsYourTurn(this);
		}

		else if (whoseTurn == PLAYER2)
		{
			x = moves[moveIndex - 1] / SIDE;
			y = moves[moveIndex - 1] % SIDE;
			board[x][y] = PLAYER2MOVE;

			server->SendMove(PLAYER2, moves[moveIndex], board, this);

			whoseTurn = PLAYER1;
			currentTurn = PLAYER1;
			totalMovesDone++;

			server->SendIsYourTurn(this);
		}

	}

	// If the game has drawn 
	if (gameOver(board) == false &&
		totalMovesDone == SIDE * SIDE)
	{
		server->SendEndGameStatus("It's a draw", this);
	}
	else if(gameOver(board))
	{
		// Toggling the user to declare the actual 
		// winner 
		if (whoseTurn == PLAYER1)
			whoseTurn = PLAYER2;
		else if (whoseTurn == PLAYER2)
			whoseTurn = PLAYER1;

		// Declare the winner 
		declareWinner(whoseTurn);
	}
	return;
}

bool TicTacToe::readyToplay()
{
	return (player1Name != "INVALID" && player2Name != "INVALID");
}

void TicTacToe::initialise()
{
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
	std::stringstream ss;
	if (whoseTurn == PLAYER1)
		ss << "Player " << player1Name << " won!!!";
	else
		ss << "Player " << player2Name << " won!!!";
	server->SendEndGameStatus(ss.str(), this);
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

void TicTacToe::restart()
{
	totalMovesDone = 0;
	prevPlayer1Name = player1Name;
	prevPlayer2Name = player2Name;
	player1Name = player2Name = "INVALID";
	restarted = true;
	currentTurn = PLAYER1;
	initialise();
}

bool TicTacToe::ValidMove(int move)
{
	int x = moves[move - 1] / SIDE;
	int y = moves[move - 1] % SIDE;
	if (board[x][y] != ' ')
		return false;
	return true;
}
