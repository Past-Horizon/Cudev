#pragma once

#include <Cudev/Core/CodecError.h>
#include <Cudev/Core/Result.h>

#include <string>
#include <string_view>

namespace Cudev::Wide {

Result<std::string, CodecError> ToUtf8(std::wstring_view input);
Result<std::u16string, CodecError> ToUtf16(std::wstring_view input);
Result<std::u32string, CodecError> ToUtf32(std::wstring_view input);

Result<std::wstring, CodecError> FromUtf8(std::string_view input);
Result<std::wstring, CodecError> FromUtf16(std::u16string_view input);
Result<std::wstring, CodecError> FromUtf32(std::u32string_view input);

}
