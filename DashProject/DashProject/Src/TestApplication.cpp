#include "PCH/PCH.h"
#include "TestApplication.h"
#include "Utility/LogManager.h"
#include "Utility/Keyboard.h"
#include "Utility/Mouse.h"

#include "Graphics/GraphicsCore.h"


#include <string>
#include "Graphics/SwapChain.h"
#include "Graphics/CommandContext.h"

#include "Utility/FileUtility.h"
#include "Graphics/ShaderMap.h"

namespace Dash
{
	TestApplication::TestApplication()
		: IGameApp()
	{
	}

	TestApplication::~TestApplication()
	{
	}
}