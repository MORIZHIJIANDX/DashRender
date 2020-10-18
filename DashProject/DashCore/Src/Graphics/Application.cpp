#include "PCH.h"

#include "Application.h"
#include "../Utility/HighResolutionTimer.h"
#include "../Utility/Keyboard.h"
#include "../Utility/Exception.h"

namespace Dash
{
	#define RENDER_WINDOW_CLASS_NAME "RenderWindowClass"

	FApplication::FApplication(size_t windowWidth, size_t windowHeight )
		: mWindow(RENDER_WINDOW_CLASS_NAME, "Test", windowWidth, windowHeight)
		, mIsRunning(false)
	{
		
	}

	FApplication::~FApplication()
	{
	}

	int FApplication::Run()
	{
		ASSERT(!mIsRunning);

		mIsRunning = true;

		std::thread updateThread = std::thread{ &FApplication::UpdateThread, this };
		SetThreadName(updateThread, "UpdateThread");

		int returnCode = mWindow.WindowsMessageLoop();

		mIsRunning = false;

		if (updateThread.joinable())
		{
			updateThread.join();
		}

		return returnCode;
	}

	void FApplication::Stop()
	{
		mWindow.CloseWindow();
	}

	void FApplication::OnRender(const FRenderEventArgs& e)
	{
	}

	void FApplication::OnUpdate(const FUpdateEventArgs& e)
	{
	}

	void FApplication::UpdateThread()
	{
		try
		{
			FHighResolutionTimer timer;

			size_t frameCount = 0;

			while (mIsRunning)
			{
				timer.Update();

				double deltaTime = timer.DeltaSeconds();
				double elapsedTime = timer.ElapsedSeconds();

				mWindow.ProcessMessage();

				OnUpdate(FUpdateEventArgs{ deltaTime, elapsedTime, frameCount });
				OnRender(FRenderEventArgs{ deltaTime, elapsedTime, frameCount });

				++frameCount;

				FKeyboard::Get().FlushRepeatKey();

				std::this_thread::yield();
			}
		}
		catch (const HrException& e)
		{
			HRESULT removeHr = GetDeviceRemoveReason();

			MessageBoxA(NULL, HrToString(e.Error()).c_str(), "Error", MB_ICONERROR);
		}
	}
}