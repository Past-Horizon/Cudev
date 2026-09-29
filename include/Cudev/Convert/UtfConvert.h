/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <Cudev/Core/CodecError.h>
#include <Cudev/Core/Result.h>

#include <string>
#include <string_view>

namespace Cudev::Convert {

/*
	@summary
	Converts UTF-16 to UTF-8.

	@param input
	The UTF-16 string to convert.

	@returns
	The UTF-8 string or a codec error.
*/
Result<std::string, CodecError> ToUtf8(std::u16string_view input);

/*
	@summary
	Converts UTF-32 to UTF-8.

	@param input
	The UTF-32 string to convert.

	@returns
	The UTF-8 string or a codec error.
*/
Result<std::string, CodecError> ToUtf8(std::u32string_view input);

/*
	@summary
	Converts UTF-8 to UTF-16.

	@param input
	The UTF-8 string to convert.

	@returns
	The UTF-16 string or a codec error.
*/
Result<std::u16string, CodecError> ToUtf16(std::string_view input);

/*
	@summary
	Converts UTF-32 to UTF-16.

	@param input
	The UTF-32 string to convert.

	@returns
	The UTF-16 string or a codec error.
*/
Result<std::u16string, CodecError> ToUtf16(std::u32string_view input);

/*
	@summary
	Converts UTF-8 to UTF-32.

	@param input
	The UTF-8 string to convert.

	@returns
	The UTF-32 string or a codec error.
*/
Result<std::u32string, CodecError> ToUtf32(std::string_view input);

/*
	@summary
	Converts UTF-16 to UTF-32.

	@param input
	The UTF-16 string to convert.

	@returns
	The UTF-32 string or a codec error.
*/
Result<std::u32string, CodecError> ToUtf32(std::u16string_view input);

}
