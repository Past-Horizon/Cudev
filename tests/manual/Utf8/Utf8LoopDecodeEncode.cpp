#include <Cudev/Utf/8/U8Codec.h>

#include <conio.h>
#include <cstdio>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

struct Sample
{
    std::string name;
    std::u32string value;
};

struct InvalidSample
{
    std::string name;
    std::string bytes;
    Cudev::CodecError error;
};

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string result;
    result.reserve(values.size());

    for (const auto value : values)
    {
        result.push_back(static_cast<char>(value));
    }

    return result;
}

const char* ErrorName(Cudev::CodecError error)
{
    switch (error)
    {
    case Cudev::CodecError::InvalidLeadingByte:
        return "InvalidLeadingByte";
    case Cudev::CodecError::InvalidContinuationByte:
        return "InvalidContinuationByte";
    case Cudev::CodecError::TruncatedSequence:
        return "TruncatedSequence";
    case Cudev::CodecError::OverlongEncoding:
        return "OverlongEncoding";
    case Cudev::CodecError::SurrogateCodePoint:
        return "SurrogateCodePoint";
    case Cudev::CodecError::CodePointOutOfRange:
        return "CodePointOutOfRange";
    }

    return "Unknown";
}

std::vector<Sample> BuildSamples()
{
    std::vector<Sample> samples;

    samples.push_back({"empty", {}});
    samples.push_back({"short-ascii", U"hello"});
    samples.push_back({"short-mixed", U"A\u00A3\u20AC\U0001F642"});
    samples.push_back({
        "short-boundaries",
        {0x0000, 0x007F, 0x0080, 0x07FF, 0x0800, 0xFFFF, 0x10000, 0x10FFFF}});
    samples.push_back({
        "short-weird-valid",
        {0x0001, 0x001B, 0x007F, 0x0080, 0x0301, 0x061F, 0x200B, 0xFEFF,
         0xFDD0, 0xFFFF, 0x10FFFE}});

    std::u32string medium;
    medium.reserve(4096);
    for (int index = 0; index < 512; ++index)
    {
        medium.append({U'A', 0, 0x007F, 0x0080, 0x0800, 0x10000, 0x10FFFF});
    }
    samples.push_back({"medium-mixed", std::move(medium)});

    std::u32string longValue;
    longValue.reserve(100000);
    for (int index = 0; index < 12000; ++index)
    {
        longValue.append({
            U'C', 0x00A3, 0x20AC, 0x1F642, 0x0080, 0x0800, 0x10FFFF});
    }
    samples.push_back({"long-mixed", std::move(longValue)});

    return samples;
}

std::vector<InvalidSample> BuildInvalidSamples()
{
    return {
        {"lone-continuation", Bytes({0x80}), Cudev::CodecError::InvalidLeadingByte},
        {"invalid-leading-c0", Bytes({0xC0, 0x80}), Cudev::CodecError::OverlongEncoding},
        {"invalid-leading-f5", Bytes({0xF5, 0x80, 0x80, 0x80}), Cudev::CodecError::InvalidLeadingByte},
        {"truncated-two-byte", Bytes({0xC2}), Cudev::CodecError::TruncatedSequence},
        {"truncated-three-byte", Bytes({0xE1, 0x80}), Cudev::CodecError::TruncatedSequence},
        {"truncated-four-byte", Bytes({0xF1, 0x80, 0x80}), Cudev::CodecError::TruncatedSequence},
        {"bad-continuation-first", Bytes({0xE1, 0x41, 0x80}), Cudev::CodecError::InvalidContinuationByte},
        {"bad-continuation-middle", Bytes({0xF1, 0x80, 0x41, 0x80}), Cudev::CodecError::InvalidContinuationByte},
        {"bad-continuation-last", Bytes({0xF1, 0x80, 0x80, 0x41}), Cudev::CodecError::InvalidContinuationByte},
        {"e0-overlong-with-trailing-byte", Bytes({0xE0, 0x80, 0x41}), Cudev::CodecError::OverlongEncoding},
        {"f0-overlong-with-trailing-byte", Bytes({0xF0, 0x80, 0x80, 0x41}), Cudev::CodecError::OverlongEncoding},
        {"ed-surrogate-with-trailing-byte", Bytes({0xED, 0xA0, 0x41}), Cudev::CodecError::SurrogateCodePoint},
        {"f4-out-of-range-with-trailing-byte", Bytes({0xF4, 0x90, 0x80, 0x41}), Cudev::CodecError::CodePointOutOfRange},
        {"valid-prefix-then-truncated", Bytes({0x41, 0xC2}), Cudev::CodecError::TruncatedSequence},
        {"valid-prefix-then-invalid", Bytes({0x41, 0xE1, 0x80, 0x41}), Cudev::CodecError::InvalidContinuationByte},
        {"invalid-prefix-then-valid", Bytes({0xE1, 0x80, 0x41, 0x41}), Cudev::CodecError::InvalidContinuationByte}
    };
}

std::string HexPreview(std::string_view value)
{
    std::string preview;
    const auto count = value.size() < 32 ? value.size() : 32;

    for (std::size_t index = 0; index < count; ++index)
    {
        if (index != 0)
        {
            preview.push_back(' ');
        }

        const auto byte = static_cast<unsigned char>(value[index]);
        char buffer[3] = {};
        std::snprintf(buffer, sizeof(buffer), "%02X", byte);
        preview.append(buffer);
    }

    if (value.size() > count)
    {
        preview.append(" ...");
    }

    return preview;
}

