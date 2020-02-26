#pragma once
#include <iostream> 
#include<stdio.h>
using namespace std;

#define SIDE 3 // Length of the board 

class TicTacToe
{
public:
	// A function to show the instructions 
	void showInstructions();

	// A function to show the current board status 
	void showBoard(char board[SIDE][SIDE]);

	bool gameEnded = false;
};

