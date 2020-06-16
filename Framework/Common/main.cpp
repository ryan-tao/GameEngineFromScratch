#include <iostream>
#include "../Interface/IApplication.hpp"

using namespace GameEngine;

namespace GameEngine
{
	extern IApplication* g_pApp;
}

int main()
{
	int ret = 0;

	if ((ret = g_pApp->Initialize()) != 0)
	{
		std::cout << "Initialize Failed" << std::endl;
		return ret;
	}

	while (!g_pApp->IsQuit())
	{
		g_pApp->Tick();
	}

	g_pApp->Finalize();
    
    return ret;
}