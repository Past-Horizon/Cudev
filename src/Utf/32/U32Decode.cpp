#include <Cudev/Utf/32/U32Codec.h>

#include <Cudev/Utf/Common/ScalarValidation.h>

namespace Cudev::Utf32 {

Result<std::u32string, CodecError> U32Codec::Decode(std::u32string_view input) const
{
    const auto validation = Validate(input);
    if (validation.failed())
    {
        return Result<std::u32string, CodecError>::failure(*validation.error());
    }

    return Result<std::u32string, CodecError>::success(std::u32string(input));
}

Result<void, CodecError> U32Codec::Validate(std::u32string_view input) const
{
    for (const char32_t codePoint : input)
    {
        if (const auto error = Utf::ValidateScalarValue(codePoint))
        {
            return Result<void, CodecError>::failure(*error);
        }
    }

    return Result<void, CodecError>::success();
}

}
