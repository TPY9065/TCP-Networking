#pragma once

#include <iostream>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif // _WIN32

#define ASIO_STANDALONE
#include "asio.hpp"