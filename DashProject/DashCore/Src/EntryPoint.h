#pragma once
#include "Consolid/Consolid.h"
#include "Framework/GameApp.h"
#include "Utility/LogManager.h"

namespace Dash
{
	DECLARE_LOG_CATEGORY(LogTemp, All);
}

extern Dash::IGameApp* CreateApplication();