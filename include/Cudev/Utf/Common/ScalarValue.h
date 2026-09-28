#pragma once

#include <cstdint>

namespace Cudev::Utf {

constexpr bool IsScalarValue(char32_t codePoint) noexcept
{
    return codePoint <= 0x10FFFF && !(codePoint >= 0xD800 && codePoint <= 0xDFFF);
}

}