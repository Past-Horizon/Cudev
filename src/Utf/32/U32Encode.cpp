#include <Cudev/Utf/32/U32Codec.h>

#include <Cudev/Utf/Common/ScalarValidation.h>

namespace Cudev::Utf32 {

Result<std::u32string, CodecError> U32Codec::Encode(std::u32string_view input) const
{
    std::u32string codePoints;
    codePoints.reserve(input.size());

    for (const char32_t codePoint : input)
    {
        if (const auto error = Utf::ValidateScalarValue(codePoint))
        {
            return Result<std::u32string, CodecError>::failure(*error);
        }

        codePoints.push_back(codePoint);
    }

    return Result<std::u32string, CodecError>::success(std::move(codePoints));
}

}
