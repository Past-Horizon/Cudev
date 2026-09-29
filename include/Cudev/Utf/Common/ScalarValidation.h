#pragma once

#include <Cudev/Core/CodecError.h>

#include <optional>

namespace Cudev::Utf {

constexpr std::optional<CodecError> ValidateScalarValue(char32_t codePoint) noexcept
{
    if (codePoint > 0x10FFFF)
    {
        return CodecError::CodePointOutOfRange;
    }

    if (codePoint >= 0xD800 && codePoint <= 0xDFFF)
    {
        return CodecError::SurrogateCodePoint;
    }

    return std::nullopt;
}

}