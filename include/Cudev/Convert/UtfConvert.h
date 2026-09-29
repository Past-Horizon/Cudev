#pragma once

#include <Cudev/Core/CodecError.h>
#include <Cudev/Core/Result.h>

#include <string>
#include <string_view>

namespace Cudev::Convert {

Result<std::string, CodecError> ToUtf8(std::u16string_view input);
Result<std::string, CodecError> ToUtf8(std::u32string_view input);

Result<std::u16string, CodecError> ToUtf16(std::string_view input);
Result<std::u16string, CodecError> ToUtf16(std::u32string_view input);

Result<std::u32string, CodecError> ToUtf32(std::string_view input);
Result<std::u32string, CodecError> ToUtf32(std::u16string_view input);

}
