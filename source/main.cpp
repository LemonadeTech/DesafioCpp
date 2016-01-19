/*
 * main.cpp
 *
 *  Created on: Jan 15, 2016
 *      Author: fabiano
 */

#include "GameApp.h"

int main(int argc, char *argv[])
{
    GameApp theGame("Game Engine Test", 800, 600);
	theGame.InitSystem();
	theGame.Run();
    return 0;
}
