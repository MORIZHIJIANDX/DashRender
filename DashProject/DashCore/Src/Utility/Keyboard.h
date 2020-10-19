#pragma once
#include <bitset>
#include <optional>
#include "KeyCodes.h"

namespace Dash
{
	class FKeyboard
	{
		friend class FWindow;
		friend class FApplication;

		friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	public:
		static FKeyboard& Get();

		bool IsKeyPressed(EKeyCode key) const;

		FKeyState GetKeyState(EKeyCode key) const;

		std::optional<char> ReadChar();

		void EnableAutoRepeat();
		void DisableAutoRepeat();
		bool IsAutoRepeatEnabled() const;

	public:
		FKeyboardEvent KeyPressed;
		FKeyboardEvent KeyReleased;

	private:
		FKeyboard() {};
		~FKeyboard() {};

		void OnKeyPressed(FKeyEventArgs& e);
		void OnKeyReleased(FKeyEventArgs& e);
		void OnChar(char character);

		void FlushCharBuffer();

		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer);

		void ClearStates();

		bool CheckKeyPressed(BYTE value) { return (value & 0x80) > 0; };
		
		void FlushRepeatKey();
	private:

		FKeyState mPrevKeyStates[256];
		std::queue<char> mCharBuffer;
		bool mAutoRepeat = false;
	};
}