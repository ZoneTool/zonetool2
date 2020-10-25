#pragma once

#include <cstdint>
#include <assert.h>
#include <stack>
#include <string>
#include <vector>

template <typename ... Args>
std::string va(const std::string& format, Args ... args)
{
	size_t size = _snprintf(nullptr, 0, format.c_str(), args ...) + 1;
	std::vector<char> buf;
	buf.resize(size);
	_snprintf(buf.data(), size, format.c_str(), args ...);
	return std::string(buf.data(), buf.data() + size - 1);
}
