#pragma once

#include <cstdint>
#include <assert.h>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

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
