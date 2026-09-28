#pragma once

namespace Cudev {

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