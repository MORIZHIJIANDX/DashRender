#include "PCH.h"

#include "Keyboard.h"
#include "LogManager.h"

namespace Dash
{
	FKeyboard& FKeyboard::Get()
	{
		static FKeyboard instance;
		return instance;
	}

	bool FKeyboard::IsKeyPressed(EKeyCode key) const
	{
		//return mKeyStates[static_cast<unsigned int>(key)];

		return mPrevKeyStates[static_cast<unsigned int>(key)].Pressed;

		//return mPrevState.KeyStates[static_cast<unsigned int>(key)].Pressed;
	}

	FKeyState FKeyboard::GetKeyState(EKeyCode key) const
	{
		return mPrevKeyStates[static_cast<unsigned int>(key)];

		//return mPrevState.KeyStates[static_cast<unsigned int>(key)];
	}

	std::optional<char> FKeyboard::ReadChar()
	{
		if (mCharBuffer.size() > 0u)
		{
			unsigned char character = mCharBuffer.front();
			mCharBuffer.pop();
			return character;
		}
		return {};
	}

	void FKeyboard::OnKeyPressed(FKeyEventArgs& e)
	{
		if (!e.Repeat || IsAutoRepeatEnabled())
		{
			//mKeyStates[static_cast<unsigned int>(e.Key)] = true;

 			FKeyState currentKeyState;
			currentKeyState.Pressed = true;
			currentKeyState.RisingEdge =
				currentKeyState.Pressed && !mPrevKeyStates[static_cast<unsigned int>(e.Key)].Pressed;
			currentKeyState.FallingEdge =
				!currentKeyState.Pressed && mPrevKeyStates[static_cast<unsigned int>(e.Key)].Pressed;

			mPrevKeyStates[static_cast<unsigned int>(e.Key)] = currentKeyState;
		}
		
		if (e.Char != 0)
		{
			OnChar(e.Char);
		}

		KeyPressed(e);
	}

	void FKeyboard::OnKeyReleased(FKeyEventArgs& e)
	{
		FKeyState currentKeyState;
		currentKeyState.Pressed = false;
		currentKeyState.RisingEdge =
			currentKeyState.Pressed && !mPrevKeyStates[static_cast<unsigned int>(e.Key)].Pressed;
		currentKeyState.FallingEdge =
			!currentKeyState.Pressed && mPrevKeyStates[static_cast<unsigned int>(e.Key)].Pressed;

		mPrevKeyStates[static_cast<unsigned int>(e.Key)] = currentKeyState;

		KeyReleased(e);
	}


	void FKeyboard::OnChar(char character)
	{
		mCharBuffer.push(character);
		TrimBuffer(mCharBuffer);
	}

	template<typename T>
	inline void FKeyboard::TrimBuffer(std::queue<T>& buffer)
	{
		static const size_t bufferSize = 16;
		while (buffer.size() > bufferSize)
		{
			buffer.pop();
		}
	}

	void FKeyboard::ClearStates()
	{
		//mKeyStates.reset();
		for (size_t i = 0; i < 256; i++)
		{
			mPrevKeyStates[i].Pressed = false;
			mPrevKeyStates[i].RisingEdge = false;
			mPrevKeyStates[i].FallingEdge = false;
		}
	}

	void FKeyboard::FlushRepeatKey()
	{
		for (size_t i = 0; i < 256; i++)
		{
			mPrevKeyStates[i].RisingEdge = false;
			mPrevKeyStates[i].FallingEdge = false;
		}
	}

	void FKeyboard::FlushCharBuffer()
	{
		mCharBuffer = std::queue<char>();
	}

	void FKeyboard::EnableAutoRepeat()
	{
		mAutoRepeat = true;
	}

	void FKeyboard::DisableAutoRepeat()
	{
		mAutoRepeat = false;
	}

	bool FKeyboard::IsAutoRepeatEnabled() const
	{
		return mAutoRepeat;
	}
}