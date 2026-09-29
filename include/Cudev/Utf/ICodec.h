/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

#include <Cudev/Core/CodecError.h>
#include <Cudev/Core/Result.h>

namespace Cudev::Utf {

/*
    @summary
    Defines the interface for an encoded Unicode string.

    @param EncodedString
    The encoded string type.

    @param EncodedView
    The encoded input view type.

    @param UnicodeString
    The Unicode scalar string type.

    @param UnicodeView
    The Unicode scalar input view type.
*/
template <
    typename EncodedString,
    typename EncodedView,
    typename UnicodeString,
    typename UnicodeView>
class ICodec
{
public:
    /*
        @summary
        Releases the codec interface.
    */
    virtual ~ICodec() = default;

    /*
        @summary
        Encodes Unicode scalar values.

        @param input
        The Unicode scalar values to encode.

        @returns
        The encoded string or a codec error.
    */
    virtual Result<EncodedString, CodecError> Encode(UnicodeView input) const = 0;

    /*
        @summary
        Decodes an encoded string to Unicode scalar values.

        @param input
        The encoded string to decode.

        @returns
        The decoded Unicode scalar values or a codec error.
    */
    virtual Result<UnicodeString, CodecError> Decode(EncodedView input) const = 0;

    /*
        @summary
        Validates an encoded string without returning decoded data.

        @param input
        The encoded string to validate.

        @returns
        A successful result or a codec error.
    */
    virtual Result<void, CodecError> Validate(EncodedView input) const = 0;
};

}