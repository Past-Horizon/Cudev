#include <Cudev/Utf/16/U16Codec.h>

#include <Cudev/Utf/Common/ScalarValidation.h>

namespace Cudev::Utf16 {

Result<std::u16string, CodecError> U16Codec::Encode(std::u32string_view input) const
{
    std::u16string codeUnits;
    codeUnits.reserve(input.size());

    for (const char32_t codePoint : input)
    {
        if (const auto error = Utf::ValidateScalarValue(codePoint))
        {
            return Result<std::u16string, CodecError>::failure(*error);
        }

        if (codePoint <= 0xFFFF)
        {
            codeUnits.push_back(static_cast<char16_t>(codePoint));
            continue;
        }

        const auto adjusted = codePoint - 0x10000;
        codeUnits.push_back(static_cast<char16_t>(0xD800 | (adjusted >> 10)));
        codeUnits.push_back(static_cast<char16_t>(0xDC00 | (adjusted & 0x3FF)));
    }

    return Result<std::u16string, CodecError>::success(std::move(codeUnits));
}

}