#include <iostream>
#include <chrono>

#include "Consolid/Consolid.h"
#include "Utility/Events.h"
#include "Utility/LogManager.h"

#include "TestApplication.h"

Dash::IGameApp* CreateApplication()
{
	return new Dash::TestApplication();
}