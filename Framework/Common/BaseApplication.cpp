#include "BaseApplication.hpp"

int GameEngine::BaseApplication::Initialize()
{
	m_bQuit = false;
	return 0;
}

void GameEngine::BaseApplication::Finalize()
{

}

void GameEngine::BaseApplication::Tick()
{

}

bool GameEngine::BaseApplication::IsQuit()
{
	return m_bQuit;
}