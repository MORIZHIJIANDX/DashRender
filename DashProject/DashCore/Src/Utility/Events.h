#pragma once

#include "KeyCodes.h"
#include "DesignPatterns/MulticastDelegate.h"

#include <memory>
#include <string>

namespace Dash
{
	struct FEventArgs
	{
	public:
		FEventArgs()
		{}
	};

	using FEvent = TMulticastDelegate<void(FEventArgs&)>;
	using FEventDelegate = TDelegate<void(FEventArgs&)>;

	enum class EKeyState
	{
		Released = 0,
		Pressed = 1
	};

	struct FKeyEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FKeyEventArgs(EKeyCode key, unsigned int c, EKeyState state, bool control, bool shift, bool alt, bool repeat)
			: Key(key)
			, Char(c)
			, State(state)
			, Control(control)
			, Shift(shift)
			, Alt(alt)
			, Repeat(repeat)
		{}

		EKeyCode        Key;    // The Key Code that was pressed or released.
		unsigned int    Char;   // The 32-bit character code that was pressed. This value will be 0 if it is a non-printable character.
		EKeyState       State;  // Was the key pressed or released?
		bool            Control;// Is the Control modifier pressed
		bool            Shift;  // Is the Shift modifier pressed
		bool            Alt;    // Is the Alt modifier pressed
		bool			Repeat;
	};

	using FKeyboardEvent = TMulticastDelegate<void(FKeyEventArgs&)>;
	using FKeyboardEventDelegate = TDelegate<void(FKeyEventArgs&)>;


	struct FMouseMotionEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FMouseMotionEventArgs(bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
			: LeftButton(leftButton)
			, MiddleButton(middleButton)
			, RightButton(rightButton)
			, Control(control)
			, Shift(shift)
			, X(x)
			, Y(y)
			, mRelX(0)
			, mRelY(0)
		{}

		bool LeftButton;    // Is the left mouse button down?
		bool MiddleButton;  // Is the middle mouse button down?
		bool RightButton;   // Is the right mouse button down?
		bool Control;       // Is the CTRL key down?
		bool Shift;         // Is the Shift key down?

		int X;              // The X-position of the cursor relative to the upper-left corner of the client area.
		int Y;              // The Y-position of the cursor relative to the upper-left corner of the client area.
		int mRelX;			// How far the mouse moved since the last event.
		int mRelY;			// How far the mouse moved since the last event.
	};

	using FMouseMotionEvent = TMulticastDelegate<void(FMouseMotionEventArgs&)>;
	using FMouseMotionEventDelegate = TDelegate<void(FMouseMotionEventArgs&)>;


	enum class EMouseButton : unsigned int
	{
		Left = 0x00,
		Right = 0x01,
		Middle = 0x02,
		None = 0x03,
	};

	enum class EButtonState
	{
		Released = 0,
		Pressed = 1
	};

	struct FMouseButtonEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FMouseButtonEventArgs(EMouseButton buttonID, EButtonState state, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
			: mButton(buttonID)
			, State(state)
			, LeftButton(leftButton)
			, MiddleButton(middleButton)
			, RightButton(rightButton)
			, Control(control)
			, Shift(shift)
			, X(x)
			, Y(y)
		{}

		EMouseButton mButton; // The mouse button that was pressed or released.
		EButtonState State;  // Was the button pressed or released?
		bool LeftButton;    // Is the left mouse button down?
		bool MiddleButton;  // Is the middle mouse button down?
		bool RightButton;   // Is the right mouse button down?
		bool Control;       // Is the CTRL key down?
		bool Shift;         // Is the Shift key down?

		int X;              // The X-position of the cursor relative to the upper-left corner of the client area.
		int Y;              // The Y-position of the cursor relative to the upper-left corner of the client area.
	};

	using FMouseButtonEvent = TMulticastDelegate<void(FMouseButtonEventArgs&)>;
	using FMouseButtonEventDelegate = TDelegate<void(FMouseButtonEventArgs&)>;


	struct FMouseWheelEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FMouseWheelEventArgs(float wheelDelta, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
			: WheelDelta(wheelDelta)
			, LeftButton(leftButton)
			, MiddleButton(middleButton)
			, RightButton(rightButton)
			, Control(control)
			, Shift(shift)
			, X(x)
			, Y(y)
		{}

		float WheelDelta;   // How much the mouse wheel has moved. A positive value indicates that the wheel was moved to the right. A negative value indicates the wheel was moved to the left.
		bool LeftButton;    // Is the left mouse button down?
		bool MiddleButton;  // Is the middle mouse button down?
		bool RightButton;   // Is the right mouse button down?
		bool Control;       // Is the CTRL key down?
		bool Shift;         // Is the Shift key down?

		int X;              // The X-position of the cursor relative to the upper-left corner of the client area.
		int Y;              // The Y-position of the cursor relative to the upper-left corner of the client area.

	};

	using FMouseWheelEvent = TMulticastDelegate<void(FMouseWheelEventArgs&)>;
	using FMouseWheelEventDelegate = TDelegate<void(FMouseWheelEventArgs&)>;


	struct FResizeEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FResizeEventArgs(int width, int height, bool minimize)
			: Width(width)
			, Height(height)
			, Minimized(minimize)
		{}

		// The new width of the window
		int Width;
		// The new height of the window.
		int Height;

		bool Minimized;
	};

	using FResizeEvent = TMulticastDelegate<void(FResizeEventArgs&)>;
	using FResizeEventDelegate = TDelegate<void(FResizeEventArgs&)>;


	struct FUpdateEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;
		FUpdateEventArgs(double fDeltaTime, double fTotalTime, uint64_t frameCounter)
			: ElapsedTime(fDeltaTime)
			, TotalTime(fTotalTime)
			, FrameCounter(frameCounter)
		{}

		double ElapsedTime;
		double TotalTime;
		uint64_t FrameCounter;
	};

	using FUpdateEvent = TMulticastDelegate<void(FUpdateEventArgs&)>;
	using FUpdateEventDelegate = TDelegate<void(FUpdateEventArgs&)>;


	struct FRenderEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;
		FRenderEventArgs(double fDeltaTime, double fTotalTime,
			uint64_t frameCounter)
			: ElapsedTime(fDeltaTime)
			, TotalTime(fTotalTime)
			, FrameCounter(frameCounter)
		{}

		double ElapsedTime;
		double TotalTime;
		uint64_t FrameCounter;
	};

	using FRenderEvent = TMulticastDelegate<void(FRenderEventArgs&)>;
	using FRenderEventDelegate = TDelegate<void(FRenderEventArgs&)>;


	struct FUserEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;
		FUserEventArgs(int code, void* data1, void* data2)
			: Code(code)
			, Data1(data1)
			, Data2(data2)
		{}

		int Code;
		void* Data1;
		void* Data2;
	};

	using FUserEvent = TMulticastDelegate<void(FUserEventArgs&)>;
	using FUserEventDelegate = TDelegate<void(FUserEventArgs&)>;


	struct FRuntimeErrorEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FRuntimeErrorEventArgs(const std::string& errorString, const std::string& compilerError)
			: ErrorString(errorString)
			, CompilerError(compilerError)
		{}

		std::string ErrorString;
		std::string CompilerError;
	};

	using FRuntimeErrorEvent = TMulticastDelegate<void(FRuntimeErrorEventArgs&)>;
	using FRuntimeErrorEventDelegate = TDelegate<void(FRuntimeErrorEventArgs&)>;


	struct FProgressEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FProgressEventArgs(const std::string& fileName, float progress, bool cancel = false)
			: FileName(fileName)
			, Progress(progress)
			, Cancel(cancel)
		{}

		// The file that is being loaded.
		std::string FileName;
		// The progress of the loading process.
		float Progress;
		// Set to TRUE to cancel loading.
		bool Cancel;
	};

	using FProgressEvent = TMulticastDelegate<void(FProgressEventArgs&)>;
	using FProgressEventDelegate = TDelegate<void(FProgressEventArgs&)>;


	enum class EFileAction
	{
		Unknown,        // An unknown action triggered this event. (Should not happen, but I guess its possible)
		Added,          // A file was added to a directory.
		Removed,        // A file was removed from a directory.
		Modified,       // A file was modified. This might indicate the file's timestamp was modified or another attribute was modified.
		RenameOld,      // The file was renamed and this event stores the previous name.
		RenameNew,      // The file was renamed and this event stores the new name.
	};

	struct FFileChangeEventArgs : FEventArgs
	{
	public:
		using base = FEventArgs;

		FFileChangeEventArgs(EFileAction action, const std::string& path)
			: Action(action)
			, Path(path)
		{}

		EFileAction Action; // The action that triggered this event.
						   // The file or directory path that was modified.
		std::string Path;
	};

	using FFileChangeEvent = TMulticastDelegate<void(FFileChangeEventArgs&)>;
	using FFileChangeEventDelegate = TDelegate<void(FFileChangeEventArgs&)>;
}
