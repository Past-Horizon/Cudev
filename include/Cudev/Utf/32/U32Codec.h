/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <Cudev/Utf/ICodec.h>

#include <string>
#include <string_view>

namespace Cudev::Utf32 {

/*
    @summary
    Encodes and decodes UTF-32 strings.

    @details
    UTF-32 values are validated as Unicode scalar values.
*/
class U32Codec final
    : public Utf::ICodec<
          std::u32string,
          std::u32string_view,
          std::u32string,
          std::u32string_view>
{
public:
    /*
        @summary
        Encodes Unicode scalar values as UTF-32.

        @param input
        The Unicode scalar values to encode.

        @returns
        The UTF-32 string or a codec error.
    */
    Result<std::u32string, CodecError> Encode(std::u32string_view input) const override;

    /*
        @summary
        Decodes UTF-32 to Unicode scalar values.

        @param input
        The UTF-32 string to decode.

        @returns
        The decoded Unicode scalar values or a codec error.
    */
    Result<std::u32string, CodecError> Decode(std::u32string_view input) const override;

    /*
        @summary
        Validates a UTF-32 string.

        @param input
        The UTF-32 string to validate.

        @returns
        A successful result or a codec error.
    */
    Result<void, CodecError> Validate(std::u32string_view input) const override;
};

}
