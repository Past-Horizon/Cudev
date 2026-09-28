#pragma once

#include <Cudev/Core/CodecError.h>
#include <Cudev/Core/Result.h>

namespace Cudev::Utf {

template <
    typename EncodedString,
    typename EncodedView,
    typename UnicodeString,
    typename UnicodeView>
class ICodec
{
public:
    virtual ~ICodec() = default;

    virtual Result<EncodedString, CodecError> Encode(UnicodeView input) const = 0;
    virtual Result<UnicodeString, CodecError> Decode(EncodedView input) const = 0;
    virtual Result<void, CodecError> Validate(EncodedView input) const = 0;
};

}