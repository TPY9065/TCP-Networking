#pragma once

#include <iostream>

#include <type_traits>
namespace net
{
	template<typename T>
	// Protocal should be a POD type
	concept Protocal = std::is_standard_layout<T>::value && std::is_trivial<T>::value;
}

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif // _WIN32

#define ASIO_STANDALONE
#include "asio.hpp"