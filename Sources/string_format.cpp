#include <string.h>
#include <stdarg.h>  // for va_start, etc
#include <memory>    // for std::unique_ptr
#include <stdlib.h>
#include <stdio.h>

#include "string_format.h"
/*
va_start can't use fmt_str as reference, so * :(
*/
std::string string_format(const char * _fmt_str, ...) {
	std::string result;
    va_list ap;
	va_start(ap, _fmt_str);
	result = vastring_format(_fmt_str, ap);
	va_end(ap);
    return result;
}
#ifndef __APPLE__
std::string vastring_format(const char * _fmt_str, va_list ap) {
	std::string fmt_str = std::string(_fmt_str);
	int final_n, n = ((int)fmt_str.size()) * 2; /* reserve 2 times as much as the length of the fmt_str */
	std::unique_ptr<char[]> formatted;
	while (1) {
		formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
									  //strcpy(&formatted[0], _fmt_str); //?
		final_n = vsnprintf(&formatted[0], n, _fmt_str, ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}
#else
std::string vastring_format(const char * _fmt_str, va_list ap) {

	return std::string(_fmt_str);
}
#endif