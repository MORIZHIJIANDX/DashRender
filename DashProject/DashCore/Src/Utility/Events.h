#pragma once

#include "KeyCodes.h"
#include "../DesignPatterns/MulticastDelegate.h"

#include <memory>
#include <string>

namespace Graphics
{
	class FCamera;
	class GraphicsCommandBuffer;
}

namespace Dash
{
	class FEventArgs
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

	class FKeyEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FKeyEventArgs(EKeyCode key, unsigned int c, EKeyState state, bool control, bool shift, bool alt, bool repeat)
			: mKey(key)
			, mChar(c)
			, mState(state)
			, mControl(control)
			, mShift(shift)
			, mAlt(alt)
			, mRepeat(repeat)
		{}

		EKeyCode        mKey;    // The Key Code that was pressed or released.
		unsigned int    mChar;   // The 32-bit character code that was pressed. This value will be 0 if it is a non-printable character.
		EKeyState       mState;  // Was the key pressed or released?
		bool            mControl;// Is the Control modifier pressed
		bool            mShift;  // Is the Shift modifier pressed
		bool            mAlt;    // Is the Alt modifier pressed
		bool			mRepeat;
	};

	using FKeyboardEvent = TMulticastDelegate<void(FKeyEventArgs&)>;
	using FKeyboardEventDelegate = TDelegate<void(FKeyEventArgs&)>;


	class FMouseMotionEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FMouseMotionEventArgs(bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
			: mLeftButton(leftButton)
			, mMiddleButton(middleButton)
			, mRightButton(rightButton)
			, mControl(control)
			, mShift(shift)
			, mX(x)
			, mY(y)
			, mRelX(0)
			, mRelY(0)
		{}

		bool mLeftButton;    // Is the left mouse button down?
		bool mMiddleButton;  // Is the middle mouse button down?
		bool mRightButton;   // Is the right mouse button down?
		bool mControl;       // Is the CTRL key down?
		bool mShift;         // Is the Shift key down?

		int mX;              // The X-position of the cursor relative to the upper-left corner of the client area.
		int mY;              // The Y-position of the cursor relative to the upper-left corner of the client area.
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

	class FMouseButtonEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FMouseButtonEventArgs(EMouseButton buttonID, EButtonState state, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
			: mButton(buttonID)
			, mState(state)
			, mLeftButton(leftButton)
			, mMiddleButton(middleButton)
			, mRightButton(rightButton)
			, mControl(control)
			, mShift(shift)
			, mX(x)
			, mY(y)
		{}

		EMouseButton mButton; // The mouse button that was pressed or released.
		EButtonState mState;  // Was the button pressed or released?
		bool mLeftButton;    // Is the left mouse button down?
		bool mMiddleButton;  // Is the middle mouse button down?
		bool mRightButton;   // Is the right mouse button down?
		bool mControl;       // Is the CTRL key down?
		bool mShift;         // Is the Shift key down?

		int mX;              // The X-position of the cursor relative to the upper-left corner of the client area.
		int mY;              // The Y-position of the cursor relative to the upper-left corner of the client area.
	};

	using FMouseButtonEvent = TMulticastDelegate<void(FMouseButtonEventArgs&)>;
	using FMouseButtonEventDelegate = TDelegate<void(FMouseButtonEventArgs&)>;


	class FMouseWheelEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FMouseWheelEventArgs(float wheelDelta, bool leftButton, bool middleButton, bool rightButton, bool control, bool shift, int x, int y)
			: mWheelDelta(wheelDelta)
			, mLeftButton(leftButton)
			, mMiddleButton(middleButton)
			, mRightButton(rightButton)
			, mControl(control)
			, mShift(shift)
			, mX(x)
			, mY(y)
		{}

		float mWheelDelta;   // How much the mouse wheel has moved. A positive value indicates that the wheel was moved to the right. A negative value indicates the wheel was moved to the left.
		bool mLeftButton;    // Is the left mouse button down?
		bool mMiddleButton;  // Is the middle mouse button down?
		bool mRightButton;   // Is the right mouse button down?
		bool mControl;       // Is the CTRL key down?
		bool mShift;         // Is the Shift key down?

		int mX;              // The X-position of the cursor relative to the upper-left corner of the client area.
		int mY;              // The Y-position of the cursor relative to the upper-left corner of the client area.

	};

	using FMouseWheelEvent = TMulticastDelegate<void(FMouseWheelEventArgs&)>;
	using FMouseWheelEventDelegate = TDelegate<void(FMouseWheelEventArgs&)>;


	class FResizeEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FResizeEventArgs(int width, int height, bool minimize)
			: mWidth(width)
			, mHeight(height)
			, mMinimized(minimize)
		{}

		// The new width of the window
		int mWidth;
		// The new height of the window.
		int mHeight;

		bool mMinimized;
	};

	using FResizeEvent = TMulticastDelegate<void(FResizeEventArgs&)>;
	using FResizeEventDelegate = TDelegate<void(FResizeEventArgs&)>;


	class FUpdateEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;
		FUpdateEventArgs(double fDeltaTime, double fTotalTime, uint64_t frameCounter)
			: mElapsedTime(fDeltaTime)
			, mTotalTime(fTotalTime)
			, mFrameCounter(frameCounter)
		{}

		double mElapsedTime;
		double mTotalTime;
		uint64_t mFrameCounter;
	};

	using FUpdateEvent = TMulticastDelegate<void(FUpdateEventArgs&)>;
	using FUpdateEventDelegate = TDelegate<void(FUpdateEventArgs&)>;


	class FRenderEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;
		FRenderEventArgs(double fDeltaTime, double fTotalTime,
			uint64_t frameCounter,
			std::shared_ptr<Graphics::FCamera> camera = nullptr,
			std::shared_ptr<Graphics::GraphicsCommandBuffer> graphicsCommandBuffer = nullptr)
			: mElapsedTime(fDeltaTime)
			, mTotalTime(fTotalTime)
			, mFrameCounter(frameCounter)
			, mCamera(camera)
			, mGraphicsCommandBuffer(graphicsCommandBuffer)
		{}

		double mElapsedTime;
		double mTotalTime;
		uint64_t mFrameCounter;

		std::shared_ptr<Graphics::FCamera> mCamera;
		std::shared_ptr<Graphics::GraphicsCommandBuffer> mGraphicsCommandBuffer;
	};

	using FRenderEvent = TMulticastDelegate<void(FRenderEventArgs&)>;
	using FRenderEventDelegate = TDelegate<void(FRenderEventArgs&)>;


	class FUserEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;
		FUserEventArgs(int code, void* data1, void* data2)
			: mCode(code)
			, mData1(data1)
			, mData2(data2)
		{}

		int mCode;
		void* mData1;
		void* mData2;
	};

	using FUserEvent = TMulticastDelegate<void(FUserEventArgs&)>;
	using FUserEventDelegate = TDelegate<void(FUserEventArgs&)>;


	class FRuntimeErrorEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FRuntimeErrorEventArgs(const std::wstring& errorString, const std::wstring& compilerError)
			: mErrorString(errorString)
			, mCompilerError(compilerError)
		{}

		std::wstring mErrorString;
		std::wstring mCompilerError;
	};

	using FRuntimeErrorEvent = TMulticastDelegate<void(FRuntimeErrorEventArgs&)>;
	using FRuntimeErrorEventDelegate = TDelegate<void(FRuntimeErrorEventArgs&)>;


	class FProgressEventArgs : public FEventArgs
	{
	public:
		using base = FEventArgs;

		FProgressEventArgs(const std::wstring& fileName, float progress, bool cancel = false)
			: mFileName(fileName)
			, mProgress(progress)
			, mCancel(cancel)
		{}

		// The file that is being loaded.
		std::wstring mFileName;
		// The progress of the loading process.
		float mProgress;
		// Set to TRUE to cancel loading.
		bool mCancel;
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

	class FFileChangeEventArgs : FEventArgs
	{
	public:
		using base = FEventArgs;

		FFileChangeEventArgs(EFileAction action, const std::wstring& path)
			: mAction(action)
			, mPath(path)
		{}

		EFileAction mAction; // The action that triggered this event.
						   // The file or directory path that was modified.
		std::wstring mPath;
	};

	using FFileChangeEvent = TMulticastDelegate<void(FFileChangeEventArgs&)>;
	using FFileChangeEventDelegate = TDelegate<void(FFileChangeEventArgs&)>;
}
