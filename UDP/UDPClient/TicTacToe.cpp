#include "stdafx.h"
#include "TicTacToe.h"

void TicTacToe::showInstructions()
{
	{
		printf("\t\t\t  Tic-Tac-Toe\n\n");
		printf("Choose a cell numbered from 1 to 9 as below"
			" and play\n\n");

		printf("\t\t\t  1 | 2  | 3  \n");
		printf("\t\t\t--------------\n");
		printf("\t\t\t  4 | 5  | 6  \n");
		printf("\t\t\t--------------\n");
		printf("\t\t\t  7 | 8  | 9  \n\n");

		printf("-\t-\t-\t-\t-\t-\t-\t-\t-\t-\n\n");

		return;
	}
}

void TicTacToe::showBoard(char board[SIDE][SIDE])
{
	printf("\n\n");

	printf("\t\t\t  %c | %c  | %c  \n", board[0][0],
		board[0][1], board[0][2]);
	printf("\t\t\t--------------\n");
	printf("\t\t\t  %c | %c  | %c  \n", board[1][0],
		board[1][1], board[1][2]);
	printf("\t\t\t--------------\n");
	printf("\t\t\t  %c | %c  | %c  \n\n", board[2][0],
		board[2][1], board[2][2]);

	return;
}
