#include "PCH.h"

#include "Mouse.h"
#include "LogManager.h"
#include "LogStream.h"

namespace Dash
{
	FMouse& FMouse::Get()
	{	
		static FMouse instance;
		return instance;
	}

	bool FMouse::IsMouseButtonPressed(EMouseButton button) const
	{
		return mCurrentMouseButtonStates[static_cast<unsigned int>(button)].Pressed;
	}

	bool FMouse::IsInWindow() const
	{
		return mIsInWindow;
	}

	FKeyState FMouse::GetButtonState(EMouseButton button) const
	{
		return mCurrentMouseButtonStates[static_cast<unsigned int>(button)];
	}

	FVector2i FMouse::GetCursorPosition() const
	{
		return mMousePos;
	}

	void FMouse::SetCursorPosition(FVector2i pos)
	{
		POINT point;
		point.x = pos.x;
		point.y = pos.y;

		if (mFocusedWindow != NULL)
			if (!ClientToScreen(mFocusedWindow, &point))
				LOG_ERROR << "Can't Transform Cursor Pos To Screen";

		if (!::SetCursorPos(point.x, point.y))
			LOG_ERROR << "Can't Set Cursor Pos";
	}

	void FMouse::Initialize(HWND hwnd)
	{
		mFocusedWindow = hwnd;
	}

	void FMouse::OnMouseButtonPressed(FMouseButtonEventArgs& e)
	{
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed = e.mLeftButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].RisingEdge = 
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].FallingEdge = 
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed = e.mMiddleButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].RisingEdge = 
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].FallingEdge = 
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed = e.mRightButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].RisingEdge = 
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].FallingEdge = 
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;

		memcpy(mPrevMouseButtonStates, mCurrentMouseButtonStates, sizeof(mCurrentMouseButtonStates));

		mMousePos.x = e.mX;
		mMousePos.y = e.mY;

		MouseButtonPressed(e);
	}

	void FMouse::OnMouseButtonReleased(FMouseButtonEventArgs& e)
	{
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed = e.mLeftButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].RisingEdge =
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].FallingEdge =
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed = e.mMiddleButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].RisingEdge =
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].FallingEdge =
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed = e.mRightButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].RisingEdge =
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].FallingEdge =
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;
		
		memcpy(mPrevMouseButtonStates, mCurrentMouseButtonStates, sizeof(mCurrentMouseButtonStates));

		mMousePos.x = e.mX;
		mMousePos.y = e.mY;

		MouseButtonReleased(e);
	}

	void FMouse::OnMouseMove(FMouseMotionEventArgs& e)
	{
		e.mRelX = e.mX - mMousePos.x;
		e.mRelY = e.mY - mMousePos.y;

		mMousePos.x = e.mX;
		mMousePos.y = e.mY;

		MouseMoved(e);
	}

	void FMouse::OnMouseWheel(FMouseWheelEventArgs& e)
	{
		mMouseWheelAccumulate += e.mWheelDelta;

		if (mMouseWheelAccumulate >= WHEEL_DELTA)
		{
			mMouseWheelAccumulate -= WHEEL_DELTA;
			OnMouseWheelUp(e);
		}
		else if (mMouseWheelAccumulate <= -WHEEL_DELTA)
		{
			mMouseWheelAccumulate += WHEEL_DELTA;
			OnMouseWheelDown(e);
		}
	}

	void FMouse::OnMouseLeave(FMouseMotionEventArgs& e)
	{
		mIsInWindow = false;
	}

	void FMouse::OnMouseEnter(FMouseMotionEventArgs& e)
	{
		mIsInWindow = true;
	}

	void FMouse::OnMouseWheelDown(FMouseWheelEventArgs& e)
	{
		MouseWheelDown(e);

		LOG_INFO << "FMouse Wheel Down";
	}

	void FMouse::OnMouseWheelUp(FMouseWheelEventArgs& e)
	{
		MouseWheelUp(e);

		LOG_INFO << "FMouse Wheel Up";
	}
}