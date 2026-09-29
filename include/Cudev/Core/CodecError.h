/*
MIT License — Copyright (c) 2026 Cudev
See LICENSE file in the project root for full license text.
*/

#pragma once

namespace Cudev {

/*
    @summary
    Identifies a Unicode codec failure.
*/
enum class CodecError
{
    InvalidLeadingByte,
    InvalidContinuationByte,
    TruncatedSequence,
    OverlongEncoding,
    SurrogateCodePoint,
    CodePointOutOfRange
};

}