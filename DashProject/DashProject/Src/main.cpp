#include <iostream>
#include <chrono>

#include "Consolid/Consolid.h"
#include "Graphics/ApplicationDX12.h"
#include "Utility/LogManager.h"
#include "Utility/LogStream.h"

#include "TestApplication.h"


namespace DMath = Dash::FMath;

//int CALLBACK WinMain(HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR lpCmdLine,
//	int nCmdShow)
//{
//	Dash::FLogManager::Get()->Init();
//	Dash::FLogManager::Get()->RegisterLogStream(std::make_shared<Dash::FLogStreamConsole>());
//
//	Dash::FApplicationDX12 app;
//	app.Run();
//
//	Dash::FLogManager::Get()->Shutdown();
//}

Dash::IGameApp* CreateApplication()
{
	return new Dash::TestApplication();
}