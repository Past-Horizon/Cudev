# Cudev - Cross-Platform Unicode Codecs

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey)](Compile.md)
[![C++](https://img.shields.io/badge/C%2B%2B-20-blue)](Compile.md)

Cudev is a small C++ library for Unicode conversion.

It supports UTF-8, UTF-16, UTF-32, and platform wide strings.

Conversion errors are returned through `Cudev::Result` instead of exceptions or silent replacement characters.

## Design Goals

* **Correct**: validates Unicode scalar values, rejects malformed input, and preserves embedded null characters.
* **Cross-platform**: handles the different `wchar_t` widths on Windows and Linux without platform-specific conversion APIs.
* **Small**: keeps the encoding code focused and reusable.
* **Consistent**: uses the same UTF codec implementations across conversion types.

## Features

* UTF-8 encoding, decoding, and validation
* UTF-16 encoding, decoding, and validation
* UTF-32 encoding, decoding, and validation
* Conversion between UTF-8, UTF-16, and UTF-32
* Conversion between UTF encodings and `std::wstring`
* Errors for invalid leading bytes, invalid continuation bytes, truncated sequences, overlong encodings, surrogate code points, and out-of-range code points

## Quick Example

```cpp
#include <Cudev/Cudev.h>
#include <iostream>
#include <string>

int main()
{
    const std::u16string input = u"Hello, 世界!";
    const auto converted = Cudev::Convert::ToUtf8(input);

    if (converted.failed())
    {
        std::cerr << "UTF-8 conversion failed\n";
        return 1;
    }

    std::cout << converted.value() << '\n';
    return 0;
}
```

Wide-string conversion uses the same error handling:

```cpp
const auto wide = Cudev::Wide::FromUtf8("Hello, world!");

if (wide.succeeded())
{
    const std::wstring value = wide.value();
}
```

All conversion functions accept string views and return `Cudev::Result<T, Cudev::CodecError>`.

The conversion layer decodes input into Unicode scalar values, then encodes them into the requested format.

## Documentation

Cudev currently does not have a documentation website, but you can review .h files to read documentat

## License

Cudev uses the MIT license. See [LICENSE](LICENSE) for details.
