#include <Cudev/Utf/8/U8Codec.h>

#include <Cudev/Utf/Common/ScalarValidation.h>

namespace Cudev::Utf8 {

Result<std::string, CodecError> U8Codec::Encode(std::u32string_view input) const
{
    std::string bytes;
    bytes.reserve(input.size());

    for (const char32_t codePoint : input)
    {
        if (const auto error = Utf::ValidateScalarValue(codePoint))
        {
            return Result<std::string, CodecError>::failure(*error);
        }

        if (codePoint <= 0x7F)
        {
            bytes.push_back(static_cast<char>(codePoint));
        }
        else if (codePoint <= 0x7FF)
        {
            bytes.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
            bytes.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        }
        else if (codePoint <= 0xFFFF)
        {
            bytes.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
            bytes.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
            bytes.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        }
        else
        {
            bytes.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
            bytes.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
            bytes.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
            bytes.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
        }
    }

    return Result<std::string, CodecError>::success(std::move(bytes));
}

}