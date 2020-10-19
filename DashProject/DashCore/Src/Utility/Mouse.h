#pragma once

#include <bitset>

namespace Dash
{
	class FMouse
	{
		friend class FWindow;
		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	public:
		static FMouse& Get();
		
		bool IsMouseButtonPressed(EMouseButton button) const;
		bool IsInWindow() const;

		FKeyState GetButtonState(EMouseButton button) const;
		
		FVector2i GetCursorPosition() const;
		void SetCursorPosition(FVector2i pos);

		void Initialize(HWND hwnd);

	public:

		FMouseButtonEvent MouseButtonPressed;
		FMouseButtonEvent MouseButtonReleased;
		FMouseMotionEvent MouseMoved;
		FMouseWheelEvent MouseWheelUp;
		FMouseWheelEvent MouseWheelDown;
		
	private:
		FMouse() : mFocusedWindow(NULL), mMouseWheelAccumulate(0), mIsInWindow(false) {};
		~FMouse() {};

		void OnMouseButtonPressed(FMouseButtonEventArgs& e);
		void OnMouseButtonReleased(FMouseButtonEventArgs& e);

		void OnMouseMove(FMouseMotionEventArgs& e);
		void OnMouseWheel(FMouseWheelEventArgs& e);
		
		void OnMouseLeave(FMouseMotionEventArgs& e);
		void OnMouseEnter(FMouseMotionEventArgs& e);

		void OnMouseWheelDown(FMouseWheelEventArgs& e);
		void OnMouseWheelUp(FMouseWheelEventArgs& e);

	private:
		FKeyState mCurrentMouseButtonStates[3];
		FKeyState mPrevMouseButtonStates[3];
		FVector2i mMousePos;

		HWND mFocusedWindow;
		float mMouseWheelAccumulate;
		bool mIsInWindow;
	};
}