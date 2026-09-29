/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <Cudev/Utf/ICodec.h>

#include <string>
#include <string_view>

namespace Cudev::Utf16 {

/*
    @summary
    Encodes and decodes UTF-16 strings.

    @details
    Unicode scalar values are represented as std::u32string values.
*/
class U16Codec final
    : public Utf::ICodec<
          std::u16string,
          std::u16string_view,
          std::u32string,
          std::u32string_view>
{
public:
    /*
        @summary
        Encodes Unicode scalar values as UTF-16.

        @param input
        The Unicode scalar values to encode.

        @returns
        The UTF-16 string or a codec error.
    */
    Result<std::u16string, CodecError> Encode(std::u32string_view input) const override;

    /*
        @summary
        Decodes UTF-16 to Unicode scalar values.

        @param input
        The UTF-16 string to decode.

        @returns
        The decoded Unicode scalar values or a codec error.
    */
    Result<std::u32string, CodecError> Decode(std::u16string_view input) const override;

    /*
        @summary
        Validates a UTF-16 string.

        @param input
        The UTF-16 string to validate.

        @returns
        A successful result or a codec error.
    */
    Result<void, CodecError> Validate(std::u16string_view input) const override;
};

}