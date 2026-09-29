/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <Cudev/Utf/ICodec.h>

#include <string>
#include <string_view>

namespace Cudev::Utf8 {

/*
    @summary
    Encodes and decodes UTF-8 strings.

    @details
    Unicode scalar values are represented as std::u32string values.
*/
class U8Codec final
    : public Utf::ICodec<
          std::string,
          std::string_view,
          std::u32string,
          std::u32string_view>
{
public:
    /*
        @summary
        Encodes Unicode scalar values as UTF-8.

        @param input
        The Unicode scalar values to encode.

        @returns
        The UTF-8 string or a codec error.
    */
    Result<std::string, CodecError> Encode(std::u32string_view input) const override;

    /*
        @summary
        Decodes UTF-8 to Unicode scalar values.

        @param input
        The UTF-8 string to decode.

        @returns
        The decoded Unicode scalar values or a codec error.
    */
    Result<std::u32string, CodecError> Decode(std::string_view input) const override;

    /*
        @summary
        Validates a UTF-8 string.

        @param input
        The UTF-8 string to validate.

        @returns
        A successful result or a codec error.
    */
    Result<void, CodecError> Validate(std::string_view input) const override;
};

}