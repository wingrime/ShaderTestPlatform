#pragma once
#include <string.h>
#include <stdarg.h>  // for va_start, etc
#include <memory>    // for std::unique_ptr
#include <stdlib.h>
#include <stdio.h>
std::string string_format(const std::string fmt_str, ...);
