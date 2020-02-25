#pragma once
#include <iostream> 
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
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
};

