#pragma once

#include "ElkTools/export.h"

#include <iostream>

#ifdef _WIN32 // Windows OS both 32 and 64 bit

#include <windows.h>
#define COLOR_BLUE ElkTools::Debug::blue
#define COLOR_RED ElkTools::Debug::red
#define COLOR_GREEN ElkTools::Debug::green
#define COLOR_YELLOW ElkTools::Debug::yellow
#define COLOR_WHITE ElkTools::Debug::white
#define COLOR_DEFAULT ElkTools::Debug::grey

namespace ElkTools
{
	namespace Debug
	{
		inline std::ostream& blue(std::ostream& s)
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout,
				FOREGROUND_BLUE | FOREGROUND_GREEN |
				FOREGROUND_INTENSITY);
			return s;
		}

		inline std::ostream& red(std::ostream& s)
		{
			HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE);
			SetConsoleTextAttribute(hStdout,
				FOREGROUND_RED | FOREGROUND_INTENSITY);
			return s;
		}

		inline std::ostream& green(std::ostream& s)
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout,
				FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			return s;
		}

		inline std::ostream& yellow(std::ostream& s)
		{
			HANDLE hStdout = GetStdHandle(STD_ERROR_HANDLE);
			SetConsoleTextAttribute(hStdout,
				FOREGROUND_GREEN | FOREGROUND_RED |
				FOREGROUND_INTENSITY);
			return s;
		}

		inline std::ostream& white(std::ostream& s)
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout,
				FOREGROUND_RED | FOREGROUND_GREEN |
				FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			return s;
		}

		inline std::ostream& grey(std::ostream& s)
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout,
				FOREGROUND_RED | FOREGROUND_GREEN |
				FOREGROUND_BLUE);
			return s;
		}

		struct color
		{
			color(WORD attribute) : m_color(attribute)
			{
			}

			WORD m_color;
		};

		template <class _Elem, class _Traits>
		std::basic_ostream<_Elem, _Traits>&
			operator<<(std::basic_ostream<_Elem, _Traits>& i, color& c)
		{
			HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hStdout, c.m_color);
			return i;
		}

#elif __unix__

#define COLOR_BLUE "\033[0;34m"
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_WHITE "\033[0;37m"
#define COLOR_DEFAULT "\033[0m"

#endif // OS selection

	}
}