bool LogFailure(
    std::string_view logPath,
    std::size_t iteration,
    const Sample& sample,
    std::string_view stage,
    std::string_view detail)
{
    std::ofstream log(std::string(logPath), std::ios::app);
    if (!log)
    {
        std::cerr << "Unable to open failure log: " << logPath << '\n';
        return false;
    }

    log << "iteration=" << iteration << '\n'
        << "sample=" << sample.name << '\n'
        << "stage=" << stage << '\n'
        << "code_points=" << sample.value.size() << '\n'
        << "detail=" << detail << '\n' << '\n';

    return true;
}

class StopKeyPoller
{
public:
    bool StopRequested() const
    {
        if (!_kbhit())
        {
            return false;
        }

        const auto key = _getch();
        return key == 'g' || key == 'G';
    }
};

bool CheckSample(
    const Cudev::Utf8::U8Codec& codec,
    const Sample& sample,
    std::size_t iteration,
    std::string_view logPath)
{
    const auto encoded = codec.Encode(sample.value);
    if (encoded.failed())
    {
        const std::string detail = ErrorName(*encoded.error());
        LogFailure(logPath, iteration, sample, "encode", detail);
        return false;
    }

    const auto validation = codec.Validate(encoded.value());
    if (validation.failed())
    {
        const std::string detail = ErrorName(*validation.error());
        LogFailure(logPath, iteration, sample, "validate-encoded", detail);
        return false;
    }

    const auto decoded = codec.Decode(encoded.value());
    if (decoded.failed())
    {
        const std::string detail = ErrorName(*decoded.error());
        LogFailure(logPath, iteration, sample, "decode", detail);
        return false;
    }

    if (decoded.value() != sample.value)
    {
        LogFailure(logPath, iteration, sample, "compare", HexPreview(encoded.value()));
        return false;
    }

    return true;
}

bool CheckInvalidSample(
    const Cudev::Utf8::U8Codec& codec,
    const InvalidSample& sample,
    std::size_t iteration,
    std::string_view logPath)
{
    const auto validation = codec.Validate(sample.bytes);
    if (validation.succeeded() || !validation.error() || *validation.error() != sample.error)
    {
        LogFailure(logPath, iteration, {sample.name, {}}, "invalid-validate", sample.name);
        return false;
    }

    const auto decoded = codec.Decode(sample.bytes);
    if (decoded.succeeded() || !decoded.error() || *decoded.error() != sample.error)
    {
        LogFailure(logPath, iteration, {sample.name, {}}, "invalid-decode", sample.name);
        return false;
    }

    return true;
}

}

int main()
{
    const std::string logPath = "tests/out/Utf8LoopDecodeEncode.log";
    const auto samples = BuildSamples();
    const auto invalidSamples = BuildInvalidSamples();
    const Cudev::Utf8::U8Codec codec;
    StopKeyPoller stopKey;
    std::size_t iteration = 0;
    std::size_t sampleCount = 0;
    const auto start = std::chrono::steady_clock::now();

    std::cout << "Running UTF-8 loop. Press g to stop.\n"
              << "Failure details will be written to " << logPath << '\n';

    while (true)
    {
        bool stopRequested = false;
        for (const auto& sample : samples)
        {
            if (!CheckSample(codec, sample, iteration, logPath))
            {
                const auto elapsed = std::chrono::duration<double>(
                    std::chrono::steady_clock::now() - start).count();
                std::cerr << "UTF-8 stress test failed at iteration " << iteration
                          << ", sample " << sample.name << "\n"
                          << "Elapsed seconds: " << elapsed << '\n'
                          << "Details logged to " << logPath << '\n';
                std::cout << "Press Enter to exit.\n";
                std::cin.get();
                return 1;
            }

            ++sampleCount;
            if (stopKey.StopRequested())
            {
                stopRequested = true;
                break;
            }
        }

        if (stopRequested)
        {
            ++iteration;
            break;
        }

        for (const auto& sample : invalidSamples)
        {
            if (!CheckInvalidSample(codec, sample, iteration, logPath))
            {
                const auto elapsed = std::chrono::duration<double>(
                    std::chrono::steady_clock::now() - start).count();
                std::cerr << "UTF-8 stress test failed at iteration " << iteration
                          << ", invalid sample " << sample.name << "\n"
                          << "Elapsed seconds: " << elapsed << '\n'
                          << "Details logged to " << logPath << '\n';
                std::cout << "Press Enter to exit.\n";
                std::cin.get();
                return 1;
            }

            ++sampleCount;
            if (stopKey.StopRequested())
            {
                stopRequested = true;
                break;
            }
        }

        ++iteration;
        if (stopRequested || stopKey.StopRequested())
        {
            break;
        }
    }

    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();
    const auto samplesPerSecond = elapsed > 0.0 ? sampleCount / elapsed : 0.0;
    const auto iterationsPerSecond = elapsed > 0.0 ? iteration / elapsed : 0.0;

    std::cout << "UTF-8 stress loop stopped.\n"
              << "Iterations: " << iteration << '\n'
              << "Cases: " << sampleCount << '\n'
              << "Valid samples per iteration: " << samples.size() << '\n'
              << "Invalid samples per iteration: " << invalidSamples.size() << '\n'
              << "Elapsed seconds: " << elapsed << '\n'
              << "Iterations per second: " << iterationsPerSecond << '\n'
              << "Samples per second: " << samplesPerSecond << '\n'
              << "Press Enter to exit.\n";
    std::cin.get();
    return 0;
}
