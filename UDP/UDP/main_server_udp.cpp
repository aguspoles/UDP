#include "stdafx.h"

#include "GameServer.h"
 
int main()
{
	GameServer* server = new GameServer();
	server->Run();
	delete server;
 
    return 0;
}