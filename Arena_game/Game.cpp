#include "Game.h"

Game::Game()
{
	run = true;
	pause = false;
}

Game::~Game() {}

void Game::SetPlayers(unsigned new_players)
{
	if (new_players > 1)
		default_players = new_players;
}
unsigned Game::GetPlayers()
{
	return default_players;
}

void Game::SetMapType(bool isRound)
{
	map_type = isRound;
}
void Game::SetMapRadius(unsigned r)
{
	if(r >= 100)
		default_radius = r;
}
void Game::SetMapSizes(unsigned w , unsigned h)
{
	if (w >= 100)
		default_width = w;
	if (h >= 100)
		default_height = w;
}
bool Game::GetMapType()
{
	return map_type;
}
unsigned Game::GetMapRadius()
{
	return default_radius;
}
unsigned Game::GetMapWidth()
{
	return default_width;
}
unsigned Game::GetMapHeight()
{
	return default_height;
}

int Game::Execute()
{
	while(run)

	return State();
}

bool Game::State()
{
	return pause;
}

void Game::Pause()
{
	pause = true;
}

void Game::UnPause()
{
	pause = false;
}

void Game::Exit()
{
	run = false;
}