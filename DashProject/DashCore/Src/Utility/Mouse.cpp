#include "PCH.h"

#include "Mouse.h"
#include "LogManager.h"

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
		point.x = pos.X;
		point.y = pos.Y;

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
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed = e.LeftButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].RisingEdge = 
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].FallingEdge = 
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed = e.MiddleButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].RisingEdge = 
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].FallingEdge = 
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed = e.RightButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].RisingEdge = 
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].FallingEdge = 
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;

		memcpy(mPrevMouseButtonStates, mCurrentMouseButtonStates, sizeof(mCurrentMouseButtonStates));

		mMousePos.X = e.X;
		mMousePos.Y = e.Y;

		MouseButtonPressed(e);
	}

	void FMouse::OnMouseButtonReleased(FMouseButtonEventArgs& e)
	{
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed = e.LeftButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].RisingEdge =
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].FallingEdge =
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Left)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed = e.MiddleButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].RisingEdge =
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].FallingEdge =
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Middle)].Pressed;

		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed = e.RightButton;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].RisingEdge =
			mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && !mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;
		mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].FallingEdge =
			!mCurrentMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed && mPrevMouseButtonStates[static_cast<unsigned int>(EMouseButton::Right)].Pressed;
		
		memcpy(mPrevMouseButtonStates, mCurrentMouseButtonStates, sizeof(mCurrentMouseButtonStates));

		mMousePos.X = e.X;
		mMousePos.Y = e.Y;

		MouseButtonReleased(e);
	}

	void FMouse::OnMouseMove(FMouseMotionEventArgs& e)
	{
		e.mRelX = e.X - mMousePos.X;
		e.mRelY = e.Y - mMousePos.Y;

		mMousePos.X = e.X;
		mMousePos.Y = e.Y;

		MouseMoved(e);
	}

	void FMouse::OnMouseWheel(FMouseWheelEventArgs& e)
	{
		mMouseWheelAccumulate += e.WheelDelta;

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
	}

	void FMouse::OnMouseWheelUp(FMouseWheelEventArgs& e)
	{
		MouseWheelUp(e);
	}
}