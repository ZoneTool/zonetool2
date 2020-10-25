#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#undef min
#undef max

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <functional>

#include <stdint.h>

// #include <chaiscript/chaiscript.hpp>

using namespace std::literals;

template <typename ... Args>
std::string va(const std::string& format, Args ... args)
{
	size_t size = _snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::vector<char> buf;
	buf.resize(size);
	_snprintf(buf.data(), size, format.c_str(), args ...);
	return std::string(buf.data(), buf.data() + size - 1);
}

static void replace(std::string& str, const std::string& from, const std::string& to) 
{
	auto start_pos = str.find(from);
	if (start_pos == std::string::npos)
	{
		return;
	}

	str.replace(start_pos, from.length(), to);
	return replace(str, from, to);
}

static std::string to_lower(const std::string& input)
{
	auto result = input;
	std::transform(input.begin(), input.end(), result.begin(), [](auto in)
	{
		return tolower(in);
	});
	return result;
}

static std::vector<std::string> split(const std::string& rawInput, const std::vector<char>& delims)
{
	std::vector<std::string> strings;

	auto findFirstDelim = [](const std::string& input, const std::vector<char>& delims) -> std::pair<char, std::size_t>
	{
		auto firstDelim = 0;
		auto firstDelimIndex = static_cast<std::size_t>(-1);
		auto index = 0u;

		for (auto& delim : delims)
		{
			if ((index = input.find(delim)) != std::string::npos)
			{
				if (firstDelimIndex == -1 || index < firstDelimIndex)
				{
					firstDelim = delim;
					firstDelimIndex = index;
				}
			}
		}

		return { firstDelim, firstDelimIndex };
	};

	std::string input = rawInput;

	while (!input.empty())
	{
		auto splitDelim = findFirstDelim(input, delims);
		if (splitDelim.first != 0)
		{
			strings.push_back(input.substr(0, splitDelim.second));
			input = input.substr(splitDelim.second + 1);
		}
		else
		{
			break;
		}
	}

	strings.push_back(input);
	return strings;
}

static std::vector<std::string> split(std::string& str, char delimiter)
{
	return split(str, std::vector<char>({ delimiter }));
}


#define ZONETOOL_INFO(__FMT__,...) \
	printf("[ INFO ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_ERROR(__FMT__,...) \
	printf("[ ERROR ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_FATAL(__FMT__,...) \
	printf("[ FATAL ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__); \
	MessageBoxA(nullptr, &va("Oops! An unexpected error occured. Error was: " __FMT__ "\n\nZoneTool must be restarted to resolve the error. Last error code reported by windows: 0x%08X (%u)", __VA_ARGS__, GetLastError(), GetLastError())[0], nullptr, 0); \
	std::exit(0)

#define ZONETOOL_WARNING(__FMT__,...) \
	printf("[ WARNING ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)